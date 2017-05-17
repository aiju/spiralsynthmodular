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
#include "MixSwitchPlugin.h"
#include "MixSwitchPluginGUI.h"
#include <FL/Fl_Button.h>
#include <stdio.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {

   SpiralPlugin* SpiralPlugin_CreateInstance () { return new MixSwitchPlugin; }

   char** SpiralPlugin_GetIcon () { return SpiralIcon_xpm; }

   int SpiralPlugin_GetID () { return 126; }

   string SpiralPlugin_GetGroupName() { return "Maths/Logic"; }
}

///////////////////////////////////////////////////////

MixSwitchPlugin::MixSwitchPlugin () :
m_SwitchPos (1),
m_Triggered (false)
{
  m_GUIArgs.Chans = 2;
  m_GUIArgs.Switch = 1;
  m_GUIArgs.Echo = 1;
  m_GUIArgs.Auto = false;
  m_PluginInfo.Name = "MixSwitch";
  m_PluginInfo.Width = 80;
  m_PluginInfo.Height = 80;
  CreatePorts ();
  // Number of channels <- GUI
  m_AudioCH->Register ("Chans", &m_GUIArgs.Chans);
  // Switch Position <- GUI
  m_AudioCH->Register ("Switch", &m_GUIArgs.Switch);
  // Switch Position -> GUI
  m_AudioCH->Register ("Echo", &m_GUIArgs.Echo, ChannelHandler::OUTPUT);
  // Auto Mode (Switch position is controlled by CV or Clock = True, Switch Position controlled by GUI = False
  m_AudioCH->Register ("Auto", &m_GUIArgs.Auto, ChannelHandler::OUTPUT);
}

MixSwitchPlugin::~MixSwitchPlugin () {
}

PluginInfo &MixSwitchPlugin::Initialise (const HostInfo *Host) {
  return SpiralPlugin::Initialise (Host);
}

SpiralGUIType *MixSwitchPlugin::CreateGUI () {
  return new MixSwitchPluginGUI (m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void MixSwitchPlugin::ExecuteCommands () {
  if (m_AudioCH->IsCommandWaiting ()) {
    switch (m_AudioCH->GetCommand()) {
      case SETCHANS : SetChans (m_GUIArgs.Chans);
                      break;
    }
  }
}

void MixSwitchPlugin::CreatePorts (int n, bool AddPorts) {
  int c;
  m_PluginInfo.NumInputs = 2 + n;
  m_PluginInfo.PortTips.push_back ("CV");
  m_PluginInfo.PortTips.push_back ("Clock");
  char t[256];
  for (c=1; c<=n; c++) {
    sprintf (t, "In %d", c);
    m_PluginInfo.PortTips.push_back (t);
  }
  m_PluginInfo.NumOutputs = 2;
  m_PluginInfo.PortTips.push_back ("CV");
  m_PluginInfo.PortTips.push_back ("Out");
  if (AddPorts) {
    for (c=0; c<m_PluginInfo.NumInputs; c++) AddInput();
    for (c=0; c<m_PluginInfo.NumOutputs; c++) AddOutput();
  }
}

void MixSwitchPlugin::SetChans (int n) {
  // once to clear the connections with the current info
  // do we need this????
  UpdatePluginInfoWithHost();
  // Things can get a bit confused deleting and adding inputs
  // so we just chuck away all the ports...
  RemoveAllInputs ();
  RemoveAllOutputs ();
  m_PluginInfo.NumInputs = 0;
  m_PluginInfo.NumOutputs = 0;
  m_PluginInfo.PortTips.clear ();
  // ... and then create some new ones
  CreatePorts (n, true);
  // do the actual update
  UpdatePluginInfoWithHost ();
}

void MixSwitchPlugin::Execute() {
  float f;
  int p;
  int NumChans = m_PluginInfo.NumInputs - 2;
  for (int n=0; n<m_HostInfo->BUFSIZE; n++) {
    if (InputExists (0)) {
      m_GUIArgs.Auto = true;
      // Check the Switch Pos CV Value
      m_SwitchPos = int (GetInput (0, n));
    }
    else if (InputExists (1)) {
      m_GUIArgs.Auto = true;
      // Check the trigger CV value
      if (GetInput (1, n) < 0.01) {
        m_Triggered = false;
      }
      else {
        if (!m_Triggered) {
          m_Triggered = true;
          m_SwitchPos = m_SwitchPos + 1;
        }
      }
    }
    else {
      m_GUIArgs.Auto = false;
      m_SwitchPos = m_GUIArgs.Switch;
    }
    if (m_SwitchPos > NumChans) m_SwitchPos = 1;
    m_GUIArgs.Echo = m_SwitchPos;
    SetOutput (0, n, m_SwitchPos);
    p = m_SwitchPos + 1; // SwitchPos 1 = Port 2, and so on
    if (InputExists (p)) f=GetInput (p, n);
    else f=0.0;
    SetOutput (1, n, f);
  }
}

void MixSwitchPlugin::StreamOut (ostream &s) {
  s << m_Version << " " << m_PluginInfo.NumInputs - 2 << " " << m_SwitchPos << " ";
}

void MixSwitchPlugin::StreamIn (istream &s) {
  int Version, Chans, SwitchPos;
  s >> Version >> Chans >> SwitchPos;
  SetChans (Chans);
  m_SwitchPos = SwitchPos;
}
