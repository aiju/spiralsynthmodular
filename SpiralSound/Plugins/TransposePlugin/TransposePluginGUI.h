/*  SpiralPlugin
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

#include <FL/Fl.H>
#include <FL/Fl_Counter.H>
#include "TransposePlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef TransposeGUI
#define TransposeGUI

class TransposePluginGUI : public SpiralPluginGUI {
   public:
      TransposePluginGUI (int w, int h, TransposePlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual void UpdateValues (SpiralPlugin *o);
   protected:
      const std::string GetHelpText (const std::string &loc);
   private:
      Fl_Counter* m_Amount;
      inline void cb_Amount_i (Fl_Counter* o, void* v);
      static void cb_Amount (Fl_Counter* o, void* v);
};

#endif
