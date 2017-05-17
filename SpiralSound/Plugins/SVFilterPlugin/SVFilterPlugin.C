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
#include "SVFilterPlugin.h"
#include "SVFilterPluginGUI.h"
#include "SpiralIcon.xpm"

using namespace std;

static const double PI = 3.141592654;
static const int GRANULARITY = 10;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new SVFilterPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x000c;
}

string SpiralPlugin_GetGroupName()
{
	return "Filters/FX";
}
}

///////////////////////////////////////////////////////

SVFilterPlugin::SVFilterPlugin() :
Cutoff(0.0f),
Resonance(0.0f),
fs(44100.0f),
fc(1000.0f),
q(1.0f),
m_f(0.0f),
m_q(0.0f),
m_qnrm(0.0f),
m_h(0.0f),
m_b(0.0f), 
m_l(0.0f),
m_p(0.0f),
m_n(0.0f)
{
        m_Version = 2;
        m_PluginInfo.Name="SVF";
	m_PluginInfo.Width=120;
	m_PluginInfo.Height=110;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=5;
	m_PluginInfo.PortTips.push_back("Input");
	m_PluginInfo.PortTips.push_back("Cutoff CV");
	m_PluginInfo.PortTips.push_back("Emphasis CV");
	m_PluginInfo.PortTips.push_back("LowPass output");
	m_PluginInfo.PortTips.push_back("BandPass output");
	m_PluginInfo.PortTips.push_back("HighPass output");
	m_PluginInfo.PortTips.push_back("Notch output");
	m_PluginInfo.PortTips.push_back("Peaking output");
	m_AudioCH->Register("Cutoff",&Cutoff);
	m_AudioCH->Register("Resonance",&Resonance);
}

SVFilterPlugin::~SVFilterPlugin()
{
}

PluginInfo &SVFilterPlugin::Initialise(const HostInfo *Host)
{
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	fs = m_HostInfo->SAMPLERATE;
	return Info;
}

SpiralGUIType *SVFilterPlugin::CreateGUI()
{
	return new SVFilterPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void SVFilterPlugin::Execute()
{
	float in;

    for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		if (n%GRANULARITY==0)
		{
			fc = 4000.0f*(Cutoff+GetInput(1,n));
	 		q  = 1-Resonance+GetInput(2,n);
			m_f = 2.0f*sin(PI*fc/fs);
		}

		in = GetInput(0,n);

		if (in == 0)
			Clear();	 
		else 
		{				
			float scale=0.5f;

			m_l = m_l + m_f*m_b;
			m_h = scale*in - m_l - q*m_b;
			m_b = m_b + m_f*m_h;
			m_n = m_l + m_h;
			m_p = m_l - m_h;
		}
		
		SetOutput(0,n,m_l);
		SetOutput(1,n,m_b);
		SetOutput(2,n,m_h);
		SetOutput(3,n,m_n);
		SetOutput(4,n,m_p);
	}

}

void SVFilterPlugin::Randomise()
{
}

void SVFilterPlugin::StreamOut (ostream &s) {
     s << m_Version << " " << Cutoff << " " << Resonance << " ";
}

void SVFilterPlugin::StreamIn (istream &s) {
     int version;
     double dummy;
     s >> version;
     switch (version) {
       case 1: // s >> fc >> q;
               s >> dummy >> dummy;
               break;
       case 2: s >> Cutoff >> Resonance;
               break;
     }
}

void SVFilterPlugin::Clear()
{
	m_h=0.0f;
	m_b=0.0f;
	m_l=0.0f;
	m_p=0.0f;
	m_n=0.0f;
}
