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
#include "SplitSwitchPlugin.h"
#include "SplitSwitchPluginGUI.h"
#include <FL/Fl_Button.h>
#include <stdio.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {

   SpiralPlugin* SpiralPlugin_CreateInstance () { return new SplitSwitchPlugin; }

   char** SpiralPlugin_GetIcon () { return SpiralIcon_xpm; }

   int SpiralPlugin_GetID () { return 125; }

   string SpiralPlugin_GetGroupName() { return "Maths/Logic"; }
}

///////////////////////////////////////////////////////

SplitSwitchPlugin::SplitSwitchPlugin () :
m_SwitchPos (0),
m_Triggered (false)
{
  m_GUIArgs.Chans = 2;
  m_GUIArgs.Switch = 1;
  m_GUIArgs.Echo = 1;
  m_PluginInfo.Name = "SplitSwitch";
  m_PluginInfo.Width = 80;
  m_PluginInfo.Height = 80;
  m_PluginInfo.NumInputs = 3;
  m_PluginInfo.NumOutputs = 3;
  m_PluginInfo.PortTips.push_back ("CV");
  m_PluginInfo.PortTips.push_back ("Clock");
  m_PluginInfo.PortTips.push_back ("In");
  m_PluginInfo.PortTips.push_back ("CV");
  m_PluginInfo.PortTips.push_back ("Out 1");
  m_PluginInfo.PortTips.push_back ("Out 2");
  // Number of channels <- GUI
  m_AudioCH->Register ("Chans", &m_GUIArgs.Chans);
  // Switch Position <- GUI
  m_AudioCH->Register ("Switch", &m_GUIArgs.Switch);
  // Switch Position -> GUI
  m_AudioCH->Register ("Echo", &m_GUIArgs.Echo, ChannelHandler::OUTPUT);
  // Auto Mode (Switch position is controlled by CV or Clock = True, Switch Position controlled by GUI = False
  m_AudioCH->Register ("Auto", &m_GUIArgs.Auto, ChannelHandler::OUTPUT);
}

SplitSwitchPlugin::~SplitSwitchPlugin () {
}

PluginInfo &SplitSwitchPlugin::Initialise (const HostInfo *Host) {
  return SpiralPlugin::Initialise (Host);
}

SpiralGUIType *SplitSwitchPlugin::CreateGUI () {
  return new SplitSwitchPluginGUI (m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void SplitSwitchPlugin::ExecuteCommands () {
  if (m_AudioCH->IsCommandWaiting ()) {
    switch (m_AudioCH->GetCommand()) {
      case (SETCHANS)  : SetChans (m_GUIArgs.Chans);
                         break;
    }
  }
}

void SplitSwitchPlugin::SetChans (int n) {
  // once to clear the connections with the current info
  UpdatePluginInfoWithHost();
  while (n > m_PluginInfo.NumOutputs - 1) {
    m_PluginInfo.NumOutputs++;
    char t[256];
    sprintf (t, "Out %d", n);
    m_PluginInfo.PortTips.push_back (t);
    AddOutput ();
  }
  while (n < m_PluginInfo.NumOutputs - 1) {
    vector<string>::iterator i = m_PluginInfo.PortTips.end ();
    m_PluginInfo.PortTips.erase (i--);
    RemoveOutput();
    m_PluginInfo.NumOutputs--;
  }
  // do the actual update
  UpdatePluginInfoWithHost ();
}

void SplitSwitchPlugin::Execute() {
  int n;
  int NumChans = m_PluginInfo.NumOutputs - 1;
  for (n=1; n<m_PluginInfo.NumOutputs; n++) GetOutputBuf(n)->Zero();
  if (InputExists (2)) {
    for (n=0; n<m_HostInfo->BUFSIZE; n++) {
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
            m_SwitchPos = (m_SwitchPos+1);
          }
        }
      }
      else {
        m_GUIArgs.Auto = false;
        m_SwitchPos=(m_GUIArgs.Switch);
      }
      if (m_SwitchPos > NumChans) m_SwitchPos = 1;
      m_GUIArgs.Echo = m_SwitchPos;
      SetOutput (0, n, m_SwitchPos);
      SetOutput (m_SwitchPos, n, GetInput (2, n));
    }
  }
}

void SplitSwitchPlugin::StreamOut (ostream &s) {
  s << m_Version << " " << m_PluginInfo.NumOutputs - 1 << " " << m_SwitchPos << " ";
}

void SplitSwitchPlugin::StreamIn (istream &s) {
  int Version, Chans, SwitchPos;
  s >> Version >> Chans >> SwitchPos;
  SetChans (Chans);
  m_SwitchPos = SwitchPos;
}
