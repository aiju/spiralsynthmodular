/*  WaveShaper Plugin Copyleft (C) 2001 Yves Usson
 *  for SpiralSynthModular
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
#include <FL/Fl_Widget.H>
#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_LED_Button.H"
#include "WaveShaperPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef WaveShaperGUI
#define WaveShaperGUI

class FunctionPlot :  public Fl_Widget {
  public:
    FunctionPlot (int ox, int oy, int ww, int hh);
    ~FunctionPlot ();
  private:
    float *fval;
    void draw ();
    int handle (int event);
    Fl_Color m_IndColour, m_MrkColour, m_FGColour;
  public:
    void SetColours (unsigned i, unsigned m, unsigned f) {
         m_IndColour=(Fl_Color)i; m_MrkColour=(Fl_Color)m; m_FGColour=(Fl_Color)f;
    }
    void set (const int index, const float v);
    float get (const int index) const;
};

class WaveShaperPluginGUI : public SpiralPluginGUI {
  public:
    WaveShaperPluginGUI (int w, int h, WaveShaperPlugin *o, ChannelHandler *ch, const HostInfo *Info);
    virtual void UpdateValues (SpiralPlugin *o);
    virtual void Update ();
  protected:
    const std::string GetHelpText (const std::string &loc);
  private:
    FunctionPlot *fplot;
    Fl_LED_Button *radio_polynomial, *radio_sines;
    Fl_Knob *knob[6];
    //// Callbacks ////
    inline void cb_radio_i (Fl_LED_Button*, void*);
    static void cb_radio (Fl_LED_Button* o, void* v);
    inline void cb_knob_i (Fl_Knob*, void*);
    static void cb_knob (Fl_Knob* o, void* v);
};

#endif
