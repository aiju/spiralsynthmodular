/*  SpiralSound
 *  Copyleft (C) 2003 Andy Preston <andy@clublinux.co.uk>
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

#include "../SpiralPlugin.h"
#include <FL/Fl_Pixmap.H>

#ifndef TransposePLUGIN
#define TransposePLUGIN

class TransposePlugin : public SpiralPlugin {
   public:
      TransposePlugin();
      virtual ~TransposePlugin ();
      virtual PluginInfo& Initialise (const HostInfo *Host);
      virtual SpiralGUIType* CreateGUI ();
      virtual void Execute ();
      //virtual void ExecuteCommands ();
      virtual void StreamOut (std::ostream &s);
      virtual void StreamIn (std::istream &s);
      int GetAmount () { return m_Amount; }
   private:
      int m_Amount;
      float m_Out;
};

#endif