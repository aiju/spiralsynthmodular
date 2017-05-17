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

#ifndef StreamGUI
#define StreamGUI

#include <FL/Fl.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Button.H>
#include "StreamPlugin.h"
#include "../SpiralPluginGUI.h"
#include "../Widgets/Fl_SevenSeg.H"
#include "../Widgets/Fl_Knob.H"

class StreamPluginGUI : public SpiralPluginGUI {
   public:
      StreamPluginGUI(int w, int h, StreamPlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual ~StreamPluginGUI ();
      virtual void Update ();
      virtual void UpdateValues (SpiralPlugin *o);
   protected:
      const std::string GetHelpText (const std::string &loc);
   private:
      bool m_Playing;
      float m_PitchValue;
      char m_TextBuf[256], m_PitchLabel[256];
      void SetMaxTime (float t) { m_Pos->maximum(t); }
      void UpdatePitch (bool UpdateIt = true, bool DrawIt = true, bool SendIt = true);
      void UpdatePlayStatus (void);
      // Widgets
      Fl_SevenSeg *m_Display[6];
      Fl_Knob *m_Volume;
      Fl_Slider *m_Pitch, *m_Pos;
      Fl_Button *m_Load, *m_ToStart, *m_Play, *m_Reset, *m_Rev, *m_Div, *m_Dbl;
      Fl_Repeat_Button* m_Nudge;
      //// Callbacks ////
      inline void cb_Volume_i (Fl_Knob* o, void* v);
      static void cb_Volume (Fl_Knob* o, void* v);
      inline void cb_Pitch_i (Fl_Slider* o, void* v);
      static void cb_Pitch (Fl_Slider* o, void* v);
      inline void cb_Pos_i (Fl_Slider* o, void* v);
      static void cb_Pos (Fl_Slider* o, void* v);
      inline void cb_Load_i (Fl_Button* o, void* v);
      static void cb_Load (Fl_Button* o, void* v);
      inline void cb_ToStart_i (Fl_Button* o, void* v);
      static void cb_ToStart (Fl_Button* o, void* v);
      inline void cb_Play_i (Fl_Button* o, void* v);
      static void cb_Play (Fl_Button* o, void* v);
      inline void cb_Reset_i (Fl_Button* o, void* v);
      static void cb_Reset (Fl_Button* o, void* v);
      inline void cb_Rev_i (Fl_Button* o, void* v);
      static void cb_Rev (Fl_Button* o, void* v);
      inline void cb_Div_i (Fl_Button* o, void* v);
      static void cb_Div (Fl_Button* o, void* v);
      inline void cb_Dbl_i (Fl_Button* o, void* v);
      static void cb_Dbl (Fl_Button* o, void* v);
      inline void cb_Nudge_i (Fl_Button* o, void* v);
      static void cb_Nudge (Fl_Button* o, void* v);
};

#endif
