/*  SpiralSound
 *  Copyleft (C) 2002 Andy Preston <andy@clubunix.co.uk>
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

#include "LFOPlugin.h"
#include "LFOPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {

SpiralPlugin* SpiralPlugin_CreateInstance() { return new LFOPlugin; }

char** SpiralPlugin_GetIcon() { return SpiralIcon_xpm; }

int SpiralPlugin_GetID() { return 124; }

string SpiralPlugin_GetGroupName() { return "Oscillators"; }

}

///////////////////////////////////////////////////////

LFOPlugin::LFOPlugin() :
m_Type (SINE),
m_Freq (0.1),
m_TableLength (DEFAULT_TABLE_LEN) {
	m_CyclePos = 0;
	m_Note = 0;
	m_PluginInfo.Name = "LFO";
	m_PluginInfo.Width = 180;
	m_PluginInfo.Height = 100;
	m_PluginInfo.NumInputs = 0;
	m_PluginInfo.NumOutputs = 3;
	m_PluginInfo.PortTips.push_back ("Output");
	m_PluginInfo.PortTips.push_back ("'Cosine' Output");
	m_PluginInfo.PortTips.push_back ("Inverted Output");
	m_AudioCH->Register("Freq", &m_Freq);
	m_AudioCH->Register("Type", (char*)&m_Type);
}

LFOPlugin::~LFOPlugin() {
}

PluginInfo &LFOPlugin::Initialise (const HostInfo *Host) {
           PluginInfo& Info= SpiralPlugin::Initialise (Host);
           for (int n=0; n<NUM_TABLES; n++)
               m_Table[n].Allocate (m_TableLength);
           WriteWaves();
           return Info;
}

SpiralGUIType *LFOPlugin::CreateGUI() {
      return new LFOPluginGUI(m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void LFOPlugin::WriteWaves() {
     float RadCycle = (M_PI/180) * 360;
     float Pos = 0;
     float v = 0;
     float HalfTab = m_TableLength / 2;
     int QuatTab = m_TableLength / 4;
     int ThreeQuatTab = m_TableLength - QuatTab;
     int Shift;

     for (int n=0; n<m_TableLength; n++) {

         if (n==0) Pos = 0; else Pos = (n/(float)m_TableLength) * RadCycle;
         m_Table[SINE].Set (n, sin (Pos));

         if (n < QuatTab) Shift=n+ThreeQuatTab; else Shift=n-QuatTab;
         if (n<QuatTab || n>ThreeQuatTab) v = (((Shift-HalfTab) / HalfTab) * 2) - 1;
         else v = 1 - (Shift/HalfTab * 2);
	 m_Table[TRIANGLE].Set (n, v);

         if (n<m_TableLength/2) m_Table[SQUARE].Set (n, 1.0f);
         else m_Table[SQUARE].Set (n, -1);

         m_Table[SAW].Set (n, 1.0f - (n / (float)m_TableLength) * 2.0f);
     }
}

float LFOPlugin::AdjustPos (float pos) {
      while (pos>=m_TableLength) pos -= m_TableLength;
      if (pos<0 || pos>=m_TableLength) pos = 0;
      return pos;
}

void LFOPlugin::Execute() {
     float Incr, Pos;
     for (int n=0; n<m_HostInfo->BUFSIZE; n++) {
         Incr = m_Freq * (m_TableLength / (float)m_HostInfo->SAMPLERATE);
         // Raw Output
         m_CyclePos = AdjustPos (m_CyclePos + Incr);
         SetOutput (0, n, m_Table[m_Type][m_CyclePos]);
         // 'Cosine' Output
         Pos = AdjustPos (m_CyclePos + (m_TableLength * 0.25));
         SetOutput (1, n, m_Table[m_Type][Pos]);
         // Inverted Output
         Pos = AdjustPos (m_TableLength - m_CyclePos);
         SetOutput (2, n, m_Table[m_Type][Pos]);
     }
}

void LFOPlugin::StreamOut(ostream &s) {
     s << m_Version << " " << *this;
}

void LFOPlugin::StreamIn(istream &s) {
     int version;
     s >> version >> *this;
}

istream &operator>> (istream &s, LFOPlugin &o) {
        s >> (int&)o.m_Type >> o.m_Freq;
	return s;
}

ostream &operator<< (ostream &s, LFOPlugin &o) {
	s << (int)o.m_Type << " " << o.m_Freq << " ";
	return s;
}
