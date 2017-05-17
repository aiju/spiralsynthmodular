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
#include "TrigPlugin.h"
#include "TrigPluginGUI.h"
#include <FL/Fl_Button.H>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

using namespace std;

static const float PI=3.141592654;
static const float RAD=(PI/180)*360;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new TrigPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 53;
}

string SpiralPlugin_GetGroupName()
{
	return "Maths/Logic";
}
}

///////////////////////////////////////////////////////

TrigPlugin::TrigPlugin() :
m_Operator(SIN)
{
	m_Version=1;

	m_PluginInfo.Name="Trig";
	m_PluginInfo.Width=80;
	m_PluginInfo.Height=80;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Output");
		
	m_AudioCH->Register("Operator",(int*)&m_Operator);
}

TrigPlugin::~TrigPlugin()
{
}

PluginInfo &TrigPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *TrigPlugin::CreateGUI()
{
	return new TrigPluginGUI(m_PluginInfo.Width,
						     m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void TrigPlugin::Execute()
{	
	float Freq=0, OldFreq=0;
	
	switch (m_Operator) 
	{
		case SIN : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				SetOutput(0,n,sin(GetInput(0,n)*RAD));
			} 
			break;
		case COS : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				SetOutput(0,n,cos(GetInput(0,n)*RAD));
			} 
			break;
		case TAN :
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				SetOutput(0,n,tan(GetInput(0,n)*RAD));
			}
			break;
                default: break;

	}
}

void TrigPlugin::ExecuteCommands()
{
}

void TrigPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<endl;
	s<<(int)m_Operator<<" ";
}

void TrigPlugin::StreamIn(istream &s)
{
	int version;
	s>>version;
	int t;
	s>>t;
	m_Operator=(OperatorType)t;
}
