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
#include "SampleHoldPlugin.h"
#include "SampleHoldPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new SampleHoldPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0033;
}

string SpiralPlugin_GetGroupName()
{
	return "Control";
}
}

///////////////////////////////////////////////////////

SampleHoldPlugin::SampleHoldPlugin()
{
	m_PluginInfo.Name="SampleHold";
	m_PluginInfo.Width=220;
	m_PluginInfo.Height=125;
	m_PluginInfo.NumInputs=2;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");
	m_PluginInfo.PortTips.push_back("Clock");	
	m_PluginInfo.PortTips.push_back("Out");
}

SampleHoldPlugin::~SampleHoldPlugin()
{
}

PluginInfo &SampleHoldPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	m_val = 0;
	m_flag = 0;
	return Info;
}

SpiralGUIType *SampleHoldPlugin::CreateGUI()
{
	return NULL;
}

void SampleHoldPlugin::Execute()
{
	// Just copy the data through.
	GetOutputBuf(0)->Zero();
	if (InputExists(0) && InputExists(1))
	{
		for (int i=0;i<m_HostInfo->BUFSIZE;i++)
		{
			float v = GetInput(0,i); 
			float c = GetInput(1,i); 
			if (c > 0.666)
			{
				if (!m_flag)
				{
					m_val = v;
					SetOutput(0,i,v);
					m_flag = 1;
				}
				else
					SetOutput(0,i,m_val);
			}
			else
				SetOutput(0,i,m_val);
			if (c < 0.333)
			{
				m_flag = 0;
			}
		}
	}
}
