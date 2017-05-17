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

// is this still relevant
// for lrintf()
#define	_ISOC9X_SOURCE	1
#define _ISOC99_SOURCE	1

//#include <math.h>
//#include <sys/types.h>
//#include <sys/ioctl.h>
//#include <unistd.h>
//#include <limits.h>
#include "DiskWriterPlugin.h"
#include "DiskWriterPluginGUI.h"
#include <FL/Fl_File_Chooser.H>
#include "SpiralIcon.xpm"

using namespace std;

// what's this for - should it still be here?
#define CHECK_AND_REPORT_ERROR	if (result<0)         \
{                     \
	perror("Sound device did not accept settings"); \
	m_OutputOk=false; \
	return false;     \
}

extern "C"
{

SpiralPlugin* SpiralPlugin_CreateInstance() { return new DiskWriterPlugin; }

char** SpiralPlugin_GetIcon() { return SpiralIcon_xpm; }

int SpiralPlugin_GetID() { return 41; }

string SpiralPlugin_GetGroupName() { return "InputOutput"; }

}

///////////////////////////////////////////////////////

DiskWriterPlugin::DiskWriterPlugin()
{
	m_PluginInfo.Name="DiskWriter";
	m_PluginInfo.Width=160;
	m_PluginInfo.Height=115;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=0;
	m_PluginInfo.PortTips.push_back("Left Out");
	m_PluginInfo.PortTips.push_back("Right Out");
	m_PluginInfo.PortTips.push_back("Record Controller");

        m_GUIArgs.BitsPerSample = 16;
        m_GUIArgs.Stereo = true;
	m_GUIArgs.Recording = false;
	m_GUIArgs.TimeRecorded = 0;

	m_Version = 2;

	m_AudioCH->RegisterData("Filename",ChannelHandler::INPUT,m_GUIArgs.Name,256);
	m_AudioCH->Register("BitsPerSample",&m_GUIArgs.BitsPerSample,ChannelHandler::INPUT);
	m_AudioCH->Register("Stereo",&m_GUIArgs.Stereo,ChannelHandler::INPUT);
	m_AudioCH->Register("TimeRecorded",&m_GUIArgs.TimeRecorded,ChannelHandler::OUTPUT);
	m_AudioCH->Register("Recording",&m_GUIArgs.Recording,ChannelHandler::OUTPUT);
}

DiskWriterPlugin::~DiskWriterPlugin()
{
}

PluginInfo &DiskWriterPlugin::Initialise(const HostInfo *Host)
{
	PluginInfo& Info= SpiralPlugin::Initialise(Host);
	return Info;
}

SpiralGUIType *DiskWriterPlugin::CreateGUI()
{
	return new DiskWriterPluginGUI(m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void DiskWriterPlugin::Execute()
{
	if(m_GUIArgs.Recording && m_Wav.IsOpen())
	{
		int on=0;
		float LeftBuffer[GetHostInfo()->BUFSIZE], RightBuffer[GetHostInfo()->BUFSIZE];

		for (int n=0; n<GetHostInfo()->BUFSIZE; n++)
		{
			// stereo channels - interleave
			LeftBuffer[n]=GetInput(0,n);
			RightBuffer[n]=GetInput(1,n);
		}

		m_Wav.Save(LeftBuffer, RightBuffer, GetHostInfo()->BUFSIZE);
		m_GUIArgs.TimeRecorded = m_Wav.GetSize()/m_Wav.GetSamplerate();
	}
}

void DiskWriterPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch(m_AudioCH->GetCommand())
		{
			case OPENWAV :
				if (m_Wav.GetSamplerate() != GetHostInfo()->SAMPLERATE) {
					m_Wav.SetSamplerate(GetHostInfo()->SAMPLERATE);
				}
				if (m_Wav.GetBitsPerSample() != m_GUIArgs.BitsPerSample) {
					m_Wav.SetBitsPerSample(m_GUIArgs.BitsPerSample);
				}
				m_Wav.Open(m_GUIArgs.Name,WavFile::WRITE, (m_GUIArgs.Stereo)?(WavFile::STEREO):(WavFile::MONO));
				m_GUIArgs.TimeRecorded = 0;
			break;
			case CLOSEWAV : m_Wav.Close(); break;
			case RECORD : m_GUIArgs.Recording=true; break;
			case STOP : m_GUIArgs.Recording=false;	break;
			default : break;
		}
	}
}

void DiskWriterPlugin::StreamOut (ostream &s)
{
	s << m_Version << " " << m_GUIArgs.BitsPerSample << " " << m_GUIArgs.Stereo << " ";
}

void DiskWriterPlugin::StreamIn (istream &s)
{
	char Test;
	int Version, BitsPerSample, Stereo;

	// originally DiskWriter had NO streaming code whatsover
	// so to test if this is an old patch we must
	// read ahead and find out what the first char
	// of the next line is

	s.seekg (2, ios::cur );  //skip to next line
	Test = s.peek();         //peek first char
	s.seekg (-2, ios::cur ); //jump back to prior line

	// This test works because if the char
	// of the next line isn't a version number
	// it will only be 'D', ' ', #13, or '-'
	if ( (Test >= '0') && (Test <= '9') )
	{
		s >> Version;
	}
	else
	{
		//No Version, so use Version 1
		Version = 1;
	}

	switch (Version)
	{
		case 2:
		{
			s >> BitsPerSample >> Stereo;
			m_GUIArgs.BitsPerSample = BitsPerSample;
			m_GUIArgs.Stereo = Stereo;
		}
		break;

		case 1:
		{
			//use original fixed defaults
			m_GUIArgs.BitsPerSample = 16;
			m_GUIArgs.Stereo = true;
		}
		break;
	}
}
