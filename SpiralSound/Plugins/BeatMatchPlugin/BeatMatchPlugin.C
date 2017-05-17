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
#include "BeatMatchPlugin.h"
#include "BeatMatchPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new BeatMatchPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 48;
}

string SpiralPlugin_GetGroupName()
{
	return "Maths/Logic";
}
}

///////////////////////////////////////////////////////

BeatMatchPlugin::BeatMatchPlugin() :
m_Triggered(false),
m_EstimatedDuration(100),
m_BeatTime(100),
m_NextBeat(100),
m_OutputLevel(1.0f),
m_Sensitivity(0.5f)
{
	m_PluginInfo.Name="BeatMatch";
	m_PluginInfo.Width=80;
	m_PluginInfo.Height=80;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Output");
	
	m_AudioCH->Register("Sensitivity",&m_Sensitivity);
}

BeatMatchPlugin::~BeatMatchPlugin()
{
}

PluginInfo &BeatMatchPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *BeatMatchPlugin::CreateGUI()
{
	return new BeatMatchPluginGUI(m_PluginInfo.Width,
						     m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void BeatMatchPlugin::Execute()
{	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		bool Triggered=false;
		if (GetInput(0,n)>0)
		{
			if(!m_Triggered)
			{
				m_Triggered=true;
				Triggered=true;
			}
		}
		else
		{
			if (m_Triggered)
			{
				m_Triggered=false;
				Triggered=true;
			}
		}
		
		if (Triggered)
		{
			// adjust estimated duration
			// error = m_BeatTime
			m_EstimatedDuration-=(int)(m_BeatTime*m_Sensitivity); 
			m_BeatTime=m_EstimatedDuration;
			
			// push the sync closer
			int HalfBeat = m_EstimatedDuration/2;
			if (m_NextBeat<HalfBeat) m_NextBeat-=(int)(HalfBeat*m_Sensitivity);
			else m_NextBeat+=(int)(HalfBeat*m_Sensitivity);
		}
		
		if (m_NextBeat<=0)
		{
			m_NextBeat=m_EstimatedDuration;
			m_OutputLevel=-m_OutputLevel;
		}
		
		m_NextBeat--;
		m_BeatTime--;
		
		SetOutput(0,n,m_OutputLevel);
	}
}

void BeatMatchPlugin::ExecuteCommands()
{
}
	
void BeatMatchPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<endl;
	s<<m_Sensitivity<<" ";
}

void BeatMatchPlugin::StreamIn(istream &s) 
{
	int version;
	s>>version;
	s>>m_Sensitivity;
}
