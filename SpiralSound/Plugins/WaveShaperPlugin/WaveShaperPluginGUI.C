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

#include "WaveShaperPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>

using namespace std;

FunctionPlot::FunctionPlot(int ox, int oy, int ww, int hh) :
Fl_Widget (ox, oy, ww, hh),
m_IndColour(FL_YELLOW),
m_MrkColour(FL_BLUE),
m_FGColour(FL_GREEN)
{
  fval = new float[256];
  for (int i=0;i<256;i++) fval[i] = i / 128.0 - 1.0;
}

FunctionPlot::~FunctionPlot() {
  delete [] fval;
}

void FunctionPlot::draw() {
        int ox = x() + 3;
        int oy = y() +3;
        int ww = w() - 6;
        int hh = h() - 6;
        float coefx,coefy;

        draw_box();
        fl_clip(ox,oy,ww,hh);
	coefx = ww / 256.0;
	coefy = hh / 2.0;
	for (int i = -5; i <= 5; i++)
	{
		if (i==0) fl_color (m_IndColour);
                else fl_color (m_MrkColour);
		fl_line(ox,oy+hh/2+i*hh/10,ox+ww,oy+hh/2+i*hh/10);
	}
	for (int i = -5; i <= 5; i++)
	{
		if (i==0) fl_color (m_IndColour);
                else fl_color (m_MrkColour);
		fl_line(ox+i*ww/10+ww/2,oy,ox+i*ww/10+ww/2,oy+hh);
	}
	fl_color (m_FGColour);
	float y1 = oy+hh/2-coefy*fval[0];
	for (int i = 0; i < 255; i++)
	{
		float y2 = oy+hh/2-coefy*fval[i+1];
		fl_line((int)(ox+i*coefx),(int)y1,(int)(ox+(i+1)*coefx),(int)y2);
		y1 = y2;
	}
	fl_color (FL_BLACK);
	fl_pop_clip();
}

int FunctionPlot::handle(int event) {
  return 0;
}

void FunctionPlot::set(const int index, const float v) {
  if ((index < 0) || (index > 255)) return;
	fval[index] = v;
	if (fval[index] > 1.0f) fval[index] = 1.0f;
	if (fval[index] < -1.0f) fval[index] = -1.0f;
}

float FunctionPlot::get(const int index) const {
  if ((index < 0) || (index > 255)) return (-10.0f);
	return fval[index];
}


////////////////////////////////////////////

WaveShaperPluginGUI::WaveShaperPluginGUI (int w, int h, WaveShaperPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI(w, h, o, ch)
{
  fplot = new FunctionPlot(3, 20, 268, 195);
  fplot->box (FL_UP_BOX);
  fplot->color (Info->SCOPE_BG_COLOUR);
  fplot->SetColours (Info->SCOPE_IND_COLOUR, Info->SCOPE_MRK_COLOUR, Info->SCOPE_FG_COLOUR);

  radio_polynomial = new Fl_LED_Button(3, 218, 30, 20, "S");
  radio_polynomial->down_box(FL_ROUND_DOWN_BOX);
  radio_polynomial->value(1);
  radio_polynomial->type(102);
  radio_polynomial->labelfont(12);
  radio_polynomial->labelsize(12);
  radio_polynomial->callback((Fl_Callback*)cb_radio);

  radio_sines = new Fl_LED_Button(3, 238, 30, 20, "S");
  radio_sines->down_box(FL_ROUND_DOWN_BOX);
  radio_sines->value(0);
  radio_sines->type(102);
  radio_sines->labelfont(12);
  radio_sines->labelsize(12);
  radio_sines->callback((Fl_Callback*)cb_radio);

  { Fl_Box* o = new Fl_Box(26, 218, 25, 20, "a .x");
    o->labelfont(2);
    o->labelsize(10);
  }
  { Fl_Box* o = new Fl_Box(33, 227, 10, 15, "i");
    o->labelfont(2);
    o->labelsize(10);
  }
  { Fl_Box* o = new Fl_Box(47, 217, 10, 15, "i");
    o->labelfont(2);
    o->labelsize(10);
  }
  { Fl_Box* o = new Fl_Box(31, 246, 10, 15, "i");
    o->labelfont(2);
    o->labelsize(10);
  }
  { Fl_Box* o = new Fl_Box(36, 237, 25, 20, "a .sin i x");
    o->labelfont(2);
    o->labelsize(10);
  }

  for (int i=0; i<6; i++) {
    knob[i] = new Fl_Knob (70+i*35, 220, 30, 30, "");
    knob[i]->color (Info->GUI_COLOUR);
    knob[i]->type (Fl_Knob::DOTLIN);
    knob[i]->labelsize (10);
    knob[i]->maximum (1);
    knob[i]->minimum (-1);
    knob[i]->step (0.01);
    knob[i]->value (0);
    knob[i]->callback ((Fl_Callback*)cb_knob);
  }
  knob[0]->value(1.0);
  end();
}

void WaveShaperPluginGUI::UpdateValues (SpiralPlugin *o) {
  WaveShaperPlugin* Plugin = (WaveShaperPlugin*) o;
  int wt = Plugin->GetWaveType ();
  radio_polynomial->value (wt);
  radio_sines->value (!wt);
  for (int j=0; j<6; j++) knob[j]->value (Plugin->GetCoef (j));
  Update ();
}

void WaveShaperPluginGUI::Update () {
  // Formerly KnobCB
  float WTData[256];
  m_GUICH->GetData ("WT", (void*)WTData);
  for (int i=0; i<256; i++) fplot->set (i, WTData[i]);
  fplot->redraw ();
}

inline void WaveShaperPluginGUI::cb_radio_i (Fl_LED_Button*, void*) {
  m_GUICH->Set ("WaveType", (int)radio_polynomial->value ());
  m_GUICH->SetCommand (WaveShaperPlugin::SETWAVETYPE);
}

void WaveShaperPluginGUI::cb_radio (Fl_LED_Button* o, void* v) {
  ((WaveShaperPluginGUI*)(o->parent()))->cb_radio_i (o, v);
}

inline void WaveShaperPluginGUI::cb_knob_i (Fl_Knob* o, void* v) {
  for (int j=0; j<6; j++) {
    if (o==knob[j]) {
      m_GUICH->Set ("CoefNum", j);
      m_GUICH->Set ("CoefVal", (float)knob[j]->value ());
      m_GUICH->SetCommand (WaveShaperPlugin::SETCOEF);
      break;
    }
  }
}
void WaveShaperPluginGUI::cb_knob (Fl_Knob* o, void* v) {
  ((WaveShaperPluginGUI*)(o->parent()))->cb_knob_i (o, v);
}

const string WaveShaperPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "This plugin simulates wave shaping as found on analogue\n"
    + "synths, usable in two modes, linear or sine based.\n";
}
