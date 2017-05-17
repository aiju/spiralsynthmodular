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
#include "DistributorPlugin.h"
#include "DistributorPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {

SpiralPlugin* SpiralPlugin_CreateInstance() {
      return new DistributorPlugin;
}

char** SpiralPlugin_GetIcon() {
       return SpiralIcon_xpm;
}

int SpiralPlugin_GetID() {
    return 0x0056;
}

string SpiralPlugin_GetGroupName() {
       return "Control";
}

}

///////////////////////////////////////////////////////

DistributorPlugin::DistributorPlugin() :
m_Triggered (false),
m_ChannelSelect (0),
m_TrigDelay (0)
{
	m_PluginInfo.Name = "Distributor";
	m_PluginInfo.Width = 220;
	m_PluginInfo.Height = 250;
	m_PluginInfo.NumInputs = 2;
	m_PluginInfo.NumOutputs = 8;
	m_PluginInfo.PortTips.push_back ("Stream");
	m_PluginInfo.PortTips.push_back ("Switcher");
	m_PluginInfo.PortTips.push_back ("Stream 1");
	m_PluginInfo.PortTips.push_back ("Switcher 1");
	m_PluginInfo.PortTips.push_back ("Stream 2");
	m_PluginInfo.PortTips.push_back ("Switcher 2");
	m_PluginInfo.PortTips.push_back ("Stream 3");
	m_PluginInfo.PortTips.push_back ("Switcher 3");
	m_PluginInfo.PortTips.push_back ("Stream 4");
	m_PluginInfo.PortTips.push_back ("Switcher 4");
}

DistributorPlugin::~DistributorPlugin() {
}

PluginInfo &DistributorPlugin::Initialise (const HostInfo *Host) {
           return SpiralPlugin::Initialise (Host);
}

SpiralGUIType *DistributorPlugin::CreateGUI() {
              return NULL;
}

void DistributorPlugin::Execute() {
     const int Stream = 0;
     const int Switch = 1;
     for (int n=0; n<m_HostInfo->BUFSIZE; n++) {
         float InpStream = InputExists (Stream) ? GetInput (Stream, n) : 0.0;
         float InpSwitch = InputExists (Switch) ? GetInput (Switch, n) : 0.0;
         if (InpSwitch <= 0.0) m_Triggered = false;
         if (! m_Triggered && (InpSwitch > 0.0)) {
            m_Triggered = true;
            SetOutput (m_ChannelSelect+Switch, n, 0);
            m_ChannelSelect = m_ChannelSelect + 2;
            if (m_ChannelSelect > 6) m_ChannelSelect = 0;
            m_TrigDelay = 0;
         }
         SetOutput (m_ChannelSelect+Stream, n, InpStream);
         if (m_TrigDelay < 10) {
            m_TrigDelay++;
            SetOutput (m_ChannelSelect+Switch, n, InpSwitch);
         }
         else SetOutput (m_ChannelSelect+Switch, n, 0);
     }
}
