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
#include "NoisePlugin.h"
#include "NoisePluginGUI.h"
#include <FL/Fl_Button.h>
#include <limits.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new NoisePlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 49;
}

string SpiralPlugin_GetGroupName()
{
	return "Oscillators";
}
}

///////////////////////////////////////////////////////

NoisePlugin::NoisePlugin() :
m_Type(WHITE)
{
	m_PluginInfo.Name="Noise";
	m_PluginInfo.Width=80;
	m_PluginInfo.Height=60;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Output");
	
	b0=b1=b2=b3=b4=b5=b6=0;
	
	m_AudioCH->Register("Type",(char*)&m_Type);	
}

NoisePlugin::~NoisePlugin()
{
}

PluginInfo &NoisePlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *NoisePlugin::CreateGUI()
{
	return new NoisePluginGUI(m_PluginInfo.Width,
										  m_PluginInfo.Height,
										  this,m_AudioCH,m_HostInfo);									  
}

void NoisePlugin::Execute()
{
	float White,Pink;
	if (m_Type==PINK)
	{
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{
			White=(1.0f-((rand()%INT_MAX)/(float)INT_MAX)*2.0)*0.2f;
		
			b0 = 0.99886f * b0 + White * 0.0555179f;
 		 	b1 = 0.99332f * b1 + White * 0.0750759f;
  			b2 = 0.96900f * b2 + White * 0.1538520f;
  			b3 = 0.86650f * b3 + White * 0.3104856f;
  			b4 = 0.55000f * b4 + White * 0.5329522f;
  			b5 = -0.7616f * b5 - White * 0.0168980f;
  			Pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + White * 0.5362f;
  			b6 = White * 0.115926f;
			
			SetOutput(0,n,Pink);
		}
	}
	else
	{
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{
			SetOutput(0,n,1.0f-((rand()%INT_MAX)/(float)INT_MAX)*2.0);
		}
	}
}

void NoisePlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<(char)m_Type;
}

void NoisePlugin::StreamIn(istream &s)
{
	int version;
	char t;
	s>>version>>t;
	m_Type=(Type)t;
}
