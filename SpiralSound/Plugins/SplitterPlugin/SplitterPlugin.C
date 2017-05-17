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
#include "SplitterPlugin.h"
#include "SplitterPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new SplitterPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0006;
}

string SpiralPlugin_GetGroupName()
{
	return "Control";
}
}

///////////////////////////////////////////////////////

SplitterPlugin::SplitterPlugin()
{
	m_PluginInfo.Name="Splitter";
	m_PluginInfo.Width=220;
	m_PluginInfo.Height=125;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=4;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Out one");
	m_PluginInfo.PortTips.push_back("Out two");
	m_PluginInfo.PortTips.push_back("Out three");
	m_PluginInfo.PortTips.push_back("Out four");
}

SplitterPlugin::~SplitterPlugin()
{
}

PluginInfo &SplitterPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *SplitterPlugin::CreateGUI()
{
	return NULL;
}

void SplitterPlugin::Execute()
{
	// Just copy the data through.
	GetOutputBuf(0)->Zero();
	GetOutputBuf(3)->Zero();
	GetOutputBuf(2)->Zero();
	GetOutputBuf(1)->Zero();

	if (InputExists(0))  
	{
		GetOutputBuf(0)->Mix(*GetInput(0),0);
		GetOutputBuf(1)->Mix(*GetInput(0),0);
		GetOutputBuf(2)->Mix(*GetInput(0),0);
		GetOutputBuf(3)->Mix(*GetInput(0),0);
	}
}

