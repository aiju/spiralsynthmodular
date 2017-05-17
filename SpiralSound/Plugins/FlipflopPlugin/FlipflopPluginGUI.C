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

#include "FlipflopPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

FlipflopPluginGUI::FlipflopPluginGUI(int w, int h,FlipflopPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	m_TriggerTime = new Fl_Knob (20, 15, 40, 40, "Trigger Time");
        m_TriggerTime->color (Info->GUI_COLOUR);
    m_TriggerTime->labelsize(8);
    m_TriggerTime->maximum(1.0f);
    m_TriggerTime->minimum(0.0f);
    m_TriggerTime->step(0.0001f);
	m_TriggerTime->labelsize(8);
	m_TriggerTime->callback((Fl_Callback*)cb_TriggerTime);

	m_Monostable = new Fl_Button (10, 70, 60, 20, "Monostable");
        m_Monostable->color (Info->GUI_COLOUR);
        m_Monostable->selection_color (Info->GUI_COLOUR);
        m_Monostable->box (FL_PLASTIC_UP_BOX);
	m_Monostable->labelsize(8);
	m_Monostable->type(1);
	m_Monostable->callback((Fl_Callback*)cb_Monostable);
	
	end();
}



void FlipflopPluginGUI::UpdateValues(SpiralPlugin *o)
{
	FlipflopPlugin* Plugin=(FlipflopPlugin*)o;
	m_TriggerTime->value(Plugin->GetTriggerTime());
	m_Monostable->value(Plugin->GetMonostable());
}

//// Callbacks ////
inline void FlipflopPluginGUI::cb_TriggerTime_i(Fl_Knob* o, void* v)
{
	m_GUICH->Set("TriggerTime",(float)o->value());
}
void FlipflopPluginGUI::cb_TriggerTime(Fl_Knob* o, void* v)
{ ((FlipflopPluginGUI*)(o->parent()))->cb_TriggerTime_i(o,v);}

inline void FlipflopPluginGUI::cb_Monostable_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Monostable",(bool)o->value());
}
void FlipflopPluginGUI::cb_Monostable(Fl_Button* o, void* v)
{ ((FlipflopPluginGUI*)(o->parent()))->cb_Monostable_i(o,v);}



const string FlipflopPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "This plugin has two modes, bistable (the default) converts momentary\n"
	+ "pulses into sustained pulses, i.e one pulse to set the output to 1.0,\n"
	+ "and another pulse to flip it back to -1.0.\n\n"
	+ "Monostable mode is sort of the reverse, any input pulse detected will\n"
	+ "cause an output pulse to be generated, the length of which is set by the\n"
	+ "trigger time control (out of 1 second max)\n\n"
	+ "For a better description, google \"flipflop bistable monostable\" :)";
}
