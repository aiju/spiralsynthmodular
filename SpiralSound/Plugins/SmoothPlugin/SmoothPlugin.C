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
#include "SmoothPlugin.h"
#include "SmoothPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new SmoothPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0014;
}

string SpiralPlugin_GetGroupName()
{
	return "Control";
}
}

///////////////////////////////////////////////////////

SmoothPlugin::SmoothPlugin() :
m_Up(0.5),
m_Down(0.5),
m_Value(0)
{
	m_PluginInfo.Name="Smooth";
	m_PluginInfo.Width=120;
	m_PluginInfo.Height=80;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Output");
	
	m_AudioCH->Register("Up",&m_Up);
	m_AudioCH->Register("Down",&m_Down);
}

SmoothPlugin::~SmoothPlugin()
{
}

PluginInfo &SmoothPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *SmoothPlugin::CreateGUI()
{
	return new SmoothPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void SmoothPlugin::Execute()
{
	float Value;
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		Value = GetInput(0,n);
		
		if (Value>m_Value) m_Value+=m_Up*0.0001f;
		else m_Value-=m_Down*0.0001f;
		
		SetOutput(0,n,m_Value);
	}
}

void SmoothPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<" ";
	s<<m_Up<<" "<<m_Down;
}

void SmoothPlugin::StreamIn(istream &s)
{
	s>>m_Version;
	s>>m_Up>>m_Down;
}

 
