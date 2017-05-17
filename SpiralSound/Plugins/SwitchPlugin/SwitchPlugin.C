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
#include "SwitchPlugin.h"
#include "SwitchPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new SwitchPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 47;
}

string SpiralPlugin_GetGroupName()
{
	return "Maths/Logic";
}
}

///////////////////////////////////////////////////////

SwitchPlugin::SwitchPlugin() :
m_Mix(false)
{
	m_PluginInfo.Name="Switch";
	m_PluginInfo.Width=80;
	m_PluginInfo.Height=50;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input 1");
	m_PluginInfo.PortTips.push_back("Input 2");
	m_PluginInfo.PortTips.push_back("CV");
	m_PluginInfo.PortTips.push_back("Output");

	m_AudioCH->Register("Mix",&m_Mix);
}

SwitchPlugin::~SwitchPlugin()
{
}

PluginInfo &SwitchPlugin::Initialise(const HostInfo *Host)
{
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *SwitchPlugin::CreateGUI()
{
	return new SwitchPluginGUI(m_PluginInfo.Width,
						     m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void SwitchPlugin::Execute()
{	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		if (!m_Mix)
		{
			if (GetInput(2,n)<=0)
			{
				SetOutput(0,n,GetInput(0,n));
			}
			else
			{
				SetOutput(0,n,GetInput(1,n));
			}
		}
		else
		{
			SetOutput(0,n,Linear(-1.0f,1.0f,GetInput(2,n),GetInput(0,n),GetInput(1,n)));
		}
	}
}

void SwitchPlugin::ExecuteCommands()
{
}
	
void SwitchPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<endl;
	s<<m_Mix<<" ";
}

void SwitchPlugin::StreamIn(istream &s) 
{
	int version;
	s>>version;
	s>>m_Mix;
}
