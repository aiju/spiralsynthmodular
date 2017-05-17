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

#ifndef OSC_GUI
#define OSC_GUI

#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Tabs.h>
#include <FL/Fl_Group.h>
#include <FL/Fl_Button.h>
#include "../Widgets/Fl_LED_Button.H"
#include "../Widgets/Fl_Knob.H"
#include "../SpiralPluginGUI.h"
#include "OscillatorPlugin.h"

class OscillatorPluginGUI : public SpiralPluginGUI {
   public:
      OscillatorPluginGUI (int w, int h, OscillatorPlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual void UpdateValues (SpiralPlugin *o);
   protected:
      const std::string GetHelpText(const std::string &loc);
   private:
      // Utility Functions
      void UpdateFreq (void);
      float CalcFineFreq (float Fine);
      // Widgets
      Fl_Tabs *m_Tabs;
      Fl_Group *m_CtlGroup, *m_NumGroup;
      Fl_Button *m_Reset;
      Fl_LED_Button *m_ShapeSquare, *m_ShapeNoise, *m_ShapeSaw;
      Fl_Pixmap m_PixmapSquare, m_PixmapNoise, m_PixmapSaw;
      Fl_Knob *m_Octave, *m_ModAmount, *m_FineTune;
      Fl_Slider *m_PulseWidth, *m_SHLen;
      Fl_Counter *m_NumFreq, *m_NumModAmount, *m_NumPulseWidth, *m_NumSHLen, *m_NumOctave;
      // Data
      float m_FineFreq;
      // Callbacks - no group
      inline void cb_Square_i (Fl_LED_Button* o, void* v);
      static void cb_Square (Fl_LED_Button* o, void* v);
      inline void cb_Saw_i (Fl_LED_Button* o, void* v);
      static void cb_Saw (Fl_LED_Button* o, void* v);
      inline void cb_Noise_i (Fl_LED_Button* o, void* v);
      static void cb_Noise (Fl_LED_Button* o, void* v);
      // Callbacks - Control Group
      inline void cb_Octave_i (Fl_Knob* o, void* v);
      static void cb_Octave (Fl_Knob* o, void* v);
      inline void cb_FineTune_i (Fl_Knob* o, void* v);
      static void cb_FineTune (Fl_Knob* o, void* v);
      inline void cb_Reset_i (Fl_Button* o, void* v);
      static void cb_Reset (Fl_Button* o, void* v);
      inline void cb_ModAmount_i (Fl_Knob* o, void* v);
      static void cb_ModAmount (Fl_Knob* o, void* v);
      inline void cb_PulseWidth_i (Fl_Slider* o, void* v);
      static void cb_PulseWidth (Fl_Slider* o, void* v);
      inline void cb_SHLen_i (Fl_Slider* o, void* v);
      static void cb_SHLen (Fl_Slider* o, void* v);
      // Callbacks - Numbers Group
      inline void cb_NumPulseWidth_i (Fl_Counter* o, void* v);
      static void cb_NumPulseWidth (Fl_Counter* o, void* v);
      inline void cb_NumFreq_i (Fl_Counter* o, void* v);
      static void cb_NumFreq (Fl_Counter* o, void* v);
      inline void cb_NumOctave_i (Fl_Counter* o, void* v);
      static void cb_NumOctave (Fl_Counter* o, void* v);
      inline void cb_NumModAmount_i (Fl_Counter* o, void* v);
      static void cb_NumModAmount (Fl_Counter* o, void* v);
      inline void cb_NumSHLen_i (Fl_Counter* o, void* v);
      static void cb_NumSHLen (Fl_Counter* o, void* v);
};

#endif
