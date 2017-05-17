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

#include <stdio.h>
#include "MixerPlugin.h"
#include "MixerPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {

    SpiralPlugin* SpiralPlugin_CreateInstance() {
        return new MixerPlugin;
    }

    char** SpiralPlugin_GetIcon() {
        return SpiralIcon_xpm;
    }

    int SpiralPlugin_GetID() {
        return 0x0007;
    }

    string SpiralPlugin_GetGroupName() {
        return "Amps/Mixers";
    }

}

///////////////////////////////////////////////////////

MixerPlugin::MixerPlugin() :
m_NumChannels(0)
{
        m_Version = 2;
        m_PluginInfo.Name = "Mixer";
	m_PluginInfo.Width = 80;
	m_PluginInfo.Height = 140;
        m_GUIArgs.Peak = false;
        CreatePorts ();
	for (int n=0; n<MAX_CHANNELS; n++) m_ChannelVal[n] = 1.0f;
	m_AudioCH->Register ("Value", &m_GUIArgs.Value);
	m_AudioCH->Register ("Num", &m_GUIArgs.Num);
	m_AudioCH->Register ("Peak", &m_GUIArgs.Peak, ChannelHandler::OUTPUT);
}

MixerPlugin::~MixerPlugin() {
}

PluginInfo &MixerPlugin::Initialise (const HostInfo *Host) {
    return SpiralPlugin::Initialise (Host);
}

SpiralGUIType *MixerPlugin::CreateGUI() {
    return new MixerPluginGUI (m_PluginInfo.Width, m_PluginInfo.Height, this, m_AudioCH, m_HostInfo);
}

void MixerPlugin::Execute () {
     // Mix the inputs
     for (int n=0; n<m_HostInfo->BUFSIZE; n++) {
         float out = 0.0;
         for (int c=0; c<m_NumChannels; c++)
             out += (GetInput (c, n) * m_ChannelVal[c]);
         SetOutput (0, n, out);
         m_GUIArgs.Peak = (out > 1.0);
     }
}

void MixerPlugin::ExecuteCommands() {
     if (m_AudioCH->IsCommandWaiting()) {
        switch (m_AudioCH->GetCommand()) {
          case SETCH:
               m_ChannelVal[m_GUIArgs.Num] = m_GUIArgs.Value;
               break;
          case SETNUM:
               SetChannels (m_GUIArgs.Num);
               break;
        }
      }
}

void MixerPlugin::SetChannels (int n) {
     // once to clear the connections with the current info
     // do we need this????
     UpdatePluginInfoWithHost();
     // Things can get a bit confused deleting and adding inputs so we just chuck away all the ports...
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

void MixerPlugin::CreatePorts (int n, bool AddPorts) {
     // default values   n = 4    AddPorts = false
     int c;
     m_PluginInfo.NumInputs = n;
     m_NumChannels = n;
     char t[256];
     for (c=1; c<=n; c++) {
         sprintf (t, "Input %d", c);
         m_PluginInfo.PortTips.push_back (t);
     }
     m_PluginInfo.NumOutputs = 1;
     m_PluginInfo.PortTips.push_back ("Output");
     if (AddPorts) {
        for (c=0; c<m_PluginInfo.NumInputs; c++) AddInput();
        AddOutput();
     }
}

void MixerPlugin::StreamOut (ostream &s) {
     s << m_Version << " ";
     s << m_NumChannels << " ";
     for (int n=0; n<m_NumChannels; n++) s << m_ChannelVal[n] << " ";
}

void MixerPlugin::StreamIn (istream &s) {
     int version, chans;
     s >> version;
     switch (version) {
       case 1: SetChannels (4);
               break;
       case 2: s >> chans;
               SetChannels (chans);
               break;
     }
     for (int n=0; n<m_NumChannels; n++) s >> m_ChannelVal[n];
}
