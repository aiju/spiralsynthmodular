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
#include <FL/Fl_Slider.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Counter.H>
#include "EnvelopePlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef EnvelopeGUI
#define EnvelopeGUI

class EnvelopePluginGUI : public SpiralPluginGUI {
   public:
      EnvelopePluginGUI(int w, int h, EnvelopePlugin *o,ChannelHandler *ch,const HostInfo *Info);
      virtual void UpdateValues(SpiralPlugin *o);
   protected:
      const std::string GetHelpText(const std::string &loc);
   private:
      Fl_Tabs *m_Tabs;
      Fl_Group *m_CtlGroup, *m_NumGroup;
      Fl_Slider *m_Thresh, *m_Attack, *m_Decay, *m_Sustain, *m_Release, *m_Volume;
      Fl_Counter *m_NumThresh, *m_NumAttack, *m_NumDecay, *m_NumSustain, *m_NumRelease, *m_NumVolume;
      // Slider Callbacks
      inline void cb_Thresh_i (Fl_Slider* o, void* v);
      static void cb_Thresh (Fl_Slider* o, void* v);
      inline void cb_Attack_i (Fl_Slider* o, void* v);
      static void cb_Attack (Fl_Slider* o, void* v);
      inline void cb_Decay_i (Fl_Slider* o, void* v);
      static void cb_Decay (Fl_Slider* o, void* v);
      inline void cb_Sustain_i (Fl_Slider* o, void* v);
      static void cb_Sustain (Fl_Slider* o, void* v);
      inline void cb_Release_i (Fl_Slider* o, void* v);
      static void cb_Release (Fl_Slider* o, void* v);
      inline void cb_Volume_i (Fl_Slider* o, void* v);
      static void cb_Volume (Fl_Slider* o, void* v);
      // Counter Callbacks
      inline void cb_NumThresh_i (Fl_Counter* o, void* v);
      static void cb_NumThresh (Fl_Counter* o, void* v);
      inline void cb_NumAttack_i (Fl_Counter* o, void* v);
      static void cb_NumAttack (Fl_Counter* o, void* v);
      inline void cb_NumDecay_i (Fl_Counter* o, void* v);
      static void cb_NumDecay (Fl_Counter* o, void* v);
      inline void cb_NumSustain_i (Fl_Counter* o, void* v);
      static void cb_NumSustain (Fl_Counter* o, void* v);
      inline void cb_NumRelease_i (Fl_Counter* o, void* v);
      static void cb_NumRelease (Fl_Counter* o, void* v);
      inline void cb_NumVolume_i (Fl_Counter* o, void* v);
      static void cb_NumVolume (Fl_Counter* o, void* v);
};

#endif
