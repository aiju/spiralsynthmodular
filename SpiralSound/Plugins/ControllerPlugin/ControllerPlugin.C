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

#include <stdio.h>
#include "ControllerPlugin.h"
#include "ControllerPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new ControllerPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0003;
}

string SpiralPlugin_GetGroupName()
{
	return "Control";
}
}

///////////////////////////////////////////////////////

ControllerPlugin::ControllerPlugin() :
m_Num(4)
{
	m_Version=5;

	m_PluginInfo.Name="CV";
	m_PluginInfo.Width=240;
	m_PluginInfo.Height=224;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=4;
	m_PluginInfo.PortTips.push_back("CV 1");
	m_PluginInfo.PortTips.push_back("CV 2");
	m_PluginInfo.PortTips.push_back("CV 3");
	m_PluginInfo.PortTips.push_back("CV 4");

	for (int n=0; n<MAX_CHANNELS; n++)
	{
		m_ChannelVal[n]=0.0f;
		m_MinVal[n]=-1.0f;
		m_MaxVal[n]=1.0f;
		m_Names[n]="Name";
	}
	
	m_AudioCH->Register("Number",&m_GUIArgs.Number);
	m_AudioCH->Register("Value",&m_GUIArgs.Value);
	m_AudioCH->Register("Min",&m_GUIArgs.Min);
	m_AudioCH->Register("Max",&m_GUIArgs.Max);
	m_AudioCH->RegisterData("Name",ChannelHandler::INPUT,m_GUIArgs.Name,256);
}

ControllerPlugin::~ControllerPlugin()
{
}

PluginInfo &ControllerPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *ControllerPlugin::CreateGUI()
{
	return new ControllerPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void ControllerPlugin::Execute()
{
	for(int c=0; c<m_Num; c++)
	{
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{			
			SetOutput(c,n,m_ChannelVal[c]);
		}
	}
}

void ControllerPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
			case (SETNUM)     :
				SetNum(m_GUIArgs.Number);
			break;
			case (SETALL)     :
				SetAll(m_GUIArgs.Number, m_GUIArgs.Name, m_GUIArgs.Max, m_GUIArgs.Value, m_GUIArgs.Min);
			break;
			case (SETNAME)    :
				SetName(m_GUIArgs.Number, m_GUIArgs.Name);
			break;
			case (SETMAX)     :
				SetMax(m_GUIArgs.Number, m_GUIArgs.Max);
			break;
			case (SETCHANNEL) :
				SetChannel(m_GUIArgs.Number, m_GUIArgs.Value);
			break;
			case (SETMIN)     :
				SetMin(m_GUIArgs.Number, m_GUIArgs.Min);
			break;
        }
	}
}

void ControllerPlugin::SetNum(int n)
{
	// once to clear the connections with the current info
	UpdatePluginInfoWithHost();

	if (m_Num<n)
	{
		char t[256];
		sprintf(t,"CV %d",n);
		m_PluginInfo.PortTips.push_back(t);
		AddOutput();
		m_PluginInfo.NumOutputs++;
	}
	else
	{
		vector<string>::iterator i=m_PluginInfo.PortTips.end();
		m_PluginInfo.PortTips.erase(i--);
		RemoveOutput();
		m_PluginInfo.NumOutputs--;
	}
	
	m_Num=n;
	m_PluginInfo.NumOutputs=n;

	// do the actual update
	UpdatePluginInfoWithHost();	
}

// use with caution
void ControllerPlugin::Clear()
{
	m_PluginInfo.PortTips.clear();
	RemoveAllOutputs();
	m_PluginInfo.NumOutputs=0;
}

void ControllerPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";

	switch (m_Version)
	{
		case 5:
		{
			s<<m_Num<<endl;
			for (int n=0; n<m_Num; n++)
			{
				s<<m_Names[n].size()<<" ";
				s<<m_Names[n]<<" ";
				s<<m_MinVal[n]<<" ";
				s<<m_MaxVal[n]<<" ";
				s<<m_ChannelVal[n]<<endl;
			}
		} break;
		case 4:
		{
			s<<m_Num<<endl;
			for (int n=0; n<m_Num; n++)
			{
				s<<m_Names[n]<<" ";
				s<<m_MinVal[n]<<" ";
				s<<m_MaxVal[n]<<" ";
				s<<m_ChannelVal[n]<<endl;
			}
		} break;
		case 3:
		{
			s<<m_Num<<" ";
			for (int n=0; n<m_Num; n++)
			{
				s<<m_ChannelVal[n]<<" ";
			}
			s<<1<<endl;
			s<<m_Num<<" ";
			for (int n=0; n<m_Num; n++)
			{
				s<<m_Names[n].size()<<" ";
				s<<m_Names[n]<<" ";
				s<<m_MinVal[n]<<" ";
				s<<m_MaxVal[n]<<" ";
				s<<m_ChannelVal[n]<<endl;
			}

		} break;
		default :
			cerr<<"ControllerPlugin - I dont support this streaming version any more"<<endl;
		break;
	}
}

void ControllerPlugin::StreamIn(istream &s)
{
	int version;
	s>>version;

	switch (version)
	{
		case 5:
		{
			Clear();

			s>>m_Num;
			string name;
			for (int n=0; n<m_Num; n++)
			{
				char Buf[4096];
				int size,dummy;
				s>>size;
				s.ignore(1);
				if (size > 0) {
					s.get(Buf,size+1);
					m_Names[n]=Buf;
				} else {
					m_Names[n] = "";
				}
				s>>m_MinVal[n];
				s>>m_MaxVal[n];
				s>>m_ChannelVal[n];
			}

		// add the channels one by one
			for (int n=0; n<m_Num; n++)
			{
				char t[256];
				sprintf(t,"CV %d",n);
				m_PluginInfo.PortTips.push_back(t);
				AddOutput();
			}

			m_PluginInfo.NumOutputs=m_Num;
			UpdatePluginInfoWithHost();
		} break;
		case 4:
		{
			Clear();

			s>>m_Num;
			string name;
			for (int n=0; n<m_Num; n++)
			{
				s>>m_Names[n];
				s>>m_MinVal[n];
				s>>m_MaxVal[n];
				s>>m_ChannelVal[n];
			}

		// add the channels one by one
			for (int n=0; n<m_Num; n++)
			{
				char t[256];
				sprintf(t,"CV %d",n);
				m_PluginInfo.PortTips.push_back(t);
				AddOutput();
			}

			m_PluginInfo.NumOutputs=m_Num;
			UpdatePluginInfoWithHost();
		} break;
		case 3:
		{
			Clear();

			s>>m_Num;

			for (int n=0; n<m_Num; n++)
			{
				s>>m_ChannelVal[n];
			}

			char Buf[4096];
			int size,dummy;
			s>>dummy;
			s>>m_Num;
			for (int n=0; n<m_Num; n++)
			{
				s>>size;
				s.ignore(1);
				s.get(Buf,size+1);
				m_Names[n]=Buf;
				s>>m_MinVal[n];
				s>>m_MaxVal[n];
				s>>m_ChannelVal[n];
			}

			// add the channels one by one
			for (int n=0; n<m_Num; n++)
			{
				char t[256];
				sprintf(t,"CV %d",n);
				m_PluginInfo.PortTips.push_back(t);
				AddOutput();
			}

			m_PluginInfo.NumOutputs=m_Num;
			UpdatePluginInfoWithHost();
		} break;

		default :
			cerr<<"ControllerPlugin - I dont support this streaming version any more"<<endl;
		break;
	}
}
