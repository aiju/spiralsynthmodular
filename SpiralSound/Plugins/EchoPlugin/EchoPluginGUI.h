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

#ifndef EchoGUI
#define EchoGUI

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Counter.H>
#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_LED_Button.H"
#include "../SpiralPluginGUI.h"
#include "EchoPlugin.h"

class EchoPluginGUI : public SpiralPluginGUI
{
   public:
      EchoPluginGUI (int w, int h, EchoPlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual void UpdateValues (SpiralPlugin *o);
   protected:
      const std::string GetHelpText (const std::string &loc);
   private:
      Fl_Tabs *m_TheTabs;
      Fl_Group *m_CtlGroup, *m_NumGroup;
      Fl_Knob *m_Delay, *m_Feedback;
      Fl_Counter *m_NumDelay, *m_NumFeedback;
      Fl_LED_Button *m_Bounce;
      //// Callbacks ////
      inline void cb_Delay_i (Fl_Knob* o, void* v);
      static void cb_Delay (Fl_Knob* o, void* v);
      inline void cb_Feedback_i (Fl_Knob* o, void* v);
      static void cb_Feedback (Fl_Knob* o, void* v);
      inline void cb_NumDelay_i (Fl_Counter* o, void* v);
      static void cb_NumDelay (Fl_Counter* o, void* v);
      inline void cb_NumFeedback_i (Fl_Counter* o, void* v);
      static void cb_NumFeedback (Fl_Counter* o, void* v);
      inline void cb_Bounce_i (Fl_LED_Button* o, void* v);
      static void cb_Bounce (Fl_LED_Button* o, void* v);
};

#endif
