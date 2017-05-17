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
#include "StereoMixerPlugin.h"
#include "StereoMixerPluginGUI.h"
#include <FL/Fl_Button.H>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new StereoMixerPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0008;
}

string SpiralPlugin_GetGroupName()
{
	return "Amps/Mixers";
}
}

///////////////////////////////////////////////////////

StereoMixerPlugin::StereoMixerPlugin()
{
	m_PluginInfo.Name="Stereo Mixer";
	m_PluginInfo.Width=190;
	m_PluginInfo.Height=175;
	m_PluginInfo.NumInputs=8;
	m_PluginInfo.NumOutputs=2;
	m_PluginInfo.PortTips.push_back("Input one");	
	m_PluginInfo.PortTips.push_back("Input two");	
	m_PluginInfo.PortTips.push_back("Input three");	
	m_PluginInfo.PortTips.push_back("Input four");	
	m_PluginInfo.PortTips.push_back("Pan CV one");	
	m_PluginInfo.PortTips.push_back("Pan CV two");	
	m_PluginInfo.PortTips.push_back("Pan CV three");	
	m_PluginInfo.PortTips.push_back("Pan CV four");	
	m_PluginInfo.PortTips.push_back("Output left");
	m_PluginInfo.PortTips.push_back("Output right");
	
	for (int n=0; n<NUM_CHANNELS; n++)
	{
		m_ChannelVal[n]=1.0f;
		m_Pan[n]=0.5f;		
	}
	
	m_AudioCH->Register("Num",&m_GUIArgs.Num);
	m_AudioCH->Register("Value",&m_GUIArgs.Value);
}

StereoMixerPlugin::~StereoMixerPlugin()
{
}

PluginInfo &StereoMixerPlugin::Initialise(const HostInfo *Host)
{		
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *StereoMixerPlugin::CreateGUI()
{
	return new StereoMixerPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void StereoMixerPlugin::Execute()
{
	float Pan[4];
	
	// Mix the inputs
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		Pan[0]=m_Pan[0];
		Pan[1]=m_Pan[1];
		Pan[2]=m_Pan[2];
		Pan[3]=m_Pan[3];

		if (InputExists(0)) Pan[0]+=GetInput(4,n)*0.5;
		if (InputExists(1)) Pan[1]+=GetInput(5,n)*0.5;
		if (InputExists(2)) Pan[2]+=GetInput(6,n)*0.5;
		if (InputExists(3)) Pan[3]+=GetInput(7,n)*0.5;
	
		SetOutput(0,n,(GetInput(0,n)*m_ChannelVal[0])*Pan[0]+
					  (GetInput(1,n)*m_ChannelVal[1])*Pan[1]+
					  (GetInput(2,n)*m_ChannelVal[2])*Pan[2]+
					  (GetInput(3,n)*m_ChannelVal[3])*Pan[3]);
							 
		SetOutput(1,n,(GetInput(0,n)*m_ChannelVal[0])*(1-Pan[0])+
					  (GetInput(1,n)*m_ChannelVal[1])*(1-Pan[1])+
					  (GetInput(2,n)*m_ChannelVal[2])*(1-Pan[2])+
					  (GetInput(3,n)*m_ChannelVal[3])*(1-Pan[3]));
	}
}

void StereoMixerPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
			case (SETCH) : SetChannel(m_GUIArgs.Num,m_GUIArgs.Value); break;
			case (SETPAN) : SetPan(m_GUIArgs.Num,m_GUIArgs.Value); break;		
		}
	}
}

void StereoMixerPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	for (int n=0; n<NUM_CHANNELS; n++)
	{
		s<<m_ChannelVal[n]<<" "<<m_Pan[n]<<" ";
	}
}

void StereoMixerPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	for (int n=0; n<NUM_CHANNELS; n++)
	{
		s>>m_ChannelVal[n]>>m_Pan[n];
	}
}


