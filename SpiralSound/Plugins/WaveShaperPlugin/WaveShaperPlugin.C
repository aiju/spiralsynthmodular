/*  WaveShaper Plugin Copyleft (C) 2001 Yves Usson
 *  for SpiralSynthModular
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
#include "WaveShaperPlugin.h"
#include "WaveShaperPluginGUI.h"
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {

SpiralPlugin* SpiralPlugin_CreateInstance() {
  return new WaveShaperPlugin;
}

char** SpiralPlugin_GetIcon() {
  return SpiralIcon_xpm;
}

int SpiralPlugin_GetID() {
  return 0x0032;
}
string SpiralPlugin_GetGroupName()
{
	return "Filters/FX";
}
}

///////////////////////////////////////////////////////

WaveShaperPlugin::WaveShaperPlugin () {
  int i;
  m_PluginInfo.Name = "WaveShaper";
  m_PluginInfo.Width = 278;
  m_PluginInfo.Height = 260;
  m_PluginInfo.NumInputs = 1;
  m_PluginInfo.NumOutputs = 1;
  m_PluginInfo.PortTips.push_back ("Input");
  m_PluginInfo.PortTips.push_back ("Out");
  m_wt = new float[512];
  for (i=0; i<512; i++) m_wt[i] = (i / 256.0 - 1.0);
  m_GUIArgs.FuncPlot = new float[256];
  for (i=0; i<256; i++) m_GUIArgs.FuncPlot[i] = m_wt[i*2];
  m_Wave = 1;
  for (i=0; i<6; i++) m_Coefs[i] = 0.0;
  m_Coefs[0] = 1.0;
  m_AudioCH->Register ("WaveType", &m_GUIArgs.WaveType);
  m_AudioCH->Register ("CoefNum", &m_GUIArgs.CoefNum);
  m_AudioCH->Register ("CoefVal", &m_GUIArgs.CoefVal);
  m_AudioCH->RegisterData ("WT", ChannelHandler::OUTPUT, m_GUIArgs.FuncPlot, 256 * sizeof (float));
}

WaveShaperPlugin::~WaveShaperPlugin() {
  delete [] m_GUIArgs.FuncPlot;
  delete [] m_wt;
}

PluginInfo &WaveShaperPlugin::Initialise (const HostInfo *Host) {
  PluginInfo& Info = SpiralPlugin::Initialise (Host);
  calc();
  return Info;
}

SpiralGUIType *WaveShaperPlugin::CreateGUI () {
  return new WaveShaperPluginGUI (m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void WaveShaperPlugin::ExecuteCommands () {
  if (m_AudioCH->IsCommandWaiting ()) {
    switch (m_AudioCH->GetCommand ()) {
      case (SETWAVETYPE) :
        m_Wave = m_GUIArgs.WaveType;
        calc ();
        break;
      case (SETCOEF) :
        if ((m_GUIArgs.CoefNum < 0) || (m_GUIArgs.CoefNum > 5)) break;
        m_Coefs[m_GUIArgs.CoefNum] = m_GUIArgs.CoefVal;
        calc ();
        break;
    }
  }
}

void WaveShaperPlugin::Execute () {
  float k1, k2;
  if (!InputExists (0)) return;
  for (int i=0; i<m_HostInfo->BUFSIZE; i++) {
    float v = GetInput (0, i);
    int index = (short)(256.0*v) + 256;
    // short rm = v - ((index-256)*(long)SpiralInfo::MAXSAMPLE)/256;
    if (index < 0) index = 0;
    if (index > 511) index = 511;
    // k1 = rm / (SpiralInfo::MAXSAMPLE/256.0);
    // v = (short)(k1*(wt[index]-wt[index+1])+wt[index+1]);
    SetOutput (0, i, m_wt[index]);
  }
}

// Functions used by GUI.UpdateValues

float WaveShaperPlugin::GetCoef (int index) {
  if ((index < 0) || (index > 5)) return 0;
  return m_Coefs[index];
}

int WaveShaperPlugin::GetWaveType(){
  return m_Wave;
}

// Internal private functions

void WaveShaperPlugin::set (int index, float v) {
  if ((index < 0) || (index > 511)) return;
  if (v > 1.0f) v = 1.0f;
  if (v < -1.0f) v = -1.0f;
  m_wt[index] = v;
}

void WaveShaperPlugin::calc (void) {
  int i;
  float xx,x,y,max;
  if (m_Wave) {
    max = 1.0;
    for (i=0; i<512; i++) {
      x = i / 256.0 - 1.0;
      y = 0;
      xx = x;
      for (int j=1; j<7; j++) {
        y += m_Coefs[j-1] * xx;
	xx *= x;
      }
      y = fabs(y);
      max = max > y? max:y;
    }
    for (i=0; i<512; i++) {
      x = i / 256.0 - 1.0;
      y = 0;
      xx = x;
      for (int j=1; j<7; j++) {
        y += m_Coefs[j-1] * xx;
        xx *= x;
      }
      y /= max;
      set (i, y);
    }
  }
  else {
    max = 1.0;
    for (i=0; i<512; i++) {
      x = (i / 256.0 - 1.0)*M_PI;
      y = 0;
      for (int j=1; j<7; j++) y += m_Coefs[j-1] * sin(x*(1+(j-1)*3));
      y = fabs(y);
      max = max > y? max:y;
    }
    for (i=0; i<512; i++) {
      x = (i / 256.0 - 1.0)*M_PI;
      y = 0;
      for (int j=1; j<7; j++) y += m_Coefs[j-1] * sin(x*(1+(j-1)*3));
      y /= max;
      set(i, y);
    }
  }
  for (i=0; i<256; i++) m_GUIArgs.FuncPlot[i] = m_wt[i*2];
}

// Streaming

void WaveShaperPlugin::StreamOut (ostream &s) {
  s << m_Version << " " << m_Wave;
  for (int i=0; i<6; i++) s << " " << m_Coefs[i];
}

void WaveShaperPlugin::StreamIn (istream &s) {
  int version;
  s >> version >> m_Wave;
  for (int i=0; i<6; i++) s >> m_Coefs[i];
  calc ();
}

