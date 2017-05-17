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
#include "AnotherFilterPlugin.h"
#include "AnotherFilterPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

#define PI 3.141592654

using namespace std;

static const int GRANULARITY = 10;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new AnotherFilterPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 43;
}

string SpiralPlugin_GetGroupName()
{
	return "Filters/FX";
}
}

///////////////////////////////////////////////////////

AnotherFilterPlugin::AnotherFilterPlugin() :
Cutoff(0.0f),
Resonance(0.0f),
vibrapos(0.0f),
vibraspeed(0.0f)
{
	m_PluginInfo.Name="AnotherLPF";
	m_PluginInfo.Width=120;
	m_PluginInfo.Height=110;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Cutoff CV");	
	m_PluginInfo.PortTips.push_back("Emphasis CV");	
	m_PluginInfo.PortTips.push_back("LowPass output");
	
	m_AudioCH->Register("Cutoff",&Cutoff);
	m_AudioCH->Register("Resonance",&Resonance);
}

AnotherFilterPlugin::~AnotherFilterPlugin()
{
}

PluginInfo &AnotherFilterPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);		
}

SpiralGUIType *AnotherFilterPlugin::CreateGUI()
{
	return new AnotherFilterPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void AnotherFilterPlugin::Execute()
{
	float in;
					
    for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		if (n%GRANULARITY==0)
		{
			w = 2.0*PI*((Cutoff+GetInput(1,n))*10000+1)/m_HostInfo->SAMPLERATE; // Pole angle
			q = 1.0-w/(2.0*(((Resonance+GetInput(2,n))*10+1)+0.5/(1.0+w))+w-2.0); // Pole magnitude
			r = q*q;
			c = r+1.0-2.0*cos(w)*q;
		}

		/* Accelerate vibra by signal-vibra, multiplied by lowpasscutoff */
  		vibraspeed += (GetInput(0,n)*0.3f - vibrapos) * c;

  		/* Add velocity to vibra's position */
  		vibrapos += vibraspeed;

  		/* Attenuate/amplify vibra's velocity by resonance */
  		vibraspeed *= r;

		// needs clipping :(
		if (vibrapos>1.0f) vibrapos=1.0f;
		if (vibrapos<-1.0f) vibrapos=-1.0f;

  		/* Store new value */
  		SetOutput(0,n,vibrapos);
	}
}
	
void AnotherFilterPlugin::Randomise()
{
}
	
void AnotherFilterPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<Cutoff<<" "<<Resonance<<" ";
}

void AnotherFilterPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	s>>Cutoff>>Resonance;
}

