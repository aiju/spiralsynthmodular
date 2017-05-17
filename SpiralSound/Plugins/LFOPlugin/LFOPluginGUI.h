/*  SpiralPlugin
 *  Copyleft (C) 2002 Andy Preston <andy@clubunix.co.uk>
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

#ifndef LFO_GUI_H
#define LFO_GUI_H

#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Counter.H>
#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_LED_Button.H"
#include "../SpiralPluginGUI.h"
#include "LFOPlugin.h"

class LFOPluginGUI : public SpiralPluginGUI {
   public:
      LFOPluginGUI (int w, int h, LFOPlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual void UpdateValues (SpiralPlugin *o);
   protected:
      const std::string GetHelpText (const std::string &loc);
   private:
      Fl_Tabs *m_TheTabs;
      Fl_Group *m_CtlGroup, *m_NumGroup;
      Fl_LED_Button *m_ShapeSine, *m_ShapeTri, *m_ShapeSquare, *m_ShapeSaw;
      Fl_Pixmap m_PixmapSine, m_PixmapTri, m_PixmapSquare, m_PixmapSaw;
      Fl_Knob *m_Perd;
      Fl_Counter *m_NumFreq, *m_NumPerd;
      // callbacks
      inline void cb_Perd_i (Fl_Knob *o, void *v);
      static void cb_Perd (Fl_Knob *o, void *v);
      inline void cb_NumFreq_i (Fl_Knob *o, void *v);
      static void cb_NumFreq (Fl_Knob *o, void *v);
      inline void cb_NumPerd_i (Fl_Knob *o, void *v);
      static void cb_NumPerd (Fl_Knob *o, void *v);
      inline void cb_Sine_i (Fl_LED_Button *o, void *v);
      static void cb_Sine (Fl_LED_Button *o, void *v);
      inline void cb_Tri_i (Fl_LED_Button *o, void *v);
      static void cb_Tri (Fl_LED_Button *o, void *v);
      inline void cb_Square_i (Fl_LED_Button *o, void *v);
      static void cb_Square (Fl_LED_Button *o, void *v);
      inline void cb_Saw_i (Fl_LED_Button *o, void *v);
      static void cb_Saw (Fl_LED_Button *o, void *v);
};

#endif
