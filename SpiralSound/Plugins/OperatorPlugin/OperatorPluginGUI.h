/*  SpiralPlugin
 *  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
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
#include <FL/Fl_Button.H>
#include <FL/Fl_Counter.H>
#include "OperatorPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef OpPluginGUI
#define OpPluginGUI

class OperatorPluginGUI : public SpiralPluginGUI {
   public:
      OperatorPluginGUI (int w, int h, OperatorPlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual void UpdateValues(SpiralPlugin *o);
   protected:
      const std::string GetHelpText(const std::string &loc);
   private:
      Fl_Button *m_Add, *m_Sub, *m_Mul, *m_Div;
      Fl_Counter *m_Constant;
      //// Callbacks ////
      inline void cb_Add_i (Fl_Button* o, void* v);
      static void cb_Add (Fl_Button* o, void* v);
      inline void cb_Sub_i (Fl_Button* o, void* v);
      static void cb_Sub (Fl_Button* o, void* v);
      inline void cb_Mul_i (Fl_Button* o, void* v);
      static void cb_Mul (Fl_Button* o, void* v);
      inline void cb_Div_i (Fl_Button* o, void* v);
      static void cb_Div (Fl_Button* o, void* v);
      inline void cb_Constant_i (Fl_Counter* o, void* v);
      static void cb_Constant (Fl_Counter* o, void* v);
};

#endif
