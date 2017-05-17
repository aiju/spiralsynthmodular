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
#include "EnvelopePlugin.h"
#include "EnvelopePluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new EnvelopePlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0005;
}

string SpiralPlugin_GetGroupName()
{
	return "Control";
}
}

///////////////////////////////////////////////////////

EnvelopePlugin::EnvelopePlugin() :
m_Trigger(false),
m_t(-1.0f),
m_Attack(0.0f),
m_Decay(0.5f),
m_Sustain(1.0f),
m_Release(1.0f),
m_Volume(0.5f),
m_TrigThresh(0.01)
{
	m_PluginInfo.Name="Envelope";
	m_PluginInfo.Width=142;
	m_PluginInfo.Height=132;
	m_PluginInfo.NumInputs=2;
	m_PluginInfo.NumOutputs=2;
	m_PluginInfo.PortTips.push_back("Trigger CV");	
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("CV");	
	m_PluginInfo.PortTips.push_back("Output");
	
	m_AudioCH->Register("Attack",&m_Attack);
	m_AudioCH->Register("Decay",&m_Decay);
	m_AudioCH->Register("Sustain",&m_Sustain);
	m_AudioCH->Register("Release",&m_Release);
	m_AudioCH->Register("Volume",&m_Volume);
	m_AudioCH->Register("Trig",&m_TrigThresh);
}

EnvelopePlugin::~EnvelopePlugin()
{
}

PluginInfo &EnvelopePlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	m_SampleTime=1.0/(float)(m_HostInfo->SAMPLERATE);
	return Info;
}

SpiralGUIType *EnvelopePlugin::CreateGUI()
{
	return new EnvelopePluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void EnvelopePlugin::Execute()
{
	float temp=0;
	bool Freeze=false;
	
	// Early out?	
	/*if (m_t<0 && (!m_Input[0] || m_Input[0]->IsEmpty()))
	{
		m_Output[0]->Zero();
		m_Output[1]->Zero();				
		return;
	}*/
		
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{	
		// Check the trigger CV values
		if (GetInput(0,n)>m_TrigThresh)
		{
			if (m_Trigger==false) 
			{
				m_t=0.0f; 
				m_Trigger=true;
			}
		}
		else
		{	
			m_Trigger=false;
		}
		
		// if we are in the envelope...
		if (m_t>=0 && m_t<m_Attack+m_Decay+m_Release) 
		{				
			// find out what part of the envelope we are in	
			// in the attack
			if (m_t<m_Attack)
			{			
				// get normalised position to
				// get the volume between 0 and 1
				temp=m_t/m_Attack;
			}
			else
			// in the decay
			if (m_t<m_Attack+m_Decay)
			{
				//if (n==0) cerr<<"in decay"<<endl;
				// normalised position in m_Attack->m_Decay range
				float nt=(m_t-m_Attack)/m_Decay;
				
				// volume between 1 and m_Sustain
				temp=(1-nt)+(m_Sustain*nt);
			}
			else
			// in the release
			{
				//if (n==0) cerr<<"in release"<<endl;
				// normalised position in m_Decay->m_Release range
				float nt=(m_t-(m_Attack+m_Decay))/m_Release;
				
				// volume between m_Sustain and 0			
				temp=m_Sustain*(1-nt);
				
				if (m_Release<0.2f)
				{
					temp=m_Sustain;
				}	
				
				if (m_Trigger) Freeze=true;
			}
			
			temp*=m_Volume;
			
			SetOutput(0,n,temp);
			SetOutput(1,n,GetInput(1,n)*temp); 
				
			if (!Freeze) m_t+=m_SampleTime;
		}
		else
		{
			SetOutput(0,n,0);
			SetOutput(1,n,0); 
			
			// if we've run off the end
			if (m_t>m_Attack+m_Decay+m_Release)
			{
				m_t=-1;
				//m_Output[0]->Zero();
				//m_Output[1]->Zero();
				return;
			}

		}
	} 	
}

void EnvelopePlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<m_Attack<<" "<<m_Decay<<" "<<m_Sustain<<" "<<
	m_Release<<" "<<m_Volume<<" "<<m_TrigThresh;
}

void EnvelopePlugin::StreamIn(istream &s)
{
	int version;
	s>>version;
	s>>m_Attack>>m_Decay>>m_Sustain>>m_Release>>m_Volume>>m_TrigThresh;
}
