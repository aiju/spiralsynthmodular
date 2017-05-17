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

#include "MoogFilterPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

MoogFilterPluginGUI::MoogFilterPluginGUI(int w, int h,MoogFilterPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
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

void MoogFilterPluginGUI::UpdateValues(SpiralPlugin *o)
{
	MoogFilterPlugin *Plugin = (MoogFilterPlugin*)o;
	
	Cutoff->value(1.0f-Plugin->GetCutoff());
	Resonance->value(Plugin->GetResonance());
}

inline void MoogFilterPluginGUI::cb_Cutoff_i(Fl_Slider* o, void* v) 
{ 
	float value=1.0f-o->value();
//	m_Plugin->SetCutoff((value*value)+10.0f); 
	m_GUICH->Set("Cutoff",value); 
}
void MoogFilterPluginGUI::cb_Cutoff(Fl_Slider* o, void* v) 
{ ((MoogFilterPluginGUI*)(o->parent()))->cb_Cutoff_i(o,v); }

inline void MoogFilterPluginGUI::cb_Resonance_i(Fl_Knob* o, void* v) 
{ m_GUICH->Set("Resonance",(float)(o->value())); }
void MoogFilterPluginGUI::cb_Resonance(Fl_Knob* o, void* v) 
{ ((MoogFilterPluginGUI*)(o->parent()))->cb_Resonance_i(o,v); }

const string MoogFilterPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Classic moog filter. Very different sound to the other filters,\n" 
	+ "needless to say, very squelchy. As well as lowpass, band and high\n"
	+ "pass are simultaneously calculated too. The emphasis can be pushed\n"
	+ "into self oscillation (careful of the speakers). In this way, it\n" 
	+ "can be used to generate sinewave oscillations.";
}
