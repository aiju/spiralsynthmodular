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
#include "MoogFilterPlugin.h"
#include "MoogFilterPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

#define PI 3.141592654

using namespace std;

static const int GRANULARITY = 10;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new MoogFilterPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x000d;
}

string SpiralPlugin_GetGroupName()
{
	return "Filters/FX";
}
}

///////////////////////////////////////////////////////

MoogFilterPlugin::MoogFilterPlugin() :
Cutoff(0.5f),
Resonance(0.0f),
fs(44100.0f),
fc(1000.0f),
f(0.0f),
p(0.0f),
q(0.0f),
b0(0.1f),
b1(0.0f),
b2(0.0f),
b3(0.0f),
b4(0.0f),
t1(0.0f),
t2(0.0f)
{
	m_PluginInfo.Name="Moog Filter";
	m_PluginInfo.Width=120;
	m_PluginInfo.Height=110;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=3;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Cutoff CV");	
	m_PluginInfo.PortTips.push_back("Emphasis CV");	
	m_PluginInfo.PortTips.push_back("LowPass output");
	m_PluginInfo.PortTips.push_back("BandPass output");
	m_PluginInfo.PortTips.push_back("HighPass output");
	
	m_AudioCH->Register("Cutoff",&Cutoff);
	m_AudioCH->Register("Resonance",&Resonance);
}

MoogFilterPlugin::~MoogFilterPlugin()
{
}

PluginInfo &MoogFilterPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	fs = m_HostInfo->SAMPLERATE;
	return Info;
}

SpiralGUIType *MoogFilterPlugin::CreateGUI()
{
	return new MoogFilterPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void MoogFilterPlugin::Execute()
{
	float in, Q;
	
	// Early out?	
	// We don't want to early out if the resonance is set to
	// self oscillation, as this turns into an generator
	// in it's own right...
	/*if (Resonance<0.5 && !GetInput(2))
	{
		if (!m_Input[0] || m_Input[0]->IsEmpty())
		{
			cerr<<"moog early out"<<endl;
		
			m_Output[0]->Zero();
			m_Output[1]->Zero();	
			m_Output[2]->Zero();				
			return;
		}
	}*/
	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		if (n%GRANULARITY==0)
		{
			fc = (Cutoff+GetInput(1,n))*0.25;
			if (fc<0) fc=0;
			
			q = 1.0f - fc;
			p = fc + 0.8f * fc * q;
			f = p + p - 1.0f;
			Q = ((Resonance+GetInput(2,n))*6)-3.0f;
			q = Q + (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
		}

		in = GetInput(0,n);
		
		if (in == 0)
		{
			Clear();
			SetOutput(0,n,0);	 
	  		SetOutput(1,n,0);
			SetOutput(2,n,0);	
		} else {				
			in -= q * b4;
			
			if (in>1) in=1;
			if (in<-1) in=-1;
									
			t1 = b1; b1 = (in + b0) * p - b1 * f;
			t2 = b2; b2 = (b1 + t1) * p - b2 * f;
			t1 = b3; b3 = (b2 + t2) * p - b3 * f;		
     			     b4 = (b3 + t1) * p - b4 * f;	
			b4 = b4 - b4 * b4 * b4 * 0.166667f;
			
			b0 = in;	
			 
			SetOutput(0,n,b4);	 
			SetOutput(1,n,(in-b4));
			SetOutput(2,n,3.0f * (b3 - b4));	
		}
	}			
}

void MoogFilterPlugin::Clear()
{
	b0 = 0.0f;
	b1 = 0.0f;
	b2 = 0.0f;
	b3 = 0.0f;
	b4 = 0.0f;
	t1 = 0.0f;
	t2 = 0.0f;
}

void MoogFilterPlugin::Randomise()
{
}
	
void MoogFilterPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<Cutoff<<" "<<Resonance<<" ";
}

void MoogFilterPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	s>>Cutoff>>Resonance;
}
