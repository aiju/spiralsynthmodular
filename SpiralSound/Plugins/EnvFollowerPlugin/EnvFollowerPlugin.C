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
#include "EnvFollowerPlugin.h"
#include "EnvFollowerPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new EnvFollowerPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0013;
}

string SpiralPlugin_GetGroupName()
{
	return "Control";
}
}

///////////////////////////////////////////////////////

EnvFollowerPlugin::EnvFollowerPlugin() :
m_Attack(0.5),
m_Decay(0.5),
m_Value(0)
{
	m_PluginInfo.Name="EnvFollower";
	m_PluginInfo.Width=120;
	m_PluginInfo.Height=80;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");
	m_PluginInfo.PortTips.push_back("Output");

	m_AudioCH->Register("Attack",&m_Attack);
	m_AudioCH->Register("Decay",&m_Decay);
}

EnvFollowerPlugin::~EnvFollowerPlugin()
{
}

PluginInfo &EnvFollowerPlugin::Initialise(const HostInfo *Host)
{
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *EnvFollowerPlugin::CreateGUI()
{
	return new EnvFollowerPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void EnvFollowerPlugin::Execute()
{
	float Value;
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		Value = fabs(GetInput(0,n));
		
		if (Value>m_Value) m_Value+=(m_Attack*100);
		else m_Value-=(m_Decay*100);
		
		SetOutput(0,n,m_Value);
	}
}

void EnvFollowerPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<" ";
	s<<m_Attack<<" "<<m_Decay;
}

void EnvFollowerPlugin::StreamIn(istream &s)
{
	s>>m_Version;
	s>>m_Attack>>m_Decay;
}
