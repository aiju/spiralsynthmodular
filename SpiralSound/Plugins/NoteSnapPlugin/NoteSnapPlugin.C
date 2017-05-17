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
#include "NoteSnapPlugin.h"
#include "NoteSnapPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new NoteSnapPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0018;
}

string SpiralPlugin_GetGroupName()
{
	return "Control";
}
}

///////////////////////////////////////////////////////

NoteSnapPlugin::NoteSnapPlugin() :
m_Out(0)
{
	m_PluginInfo.Name="Note Snap";
	m_PluginInfo.Width=80;
	m_PluginInfo.Height=80;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=2;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Output");
	m_PluginInfo.PortTips.push_back("Changed Trigger");
	
	for (int n=0; n<12; n++)
	{
		m_Filter[n]=true;
	}
	
	m_AudioCH->Register("Note",&m_Note);
}

NoteSnapPlugin::~NoteSnapPlugin()
{
}

PluginInfo &NoteSnapPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *NoteSnapPlugin::CreateGUI()
{
	return new NoteSnapPluginGUI(m_PluginInfo.Width,
						     m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void NoteSnapPlugin::Execute()
{	
	float Freq=0;
	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		Freq=GetInputPitch(0,n);
		
		SetOutput(1,n,0);	
		if (Freq!=m_LastFreq) // if it's changed
		{
			for (int i=0; i<131; i++) // for every note
			{			
				if (m_Filter[(i+1)%12] && Freq>=NoteTable[i] && Freq<NoteTable[i+1])
				{		
					m_Out=NoteTable[i];
					if (i!=m_LastNote) 
					{
						SetOutput(1,n,1);
						m_LastNote=i;
					}
				}
			}
		}
		
		m_LastFreq=Freq;
		SetOutputPitch(0,n,m_Out);		
	}
}

void NoteSnapPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
			case NOTE_ON  : m_Filter[m_Note]=true; break;
			case NOTE_OFF : m_Filter[m_Note]=false; break;
		}
	}
}
	
void NoteSnapPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<endl;
	for (int n=0; n<12; n++)
	{
		s<<m_Filter[n]<<" ";
	}

}

void NoteSnapPlugin::StreamIn(istream &s) 
{
// Pre-version 1 - check for blank line
	if (s.peek() == 10) {
		for (int n=0; n<12; n++)
		{
		// Use default (no notes filtered)
			m_Filter[n] = 1;
		}
	} else {
		int version;
		s>>version;
		for (int n=0; n<12; n++)
		{
			s>>m_Filter[n];
		}
	}
}
