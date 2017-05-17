/*  SpiralSound
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
#include "StreamPlugin.h"
#include "StreamPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"
#include <stdio.h>

#include "../../RiffWav.h"

using namespace std;

static const float TRIG_THRESH = 0.1;
static const float BUFSECONDS = 1.0f;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new StreamPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0119;
}

string SpiralPlugin_GetGroupName()
{
	return "Delay/Sampling";
}
}

///////////////////////////////////////////////////////

StreamPlugin::StreamPlugin() :
m_SampleRate (44100),
m_SampleSize (256),
m_StreamPos (0),
m_GlobalPos (0),
m_Pitch (1.0f),
m_SamplePos (-1),
m_Pos (0),
m_Mode(STOPM)
{
	m_PluginInfo.Name = "Stream";
	m_PluginInfo.Width = 245;
	m_PluginInfo.Height = 165;
	m_PluginInfo.NumInputs = 3;
	m_PluginInfo.NumOutputs = 3;
	m_PluginInfo.PortTips.push_back ("Pitch CV");
	m_PluginInfo.PortTips.push_back ("Play Trigger");
	m_PluginInfo.PortTips.push_back ("Stop Trigger");
	m_PluginInfo.PortTips.push_back ("Left Out");
	m_PluginInfo.PortTips.push_back ("Right Out");
	m_PluginInfo.PortTips.push_back ("Finish Trigger");
	m_GUIArgs.Volume = 1.0f;
	m_GUIArgs.PitchMod = 1.0f;
        m_GUIArgs.PlayOut = false;
	m_AudioCH->Register ("Volume", &m_GUIArgs.Volume);
	m_AudioCH->Register ("Pitch", &m_GUIArgs.PitchMod, ChannelHandler::INPUT);
	m_AudioCH->RegisterData ("FileName", ChannelHandler::INPUT,
                                             &m_GUIArgs.FileName, sizeof (m_GUIArgs.FileName));
	m_AudioCH->Register ("Time", &m_GUIArgs.Time);
	m_AudioCH->Register ("TimeOut", &m_GUIArgs.TimeOut, ChannelHandler::OUTPUT);
	m_AudioCH->Register ("MaxTime", &m_GUIArgs.MaxTime, ChannelHandler::OUTPUT);
	m_AudioCH->Register ("Playing", &m_GUIArgs.PlayOut, ChannelHandler::OUTPUT);
}

StreamPlugin::~StreamPlugin()
{
}

PluginInfo &StreamPlugin::Initialise(const HostInfo *Host)
{
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	return Info;
}

SpiralGUIType *StreamPlugin::CreateGUI() {
	return new StreamPluginGUI(m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void StreamPlugin::Execute() {
     if (m_File.IsOpen()) {
        for (int n=0; n<m_HostInfo->BUFSIZE; n++) {
            bool FinTrig = false;
            float CVPitch = GetInput(0, n)*10.0f;
            if (GetInput (1, n) > 0) m_Mode = PLAYM;
            if (GetInput (2, n) > 0) {
               m_Mode = STOPM;
               m_Pos = 0;
               m_GlobalPos = 0;
               m_StreamPos = 0;
            }

            if (m_Pos<0) {	
               m_Pos = m_SampleSize - 1;
               m_StreamPos -= m_SampleSize;
               FinTrig = m_StreamPos < 0;
               if (FinTrig) {
                  m_StreamPos = m_File.GetSize() - m_SampleSize;
                  m_GlobalPos = m_StreamPos;
               }
               m_File.SeekToChunk (m_StreamPos);

               if ((m_File.GetSize() - m_StreamPos) < 256)
		 m_SampleSize = m_File.GetSize() - m_StreamPos;
 	       else
		 m_SampleSize = 256;	

               m_File.LoadChunk (m_SampleSize, m_SampleL, m_SampleR);
            }
            else if (m_Pos >= m_SampleSize) {
               m_Pos = 0;
               m_StreamPos += m_SampleSize;
               FinTrig = m_StreamPos >= m_File.GetSize();
               if (FinTrig) {
                  m_StreamPos = 0;
                  m_GlobalPos = 0;
               }
               m_File.SeekToChunk (m_StreamPos);

               if ((m_File.GetSize() - m_StreamPos) < 256)
		 m_SampleSize = m_File.GetSize() - m_StreamPos;
 	       else
		 m_SampleSize = 256;	

               m_File.LoadChunk (m_SampleSize, m_SampleL, m_SampleR);
            }

            if (FinTrig) SetOutput (2, n, 1);
            else SetOutput (2, n, 0);
            if (m_Mode==PLAYM) {
              SetOutput (0, n, m_SampleL[m_Pos] * m_GUIArgs.Volume);
              SetOutput (1, n, m_SampleR[m_Pos] * m_GUIArgs.Volume);
              m_Pos += m_GUIArgs.PitchMod + CVPitch;
              m_GlobalPos += m_GUIArgs.PitchMod + CVPitch;
            }
            else {
              SetOutput (0, n, 0);
              SetOutput (1, n, 0);
            }
        }
        m_GUIArgs.TimeOut = m_GlobalPos / (float)m_SampleRate;
        m_GUIArgs.PlayOut = m_Mode==PLAYM;
     }
}

void StreamPlugin::ExecuteCommands() {
     if (m_AudioCH->IsCommandWaiting()) {
        switch (m_AudioCH->GetCommand()) {
	  case SET_TIME:
            SetTime();
            break;
          case LOAD:
            OpenStream();
            break;
	  case RESTART:
            m_StreamPos = 0;
            m_GlobalPos = 0;
            break;
	  case STOP:
            m_Mode = STOPM;
            break;
          case PLAY:
            m_Mode = PLAYM;
            break;
	}
     }
}

void StreamPlugin::SetTime (void) {
     m_GlobalPos = m_SampleRate * m_GUIArgs.Time;
     m_StreamPos = (int)(m_SampleRate * m_GUIArgs.Time);
     m_Pos = m_SampleSize;
}

void StreamPlugin::OpenStream (void) {
     m_StreamPos = 0;
     m_GlobalPos = 0;

     if (m_File.IsOpen ()) m_File.Close ();
     m_File.Open (m_GUIArgs.FileName, WavFile::READ);

     if (m_File.GetSize() < 256)
       m_SampleSize = m_File.GetSize();
     else
       m_SampleSize = 256;	

     m_SampleL.Allocate (m_SampleSize);
     m_SampleR.Allocate (m_SampleSize);
     m_Pitch = m_SampleRate / (float)m_HostInfo->SAMPLERATE;
     if (m_File.IsStereo ()) {
        m_Pitch *= 2;
        m_GUIArgs.MaxTime = GetLength();
     }
     else m_GUIArgs.MaxTime = GetLength() / 2;
}

float StreamPlugin::GetLength (void) {
      if (m_File.IsStereo()) return m_File.GetSize() / (float)m_File.GetSamplerate ();
                        else return m_File.GetSize() / (float)m_File.GetSamplerate () * 2;
}

void StreamPlugin::StreamOut (ostream &s) {
     s << m_Version << " "
       << m_GUIArgs.Volume << " "
       << m_GUIArgs.PitchMod << " "
       << strlen (m_GUIArgs.FileName) << " "
       << m_GUIArgs.FileName << " "
       // is it really necessary to save this lot??
       << m_Pos << " "
       << m_StreamPos << " "
       << m_GlobalPos << " "
       << m_Pitch << " " << endl;
}

void StreamPlugin::StreamIn(istream &s) {
     int version;
     s >> version;

     s >> m_GUIArgs.Volume >> m_GUIArgs.PitchMod;
     int size;
     s >> size;
     if (size > 255) size = 255;
     s.ignore (1);
     s.get (m_GUIArgs.FileName, size+1);
     if (m_GUIArgs.FileName != "None") OpenStream ();
       // is it really necessary to load this lot??
     s >> m_Pos;
     s >> m_StreamPos;
     s >> m_GlobalPos;
     s >> m_Pitch;
}
