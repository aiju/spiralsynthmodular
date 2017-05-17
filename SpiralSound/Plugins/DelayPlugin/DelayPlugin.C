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
#include "DelayPlugin.h"
#include "DelayPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

static const float MAX_DELAY=1.0f;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new DelayPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x000f;
}

string SpiralPlugin_GetGroupName()
{
	return "Delay/Sampling";
}
}

///////////////////////////////////////////////////////

DelayPlugin::DelayPlugin() :
m_Delay(0.75),
m_Mix(0.4),
m_ReadHeadPos(0),
m_WriteHeadPos(0)
{
	m_PluginInfo.Name="Delay";
	m_PluginInfo.Width=120;
	m_PluginInfo.Height=110;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");
	m_PluginInfo.PortTips.push_back("Delay CV");
	m_PluginInfo.PortTips.push_back("ReadHead CV");
	m_PluginInfo.PortTips.push_back("Output");
	m_AudioCH->Register("Delay",&m_Delay);
	m_AudioCH->Register("Mix",&m_Mix);
}

DelayPlugin::~DelayPlugin()
{
}

PluginInfo &DelayPlugin::Initialise(const HostInfo *Host)
{
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	m_Buffer.Allocate((int)(m_HostInfo->SAMPLERATE*MAX_DELAY));
	return Info;
}

SpiralGUIType *DelayPlugin::CreateGUI()
{
	return new DelayPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void DelayPlugin::Execute () {
  int Delay;
  float max_pos = MAX_DELAY * m_HostInfo->SAMPLERATE;
  for (int n=0; n<m_HostInfo->BUFSIZE; n++) {
    Delay = (int)((m_Delay + GetInput(1,n) * 0.5f) * (float)m_HostInfo->SAMPLERATE);
    if (Delay >= max_pos) Delay = (int)(max_pos) - 1;
    if (Delay < 0) Delay=0;
    /* This used to give an error message - but we could get a segfault
    if (m_ReadHeadPos<0 || m_ReadHeadPos>=MAX_DELAY*m_HostInfo->SAMPLERATE)
      cerr<<"read agh! "<<m_ReadHeadPos<<endl;
    if (m_WriteHeadPos<0 || m_WriteHeadPos>=MAX_DELAY*m_HostInfo->SAMPLERATE)
      cerr<<"write agh! "<<m_WriteHeadPos<<endl;
    */
    if (m_ReadHeadPos >= max_pos) m_ReadHeadPos = max_pos - 1;
    if (m_ReadHeadPos < 0) m_ReadHeadPos = 0;
    if (m_WriteHeadPos >= max_pos) m_WriteHeadPos = max_pos - 1;
    if (m_WriteHeadPos < 0) m_WriteHeadPos = 0;
    // Read from the buffer
    SetOutput (0, n, m_Buffer[m_ReadHeadPos] + m_Mix * GetInput (0, n));
    // Write to the buffer
    m_Buffer.Set ((int)m_WriteHeadPos, GetInput (0, n));
    m_WriteHeadPos++;
    m_ReadHeadPos = m_WriteHeadPos + GetInput (2, n) * Delay;
    if (m_ReadHeadPos < 0) m_ReadHeadPos = Delay + m_ReadHeadPos;
    if (m_WriteHeadPos < 0) m_WriteHeadPos = Delay + m_WriteHeadPos;
    if (Delay > 0) {
      if (m_ReadHeadPos >= Delay) m_ReadHeadPos = m_ReadHeadPos - Delay;
      if (m_WriteHeadPos >= Delay) m_WriteHeadPos = m_WriteHeadPos - Delay;
    }
    else {
      m_ReadHeadPos = 0;
      m_WriteHeadPos = 0;
    }
  }
}

void DelayPlugin::Randomise()
{
}

void DelayPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<m_Delay<<" "<<m_Mix<<" ";
}

void DelayPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	s>>m_Delay>>m_Mix;
}
