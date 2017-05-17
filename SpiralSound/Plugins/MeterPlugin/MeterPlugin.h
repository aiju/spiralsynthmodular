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

#ifndef METERPLUGIN
#define METERPLUGIN

#include "../SpiralPlugin.h"

class MeterPlugin : public SpiralPlugin {
  public:
    MeterPlugin();
    virtual ~MeterPlugin();
    virtual PluginInfo& Initialise (const HostInfo *Host);
    virtual SpiralGUIType* CreateGUI();
    virtual void Execute();
    virtual void ExecuteCommands();
    virtual void StreamOut (std::ostream &s);
    virtual void StreamIn (std::istream &s);
    int GetVUMode (void) { return m_VUMode; }
    enum GUICommands {NONE, SETVU, SETMM};
  private:
    float *m_Data;
    // m_VUMode isn't USED for anything, it's here so we can save/load it
    bool m_DataReady, m_VUMode;
};

#endif
