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

#ifndef MIXER_PLUGIN_H
#define MIXER_PLUGIN_H

#include "../SpiralPlugin.h"

static const int MAX_CHANNELS = 16;

class MixerPlugin : public SpiralPlugin {
  public:
      MixerPlugin();
      virtual ~MixerPlugin();
      virtual PluginInfo &Initialise(const HostInfo *Host);
      virtual SpiralGUIType *CreateGUI();
      virtual void Execute();
      virtual void ExecuteCommands();
      virtual void StreamOut(std::ostream &s);
      virtual void StreamIn(std::istream &s);
      // has to be defined in the plugin
      virtual void UpdateGUI() { Fl::check(); }
      enum GUICommands { NONE, SETCH, SETNUM };
      struct GUIArgs {
	     int Num;
	     float Value;
             bool Peak;
      };
      float GetChannel (int n) { return m_ChannelVal[n]; }
      int GetChannels (void) { return m_NumChannels; }
  private:
      void CreatePorts (int n = 4, bool AddPorts = false);
      GUIArgs m_GUIArgs;
      int m_NumChannels;
      void SetChannels (int n);
      float m_ChannelVal[MAX_CHANNELS];
};

#endif
