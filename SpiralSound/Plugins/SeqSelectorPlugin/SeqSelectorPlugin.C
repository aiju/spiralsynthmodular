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
#include "SeqSelectorPlugin.h"
#include "SeqSelectorPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new SeqSelectorPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0015;
}

string SpiralPlugin_GetGroupName()
{
	return "Sequencing";
}
}

///////////////////////////////////////////////////////

SeqSelectorPlugin::SeqSelectorPlugin()
{
	m_PluginInfo.Name="SeqSelector";
	m_PluginInfo.Width=300;
	m_PluginInfo.Height=200;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=8;
	m_PluginInfo.PortTips.push_back("Trigger");	
	m_PluginInfo.PortTips.push_back("CV One");	
	m_PluginInfo.PortTips.push_back("CV Two");	
	m_PluginInfo.PortTips.push_back("CV Three");	
	m_PluginInfo.PortTips.push_back("CV Four");	
	m_PluginInfo.PortTips.push_back("CV Five");	
	m_PluginInfo.PortTips.push_back("CV Six");	
	m_PluginInfo.PortTips.push_back("CV Seven");	
	m_PluginInfo.PortTips.push_back("CV Eight");	
	
	m_Pos=0;
	m_Triggered=false;
	m_UseRange=false;	
	
	m_AudioCH->Register("Num",&m_GUIArgs.Num);
	m_AudioCH->Register("Line",&m_GUIArgs.Line);
	m_AudioCH->Register("Val",&m_GUIArgs.Val);
	m_AudioCH->Register("Pos",(int*)&m_Pos,ChannelHandler::OUTPUT);
}

SeqSelectorPlugin::~SeqSelectorPlugin()
{
}

PluginInfo &SeqSelectorPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *SeqSelectorPlugin::CreateGUI()
{
	return new SeqSelectorPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void SeqSelectorPlugin::Execute()
{
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		// Sends momentary spike of value when triggered, 
		// works on the note off, rather than the first
		// detection of a trigger value
		if (GetInput(0,n)>0.1)
		{
			m_Triggered=true;
		}
		else
		{			
			if (m_Triggered==true && m_Lines.size()>0)			
			{								
				m_Pos++;
				
				if (m_UseRange)
				{
					if (m_Pos>=m_End)
					{
						m_Pos=m_Begin;
					}
				}
				else
				{
					if (m_Pos>=m_Lines.size())
					{
						m_Pos=0;
					}
				}
								
				for(int i=0; i<8; i++)
				{			
					SetOutputPitch(i,n,NoteTable[(int)m_Lines[m_Pos].Value[i]]);
				}														
			}					
			else
			{
				for(int i=0; i<8; i++)
				{
					// zero frequency
					SetOutputPitch(i,n,-1);		
				}
			}
			
			m_Triggered=false;						
		}		
	}
}

void SeqSelectorPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
			case SET_BEGIN : m_Begin=m_GUIArgs.Line; break;
			case SET_END   : m_End=m_GUIArgs.Line; break;
			case RANGE     : m_UseRange=m_GUIArgs.Val; break;
			case ADD_LINE  : 
				{
					Line NewLine;
					if (m_Lines.size()) 
					{
						for (int n=0; n<NUM_VALUES; n++) NewLine.Value[n]=m_Lines[m_Lines.size()-1].Value[n];
					}
					else 
					{
						for (int n=0; n<NUM_VALUES; n++) NewLine.Value[n]=0;
					}
					m_Lines.push_back(NewLine);
				} break;
			case REM_LINE  : m_Lines.pop_back(); break;
			case SET_VAL   : m_Lines[m_GUIArgs.Line].Value[m_GUIArgs.Num]=m_GUIArgs.Val; break;
			default: break;
		}
	}
}

void SeqSelectorPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	s<<m_Lines.size()<<" ";
	
	if (m_Lines.size()>0)
	{
		for (vector<Line>::iterator i = m_Lines.begin();
			 i!=m_Lines.end(); i++)
		{
			for (int n=0; n<NUM_VALUES; n++)
			{
				s<<i->Value[n]<<" ";
			}
		}
	}
}

void SeqSelectorPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	
	int Num;
	s>>Num;
	
	for (int i=0; i<Num; i++)
	{
		Line NewLine;
		for (int n=0; n<NUM_VALUES; n++)
		{
			s>>NewLine.Value[n];
		}
		m_Lines.push_back(NewLine);
	}	
}
