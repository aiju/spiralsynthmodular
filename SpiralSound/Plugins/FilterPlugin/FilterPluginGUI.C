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

#include "FilterPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

FilterPluginGUI::FilterPluginGUI(int w, int h,FilterPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	Cutoff = new Fl_Slider(15, 20, 20, 70, "Cutoff");
	Cutoff->type (FL_VERT_NICE_SLIDER);
        Cutoff->selection_color (Info->GUI_COLOUR);
        Cutoff->box (FL_PLASTIC_DOWN_BOX);
	Cutoff->selection_color (Info->GUI_COLOUR);
        Cutoff->labelsize(10);
	Cutoff->maximum(100);
        Cutoff->step(0.01);
        Cutoff->value(90.51);
        Cutoff->callback((Fl_Callback*)cb_Cutoff);

	Resonance = new Fl_Knob(58, 18, 45, 45, "");
    Resonance->color(Info->GUI_COLOUR);
	Resonance->type(Fl_Knob::DOTLIN);
    Resonance->labelsize(10);
    Resonance->maximum(10);
    Resonance->step(0.1);
    Resonance->value(0);
	Resonance->callback((Fl_Callback*)cb_Resonance);

	RevCutoff = new Fl_Button(57, 68, 50, 15, "RvCMod");
        RevCutoff->type (FL_TOGGLE_BUTTON);
        RevCutoff->box (FL_PLASTIC_UP_BOX);
        RevCutoff->color (Info->GUI_COLOUR);
        RevCutoff->selection_color (Info->GUI_COLOUR);
        RevCutoff->labelsize(10);
        RevCutoff->callback((Fl_Callback*)cb_RevCutoff);

	RevResonance = new Fl_Button(57, 86, 50, 15, "RvRMod");
        RevResonance->type (FL_TOGGLE_BUTTON);
        RevResonance->box (FL_PLASTIC_UP_BOX);
        RevResonance->color (Info->GUI_COLOUR);
        RevResonance->selection_color (Info->GUI_COLOUR);
        RevResonance->labelsize(10);
        RevResonance->callback((Fl_Callback*)cb_RevResonance);

	end();
}

void FilterPluginGUI::UpdateValues(SpiralPlugin *o)
{
	FilterPlugin *Plugin = (FilterPlugin*)o;

	Cutoff->value(100.0f-sqrt(Plugin->GetCutoff())-10.0f);
	Resonance->value(Plugin->GetResonance()-1.0f);

	RevCutoff->value(0);
	RevResonance->value(0);

	if (Plugin->GetRevCutoffMod()) RevCutoff->value(1);
	if (Plugin->GetRevResonanceMod()) RevResonance->value(1);
}

inline void FilterPluginGUI::cb_Cutoff_i(Fl_Slider* o, void* v)
{
	float value=100.0f-o->value();
	m_GUICH->Set("Cutoff",(float)(value*value)+10.0f);
}

void FilterPluginGUI::cb_Cutoff(Fl_Slider* o, void* v)
{ ((FilterPluginGUI*)(o->parent()))->cb_Cutoff_i(o,v); }

inline void FilterPluginGUI::cb_Resonance_i(Fl_Knob* o, void* v)
{ m_GUICH->Set("Resonance",(float)(o->value()+1.0f)); }
void FilterPluginGUI::cb_Resonance(Fl_Knob* o, void* v)
{ ((FilterPluginGUI*)(o->parent()))->cb_Resonance_i(o,v); }

inline void FilterPluginGUI::cb_RevCutoff_i(Fl_Button* o, void* v)
{ m_GUICH->Set("RevC",(bool)o->value()); }
void FilterPluginGUI::cb_RevCutoff(Fl_Button* o, void* v)
{ ((FilterPluginGUI*)(o->parent()))->cb_RevCutoff_i(o,v); }

inline void FilterPluginGUI::cb_RevResonance_i(Fl_Button* o, void* v)
{ m_GUICH->Set("RevR",(bool)o->value()); }
void FilterPluginGUI::cb_RevResonance(Fl_Button* o, void* v) 
{ ((FilterPluginGUI*)(o->parent()))->cb_RevResonance_i(o,v); }
	
const string FilterPluginGUI::GetHelpText(const string &loc){
    return string("")
	+ "The standard SpiralSynth filter, based on the (zxforms design).\n"
	+ "Quite a meaty sound - low pass only, nice for bass modulations.\n"
	+ "With variable emphasis/cutoff CV's.\n\n"
	+ "It's also pretty fast, and well tested in SpiralSynth.";
}
