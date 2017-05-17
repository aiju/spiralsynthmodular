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
#include "WaveTablePlugin.h"
#include "WaveTablePluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

static const int IN_FREQ  = 0;
static const int IN_PW    = 1;
static const int IN_SHLEN = 2;

static const int OUT_MAIN = 0;

extern "C"
{
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new WaveTablePlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0017;
}

string SpiralPlugin_GetGroupName()
{
	return "Oscillators";
}
}

///////////////////////////////////////////////////////

WaveTablePlugin::WaveTablePlugin() :
m_Type(SINE),
m_Octave(0),
m_FineFreq(1.0f),
m_ModAmount(1.0f),
m_TableLength(DEFAULT_TABLE_LEN)
{
	m_CyclePos=0;	
	m_Note=0;
	
	m_PluginInfo.Name="WaveTable";
	m_PluginInfo.Width=210;
	m_PluginInfo.Height=140;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Frequency CV");
	m_PluginInfo.PortTips.push_back("Output");
	
	m_AudioCH->Register("Octave",&m_Octave,ChannelHandler::INPUT);
	m_AudioCH->Register("FineFreq",&m_FineFreq,ChannelHandler::INPUT);
	m_AudioCH->Register("Type",(char*)&m_Type,ChannelHandler::INPUT);
	m_AudioCH->Register("ModAmount",&m_ModAmount,ChannelHandler::INPUT);
}

WaveTablePlugin::~WaveTablePlugin()
{
}

PluginInfo &WaveTablePlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info= SpiralPlugin::Initialise(Host);
	
	for (int n=0; n<NUM_TABLES; n++)
	{
		m_Table[n].Allocate(m_TableLength);
	}
	
	WriteWaves();
		
	return Info;
}

SpiralGUIType *WaveTablePlugin::CreateGUI()
{
	return new WaveTablePluginGUI(m_PluginInfo.Width,m_PluginInfo.Height,this,m_AudioCH,m_HostInfo);	
}

void WaveTablePlugin::WriteWaves()
{
	float RadCycle = (M_PI/180)*360;
	float Pos=0;

	for (int n=0; n<m_TableLength; n++)
	{
		if (n==0) Pos=0;
		else Pos=(n/(float)m_TableLength)*RadCycle;
		m_Table[SINE].Set(n,sin(Pos));		
	}

	for (int n=0; n<m_TableLength; n++)
	{
		if (n<m_TableLength/2) m_Table[SQUARE].Set(n,1.0f);
		else m_Table[SQUARE].Set(n,-1);				
	}
	
	for (int n=0; n<m_TableLength; n++)
	{
		m_Table[REVSAW].Set(n,((n/(float)m_TableLength)*2.0f)-1.0f);
	}
	
	for (int n=0; n<m_TableLength; n++)
	{
		m_Table[SAW].Set(n,1-(n/(float)m_TableLength)*2.0f);
	}

	float HalfTab=m_TableLength/2;
	float v=0;
	for (int n=0; n<m_TableLength; n++)
	{
		if (n<HalfTab) v=1-(n/HalfTab)*2.0f;
		else v=(((n-HalfTab)/HalfTab)*2.0f)-1.0f;
		v*=0.99;
		m_Table[TRIANGLE].Set(n,v);		
	}

	for (int n=0; n<m_TableLength; n++)
	{
		if (n<m_TableLength/1.2) m_Table[PULSE1].Set(n,1);
		else m_Table[PULSE1].Set(n,-1);				
	}
	
	for (int n=0; n<m_TableLength; n++)
	{
		if (n<m_TableLength/1.5) m_Table[PULSE2].Set(n,1);
		else m_Table[PULSE2].Set(n,-1);				
	}

	Pos=0;
	for (int n=0; n<m_TableLength; n++)
	{
		if (n==0) Pos=0;
		else Pos=(n/(float)m_TableLength)*RadCycle;
		if (sin(Pos)==0) m_Table[INVSINE].Set(n,0);
		else m_Table[INVSINE].Set(n,(1.0f/sin(Pos))/10.0f);				
	}
}

void WaveTablePlugin::Execute()
{
 	float Freq=0;
	float Incr;
	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{	
		SetOutput(OUT_MAIN,n,0);
		
		if (InputExists(0)) 
		{
			Freq=GetInputPitch(0,n);
			Freq*=m_ModAmount;
		}
		else 
		{	
			Freq=110;
		}

		Freq*=m_FineFreq;
		if (m_Octave>0) Freq*=1<<(m_Octave);
		if (m_Octave<0) Freq/=1<<(-m_Octave);
		
		Incr = Freq*(m_TableLength/(float)m_HostInfo->SAMPLERATE);
		m_CyclePos+=Incr;
		
		while (m_CyclePos>=m_TableLength) 
		{
			m_CyclePos-=m_TableLength;
		}
		
		if (m_CyclePos<0 || m_CyclePos>=m_TableLength) m_CyclePos=0;
		
		SetOutput(OUT_MAIN,n,m_Table[m_Type][m_CyclePos]);	
	}
}

void WaveTablePlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<*this;
}

void WaveTablePlugin::StreamIn(istream &s)
{
	int version;
	s>>version>>*this;
}

istream &operator>>(istream &s, WaveTablePlugin &o)
{
	s>>(int&)o.m_Type>>o.m_Octave>>o.m_FineFreq>>o.m_ModAmount;
	return s;
}

ostream &operator<<(ostream &s, WaveTablePlugin &o)
{
	s<<(int)o.m_Type<<" "<<o.m_Octave<<" "<<o.m_FineFreq<<" "<<o.m_ModAmount<<" ";
	return s;
}
