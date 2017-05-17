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
#include "OscillatorPlugin.h"
#include "OscillatorPluginGUI.h"
#include <FL/Fl_Button.h>
#include <limits.h>
#include "SpiralIcon.xpm"

using namespace std;

static const int IN_FREQ  = 0;
static const int IN_PW    = 1;
static const int IN_SHLEN = 2;

static const int OUT_MAIN = 0;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new OscillatorPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0004;
}

string SpiralPlugin_GetGroupName()
{
	return "Oscillators";
}
}

///////////////////////////////////////////////////////

OscillatorPlugin::OscillatorPlugin() :
m_Type(SQUARE),
m_Octave(0),
m_FineFreq(1.0f),
m_PulseWidth(0.5f),
m_SHLen (0.1f),
m_ModAmount(1.0f),
m_Noisev(0),
m_FreqModBuf(NULL),
m_PulseWidthModBuf(NULL),
m_SHModBuf(NULL)
{

	m_CyclePos=0;
	m_Note=0;
	m_LastFreq=0;

	m_PluginInfo.Name="Oscillator";
	m_PluginInfo.Width=210;
	m_PluginInfo.Height=140;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Frequency CV");
	m_PluginInfo.PortTips.push_back("PulseWidth CV");
	m_PluginInfo.PortTips.push_back("Sample & Hold length CV");
	m_PluginInfo.PortTips.push_back("Output");

	m_AudioCH->Register("Octave",&m_Octave);
	m_AudioCH->Register("FineFreq",&m_FineFreq);
	m_AudioCH->Register("PulseWidth",&m_PulseWidth);
	m_AudioCH->Register("Type",(char*)&m_Type);
	m_AudioCH->Register("SHLen",&m_SHLen);
	m_AudioCH->Register("ModAmount",&m_ModAmount);	
}

OscillatorPlugin::~OscillatorPlugin()
{
}

PluginInfo &OscillatorPlugin::Initialise(const HostInfo *Host)
{
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *OscillatorPlugin::CreateGUI()
{
	return new OscillatorPluginGUI(m_PluginInfo.Width,
										  m_PluginInfo.Height,
										  this,m_AudioCH,m_HostInfo);
}

void OscillatorPlugin::Execute()
{
	short noisev=0;
	float Freq=0;
	float CycleLen=0;
	int samplelen, PW;

	switch (m_Type)
	{
	case SQUARE:
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{
			if (InputExists(0)) Freq=GetInputPitch(0,n);
			else Freq=110;
			Freq*=m_FineFreq;
			if (m_Octave>0) Freq*=1<<(m_Octave);
			if (m_Octave<0) Freq/=1<<(-m_Octave);
			CycleLen = m_HostInfo->SAMPLERATE/Freq;
			PW = (int)((m_PulseWidth+GetInput(IN_PW,n)*m_ModAmount) * CycleLen);

			// calculate square wave pattern
			m_CyclePos++;
			if (m_CyclePos>CycleLen) m_CyclePos=0;

			if (m_CyclePos<PW) SetOutput(OUT_MAIN,n,1);
			else SetOutput(OUT_MAIN,n,-1);
		}
		break;

	case SAW:
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{
			if (InputExists(0)) Freq=GetInputPitch(0,n);
			else Freq=110;
			Freq*=m_FineFreq;
			if (m_Octave>0) Freq*=1<<(m_Octave);
			if (m_Octave<0) Freq/=1<<(-m_Octave);
			CycleLen = m_HostInfo->SAMPLERATE/Freq;
			PW = (int)((m_PulseWidth+GetInput(IN_PW,n)*m_ModAmount) * CycleLen);

			// get normailise position between cycle
			m_CyclePos++;
			if (m_CyclePos>CycleLen) m_CyclePos=0;

			if (m_CyclePos<PW)
			{
				// before pw -1->1
				SetOutput(OUT_MAIN,n,Linear(0,PW,m_CyclePos,-1,1));
			}
			else
			{
				// after pw 1->-1
				SetOutput(OUT_MAIN,n,Linear(PW,CycleLen,m_CyclePos,1,-1));
			}
		}
		break;

	case NOISE:
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{
			m_CyclePos++;

			//modulate the sample & hold length
			samplelen = (int)((m_SHLen+GetInput(IN_SHLEN,n)*m_ModAmount)*m_HostInfo->SAMPLERATE);

			// do sample & hold on the noise
			if (m_CyclePos>samplelen)
			{
				m_Noisev=(short)((rand()%SHRT_MAX*2)-SHRT_MAX);
				m_CyclePos=0;
			}
			SetOutput(OUT_MAIN,n,m_Noisev/(float)SHRT_MAX);
		}
		break;

	case NONE: break;
	}
}

void OscillatorPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<*this;
}

void OscillatorPlugin::StreamIn(istream &s)
{
	int version;
	s>>version>>*this;
}

istream &operator>>(istream &s, OscillatorPlugin &o)
{
	float dummy=0;
	s>>(int&)o.m_Type>>o.m_Octave>>o.m_FineFreq>>o.m_PulseWidth>>dummy>>
	o.m_SHLen>>o.m_ModAmount;
	return s;
}

ostream &operator<<(ostream &s, OscillatorPlugin &o)
{
	float dummy=0;
	s<<(int)o.m_Type<<" "<<o.m_Octave<<" "<<o.m_FineFreq<<" "<<o.m_PulseWidth<<" "<<
	dummy<<" "<<o.m_SHLen<<" "<<o.m_ModAmount<<" ";
	return s;
}
