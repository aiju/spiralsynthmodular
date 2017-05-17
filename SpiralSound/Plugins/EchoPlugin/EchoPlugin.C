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
#include "EchoPlugin.h"
#include "EchoPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

static const float MAX_DELAY=1.0f;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new EchoPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x000e;
}

string SpiralPlugin_GetGroupName()
{
	return "Delay/Sampling";
}
}

///////////////////////////////////////////////////////

EchoPlugin::EchoPlugin() :
m_Delay (0.75),
m_Feedback (0.4),
m_Bounce (false),
m_HeadPos (0),
m_Buf0 (0),
m_Buf1 (1)
{
        m_Version = 2;
        m_PluginInfo.Name = "Echo";
	m_PluginInfo.Width = 120;
	m_PluginInfo.Height = 140;
	m_PluginInfo.NumInputs = 3;
	m_PluginInfo.NumOutputs = 2;
	m_PluginInfo.PortTips.push_back ("Input");
	m_PluginInfo.PortTips.push_back ("Delay CV");
	m_PluginInfo.PortTips.push_back ("Feedback CV");
	m_PluginInfo.PortTips.push_back ("Left/Mono Out");
	m_PluginInfo.PortTips.push_back ("Right Out");
	m_AudioCH->Register ("Delay", &m_Delay);
	m_AudioCH->Register ("Feedback", &m_Feedback);
	m_AudioCH->Register ("Bounce", &m_Bounce);
}

EchoPlugin::~EchoPlugin()
{
}

PluginInfo &EchoPlugin::Initialise (const HostInfo *Host)
{
	PluginInfo& Info = SpiralPlugin::Initialise (Host);
	m_Buffer[0].Allocate ((int)(m_HostInfo->SAMPLERATE * MAX_DELAY));
	m_Buffer[1].Allocate ((int)(m_HostInfo->SAMPLERATE * MAX_DELAY));
	return Info;
}

SpiralGUIType *EchoPlugin::CreateGUI()
{
	return new EchoPluginGUI (m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void EchoPlugin::Execute()
{
     float Delay;
     for (int n=0; n<m_HostInfo->BUFSIZE; n++) {
         Delay=( m_Delay+GetInput (1,n)*0.5 ) * (float)m_HostInfo->SAMPLERATE-1;
         if (Delay >= MAX_DELAY*m_HostInfo->SAMPLERATE)
            Delay = (MAX_DELAY*m_HostInfo->SAMPLERATE)-1;
         if (Delay<0) Delay=0;
         if (m_HeadPos>Delay) m_HeadPos=0;
         if (m_Bounce && m_HeadPos==0) {
            int c=m_Buf0;
            m_Buf0=m_Buf1;
            m_Buf1=c;
         }
         // Write to the buffer
         m_Buffer[m_Buf0].Set (m_HeadPos, GetInput (0,n)+(m_Buffer[m_Buf0][m_HeadPos]*(m_Feedback+GetInput (2,n))));
         if (!m_Bounce) m_Buffer[m_Buf1].Set (m_HeadPos, 0);
         // Read from the buffer
         SetOutput(0, n, m_Buffer[m_Buf0][m_HeadPos]);
         SetOutput(1, n, m_Buffer[m_Buf1][m_HeadPos]);
         m_HeadPos++;
     }

}

//void EchoPlugin::Randomise()
//{
//}

void EchoPlugin::StreamOut(ostream &s)
{
	s << m_Version << " " << m_Delay << " " << m_Feedback << " " << m_Bounce << " ";
}

void EchoPlugin::StreamIn(istream &s)
{
	int version;
	s >> version;
	s >> m_Delay >> m_Feedback;
        if (version>1) s >> m_Bounce; else m_Bounce = false;
}

