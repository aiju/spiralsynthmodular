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
#include "FilterPlugin.h"
#include "FilterPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

static const float MAX_RES = 10;
static const float MIN_RES = 1;
static const float MAX_CUTOFF = 3000;
static const float MIN_CUTOFF = 500;
static const int   FILTERGRAN = 50;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new FilterPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x000b;
}

string SpiralPlugin_GetGroupName()
{
	return "Filters/FX";
}
}

///////////////////////////////////////////////////////

FilterPlugin::FilterPlugin() :
fs(0),     			// Sampling frequency
fc(100),             // Filter cutoff
Q(1),               // Resonance
m_LastFC(0),
m_LastQ(1),
k(1.0),              // Set overall filter gain
m_RevCutoffMod(false),
m_RevResonanceMod(false)
{
	m_PluginInfo.Name="Filter";
	m_PluginInfo.Width=120;
	m_PluginInfo.Height=110;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Cutoff CV");	
	m_PluginInfo.PortTips.push_back("Emphasis CV");	
	m_PluginInfo.PortTips.push_back("Output");
	
	m_AudioCH->Register("Cutoff",&fc);
	m_AudioCH->Register("Resonance",&Q);
	m_AudioCH->Register("RevC",&m_RevCutoffMod);
	m_AudioCH->Register("RevR",&m_RevResonanceMod);
	
}

FilterPlugin::~FilterPlugin()
{
}

PluginInfo &FilterPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	
	fs=m_HostInfo->SAMPLERATE;
	
	SetupCoeffs();
	
    iir.length = FILTER_SECTIONS;         	
    iir.coef = (float *) calloc(4 * iir.length + 1 , sizeof(float)); 
	
	if (!iir.coef) 
    { 
         printf("Unable to allocate coef array, exiting\n"); 
         exit(1); 
    } 		
	
	return Info;
}

SpiralGUIType *FilterPlugin::CreateGUI()
{
	return new FilterPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void FilterPlugin::Execute()
{
	float Cutoff;
    float Resonance;

    if (fc<0) return;
   
    for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
  	 	coef = iir.coef + 1;     /* Skip k, or gain */ 
   	 	k=0.25;
	
	 	Cutoff = fc + (GetInput(1,n) * 1000);
	 	Resonance = Q + GetInput(2,n);  
	 	
	 	Cutoff/=2;
	 
	 	if (Resonance>MAX_RES) Resonance=MAX_RES; 
	 	if (Cutoff>MAX_CUTOFF) Cutoff=MAX_CUTOFF; 
	 	if (Resonance<MIN_RES) Resonance=MIN_RES; 
	 	if (Cutoff<MIN_CUTOFF) Cutoff=MIN_CUTOFF; 
	 
	 	if (n%FILTERGRAN==0)
	 	{
	 		for (nInd = 0; nInd < iir.length; nInd++) 
    		{
			    a2 = ProtoCoef[nInd].a2; 
		
	            a0 = ProtoCoef[nInd].a0; 
	            a1 = ProtoCoef[nInd].a1; 
	          
	            b0 = ProtoCoef[nInd].b0; 
	            b1 = ProtoCoef[nInd].b1 / Resonance;      
	            b2 = ProtoCoef[nInd].b2; 	
	            szxform(&a0, &a1, &a2, &b0, &b1, &b2, Cutoff*(Cutoff/1000.0f), fs, &k, coef); 
   		        coef += 4;                       

	        	iir.coef[0] = k; 
				
				m_LastQ=Q;
				m_LastFC=fc;			
			}
		 }		  
		 		 
		 float in = GetInput(0,n);
		 //if (in!=0)
		 //{
		 	SetOutput(0,n,iir_filter(in/0.5f,&iir));	 
		 //}
		 //else
		 //{
		 //	m_Output[0]->Set(n,0);
		 //}
	}
		
}
	
void FilterPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<fc<<" "<<Q<<" "<<m_RevCutoffMod<<" "<<m_RevResonanceMod<<" ";
}

void FilterPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	s>>fc>>Q>>m_RevCutoffMod>>m_RevResonanceMod;
}

void FilterPlugin::SetupCoeffs()
{
	ProtoCoef[0].a0 = 1.0; 
    ProtoCoef[0].a1 = 0; 
    ProtoCoef[0].a2 = 0; 
    ProtoCoef[0].b0 = 1.0; 
    ProtoCoef[0].b1 = 0.765367; 
    ProtoCoef[0].b2 = 1.0; 

    ProtoCoef[1].a0 = 1.0; 
    ProtoCoef[1].a1 = 0; 
    ProtoCoef[1].a2 = 0; 
    ProtoCoef[1].b0 = 1.0; 
    ProtoCoef[1].b1 = 1.847759; 
    ProtoCoef[1].b2 = 1.0; 
} 
