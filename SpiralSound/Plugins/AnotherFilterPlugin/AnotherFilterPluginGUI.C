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

#include "AnotherFilterPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

AnotherFilterPluginGUI::AnotherFilterPluginGUI(int w, int h,AnotherFilterPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	Cutoff = new Fl_Slider(15, 20, 20, 70, "Cutoff");
	Cutoff->type (FL_VERT_NICE_SLIDER);
        Cutoff->selection_color (Info->GUI_COLOUR);
        Cutoff->box (FL_PLASTIC_DOWN_BOX);
	Cutoff->selection_color (Info->GUI_COLOUR);
        Cutoff->labelsize(10);
	Cutoff->maximum(1);
        Cutoff->step(0.0001);
        Cutoff->value(1);
        Cutoff->callback((Fl_Callback*)cb_Cutoff);

	Resonance = new Fl_Knob(58, 18, 45, 45, "Emphasis");
        Resonance->color(Info->GUI_COLOUR);
	Resonance->type(Fl_Knob::DOTLIN);
        Resonance->labelsize(10);
        Resonance->maximum(1);
        Resonance->step(0.00001);
        Resonance->value(0);
	Resonance->callback((Fl_Callback*)cb_Resonance);

	end();
}

void AnotherFilterPluginGUI::UpdateValues(SpiralPlugin *o)
{
	AnotherFilterPlugin *Plugin = (AnotherFilterPlugin*)o;
	// Cutoff->value(100.0f-sqrt(Plugin->GetCutoff()-10.0f));
	Cutoff->value (1.0f - Plugin->GetCutoff());
	Resonance->value(Plugin->GetResonance()/*-1.0f*/);
}

inline void AnotherFilterPluginGUI::cb_Cutoff_i(Fl_Slider* o, void* v)
{
	float value=1.0f-o->value();
	m_GUICH->Set("Cutoff",value);
}

void AnotherFilterPluginGUI::cb_Cutoff(Fl_Slider* o, void* v)
{ ((AnotherFilterPluginGUI*)(o->parent()))->cb_Cutoff_i(o,v); }

inline void AnotherFilterPluginGUI::cb_Resonance_i(Fl_Knob* o, void* v)
{ m_GUICH->Set("Resonance",(float)o->value()); }

void AnotherFilterPluginGUI::cb_Resonance(Fl_Knob* o, void* v)
{ ((AnotherFilterPluginGUI*)(o->parent()))->cb_Resonance_i(o,v); }

const string AnotherFilterPluginGUI::GetHelpText(const string &loc){
    return string("")
	 + "Resonant IIR lowpass (12dB/oct) filter\n";
}
