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
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Counter.H>
#include "../SpiralPluginGUI.h"
#include "AmpPlugin.h"

#ifndef AmpGUI
#define AmpGUI

class AmpPluginGUI : public SpiralPluginGUI {
   public:
      AmpPluginGUI(int w, int h, AmpPlugin *o,ChannelHandler *ch,const HostInfo *Info);
      virtual void UpdateValues(SpiralPlugin *o);
   protected:
      const std::string GetHelpText(const std::string &loc);
   private:
      Fl_Tabs *m_TheTabs;
      Fl_Group *m_CtlGroup, *m_NumGroup;
      Fl_Slider *m_Gain, *m_DC;
      Fl_Counter *m_NumGain, *m_NumDC;
      Fl_Button *m_Reset;
      //// Callbacks ////
      inline void cb_Gain_i (Fl_Slider* o, void* v);
      static void cb_Gain (Fl_Slider* o, void* v);
      inline void cb_DC_i (Fl_Slider* o, void* v);
      static void cb_DC (Fl_Slider* o, void* v);
      inline void cb_NumGain_i (Fl_Counter* o, void* v);
      static void cb_NumGain (Fl_Counter* o, void* v);
      inline void cb_NumDC_i (Fl_Counter* o, void* v);
      static void cb_NumDC (Fl_Counter* o, void* v);
      inline void cb_Reset_i (Fl_Button* o, void* v);
      static void cb_Reset (Fl_Button* o, void* v);
};

#endif
