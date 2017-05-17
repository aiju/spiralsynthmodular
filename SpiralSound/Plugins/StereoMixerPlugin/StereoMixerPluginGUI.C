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

#include "StereoMixerPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

StereoMixerPluginGUI::StereoMixerPluginGUI (int w, int h, StereoMixerPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch) {
    int Width=20;
    int Height=100;

    for (int n=0; n<NUM_CHANNELS; n++) {

        Numbers[n]=n;

        m_Chan[n] = new Fl_Slider (20 + (2 + Width * 2) * n, 22, Width, Height, "");
        m_Chan[n]->type (4);
        m_Chan[n]->selection_color (Info->GUI_COLOUR);
        m_Chan[n]->box (FL_PLASTIC_DOWN_BOX);
        m_Chan[n]->labelsize (10);
        m_Chan[n]->maximum (2);
        m_Chan[n]->step (0.01);
        m_Chan[n]->value (1.0);
        m_Chan[n]->callback ((Fl_Callback*)cb_Chan, (void*)&Numbers[n]);
        add (m_Chan[n]);

        m_Pan[n] = new Fl_Knob (10 + (2 + Width * 2) * n, 122, 40, 40, "Pan");
        m_Pan[n]->type (Fl_Knob::DOTLIN);
        m_Pan[n]->color (Info->GUI_COLOUR);
        m_Pan[n]->labelsize (10);
        m_Pan[n]->maximum (1);
        m_Pan[n]->step (0.01);
        m_Pan[n]->value (0.5);
        m_Pan[n]->callback ((Fl_Callback*)cb_Pan, (void*)&Numbers[n]);
        add (m_Pan[n]);
    }

    end();
}

void StereoMixerPluginGUI::UpdateValues (SpiralPlugin *o) {
	StereoMixerPlugin* Plugin = (StereoMixerPlugin*)o;

  for (int n=0; n<NUM_CHANNELS; n++) {
    m_Chan[n]->value (2.0f - Plugin->GetChannel (n));
    m_Pan[n]->value (1.0f - Plugin->GetPan (n));
  }
}

inline void StereoMixerPluginGUI::cb_Chan_i(Fl_Slider* o, void* v) {
  m_GUICH->Set("Num",*(int*)(v));
  m_GUICH->Set("Value",(float)(2.0f-o->value()));
  m_GUICH->SetCommand(StereoMixerPlugin::SETCH);
}

void StereoMixerPluginGUI::cb_Chan(Fl_Slider* o, void* v) {
  ((StereoMixerPluginGUI*)(o->parent()))->cb_Chan_i(o,v);
}

inline void StereoMixerPluginGUI::cb_Pan_i(Fl_Knob* o, void* v) {
  m_GUICH->Set("Num",*(int*)(v));
  m_GUICH->Set("Value",(float)(1.0f-o->value()));
  m_GUICH->SetCommand(StereoMixerPlugin::SETPAN);
}

void StereoMixerPluginGUI::cb_Pan(Fl_Knob* o, void* v) {
  ((StereoMixerPluginGUI*)(o->parent()))->cb_Pan_i(o,v);
}

const string StereoMixerPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "A more advanced version of the 4 channel mixer,\n"
    + "this one has a stereo output. CV's are provided\n"
    + "for control of the panning of each of the inputs.\n"
    + "The panning can also be set with the knobs on the\n"
    + "plugin window.\n";
}
