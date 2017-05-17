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
#include "FlipflopPlugin.h"
#include "FlipflopPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new FlipflopPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 46;
}

string SpiralPlugin_GetGroupName()
{
	return "Maths/Logic";
}
}

///////////////////////////////////////////////////////

FlipflopPlugin::FlipflopPlugin() :
m_TriggerTime(0.01f),
m_Monostable(false),
m_Current(0),
m_Triggered(false),
m_CurrentLevel(1.0f),
m_TriggerSamples(0)
{
	m_Version = 2;
	m_PluginInfo.Name="Flipflop";
	m_PluginInfo.Width=80;
	m_PluginInfo.Height=100;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Output");

	m_AudioCH->Register("TriggerTime",&m_TriggerTime);
	m_AudioCH->Register("Monostable",&m_Monostable);
}

FlipflopPlugin::~FlipflopPlugin()
{
}

PluginInfo &FlipflopPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *FlipflopPlugin::CreateGUI()
{
	return new FlipflopPluginGUI(m_PluginInfo.Width,
						     m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void FlipflopPlugin::Execute()
{	
	bool Triggered;
	
	if (m_Monostable)
	{
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{
			if (GetInput(0,n)>0)
			{
				if(!m_Triggered)
				{
					m_Triggered=true;
					m_CurrentLevel=1.0f;
					m_TriggerSamples=(int)(m_TriggerTime*m_HostInfo->SAMPLERATE)+1;
				}
			}
			else
			{
				m_Triggered=false;
			}
			
			if (m_TriggerSamples<=0)
			{
				m_CurrentLevel=-1.0f;
			}
			
			if (m_TriggerSamples>0) m_TriggerSamples--;
			
			SetOutput(0,n,m_CurrentLevel);
		}	
	}
	else
	{
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{
			if (GetInput(0,n)>0)
			{
				if(!m_Triggered)
				{
					m_Triggered=true;
					m_CurrentLevel=-m_CurrentLevel;
				}
			}
			else
			{
				if (m_Triggered)
				{
					m_Triggered=false;
				}
			}
			
			SetOutput(0,n,m_CurrentLevel);
		}
	}
}

void FlipflopPlugin::ExecuteCommands()
{
}
	
void FlipflopPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<" ";
	s<<m_TriggerTime<<" "<<m_Monostable<<" "<<endl;
}

void FlipflopPlugin::StreamIn(istream &s) 
{
	int version;
	s>>version;
	if (version>1)
	{
		s>>m_TriggerTime>>m_Monostable;
	}
}
