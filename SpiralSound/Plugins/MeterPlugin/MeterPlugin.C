/*  SpiralSound
 *  Copyleft (C) 2002 Andy Preston <andy@clublinux.co.uk>
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
#include "MeterPlugin.h"
#include "MeterPluginGUI.h"
#include "SpiralIcon.xpm"
#include <stdio.h>

using namespace std;

extern "C" {

SpiralPlugin* SpiralPlugin_CreateInstance() { return new MeterPlugin; }

char** SpiralPlugin_GetIcon() { return SpiralIcon_xpm; }

int SpiralPlugin_GetID() { return 123; }

string SpiralPlugin_GetGroupName() { return "Control"; }

}

MeterPlugin::MeterPlugin():
m_Data (NULL),
m_DataReady (false),
m_VUMode (true)
{
  m_PluginInfo.Name = "Meter";
  m_PluginInfo.Width = 230;
  m_PluginInfo.Height = 128;
  m_PluginInfo.NumInputs = 1;
  m_PluginInfo.NumOutputs = 1;
  m_PluginInfo.PortTips.push_back ("Input");
  m_PluginInfo.PortTips.push_back ("Output");
  m_AudioCH->Register ("DataReady", &m_DataReady, ChannelHandler::OUTPUT);
  m_Version = 1;
}

MeterPlugin::~MeterPlugin() {
  if (m_Data != NULL) delete m_Data;
}

PluginInfo &MeterPlugin::Initialise (const HostInfo *Host) {
  PluginInfo& Info = SpiralPlugin::Initialise (Host);
  m_Data = new float[Host->BUFSIZE];
  m_AudioCH->RegisterData ("AudioData", ChannelHandler::OUTPUT, m_Data, Host->BUFSIZE * sizeof (float));
  return Info;
}

SpiralGUIType *MeterPlugin::CreateGUI() {
  return new MeterPluginGUI (m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void MeterPlugin::Execute() {
     // Just copy the data through.
     m_DataReady = InputExists (0);
     if (GetOutputBuf (0)) GetOutputBuf (0)->Zero();
     if (m_DataReady) {
        GetOutputBuf (0)->Mix (*GetInput(0), 0);
        memcpy (m_Data, GetInput (0)->GetBuffer (), m_HostInfo->BUFSIZE * sizeof (float));
     }
}

void MeterPlugin::ExecuteCommands () {
  if (m_AudioCH->IsCommandWaiting ()) {
    switch (m_AudioCH->GetCommand()) {
      case (SETVU) : m_VUMode = true;
                     break;
      case (SETMM) : m_VUMode = false;
                     break;
    }
  }
}

void MeterPlugin::StreamOut (ostream &s) {
  s << m_Version << " " << m_VUMode << " ";
}

void MeterPlugin::StreamIn (istream &s) {
  int Version;
  s >> Version;
  s >> m_VUMode;
}
