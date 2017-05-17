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
#include <math.h>
#include "XFadePlugin.h"
#include "XFadePluginGUI.h"
#include <FL/Fl_Button.H>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new XFadePlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x001b;
}

string SpiralPlugin_GetGroupName()
{
	return "Amps/Mixers";
}
}

///////////////////////////////////////////////////////

XFadePlugin::XFadePlugin() :
m_Mix(0.5f)
{
	m_PluginInfo.Name="XFade";
	m_PluginInfo.Width=210;
	m_PluginInfo.Height=45;
	m_PluginInfo.NumInputs=5;
	m_PluginInfo.NumOutputs=2;
	m_PluginInfo.PortTips.push_back("XFade CV");
	m_PluginInfo.PortTips.push_back("A Left");
	m_PluginInfo.PortTips.push_back("A Right");
	m_PluginInfo.PortTips.push_back("B Left");
	m_PluginInfo.PortTips.push_back("B Right");
	m_PluginInfo.PortTips.push_back("Left");
	m_PluginInfo.PortTips.push_back("Right");

	m_AudioCH->Register("Mix",&m_Mix);
}

XFadePlugin::~XFadePlugin()
{
}

PluginInfo &XFadePlugin::Initialise(const HostInfo *Host)
{
	return SpiralPlugin::Initialise(Host);;
}

SpiralGUIType *XFadePlugin::CreateGUI()
{
	return new XFadePluginGUI(m_PluginInfo.Width,
							 m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void XFadePlugin::Execute()
{
 	float x;
	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		if (InputExists(0)) x=(1+GetInput(0,n))/2.0f;		
		else x=m_Mix;
		
		SetOutput(0,n,GetInput(1,n)*x + GetInput(3,n)*(1-x));	
		SetOutput(1,n,GetInput(2,n)*x + GetInput(4,n)*(1-x));	 
	}		
}
	
void XFadePlugin::Randomise()
{
}
	
void XFadePlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<m_Mix<<" "<<endl;
}

void XFadePlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	s>>m_Mix;
}
