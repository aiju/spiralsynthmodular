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

#include "BeatMatchPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

BeatMatchPluginGUI::BeatMatchPluginGUI(int w, int h,BeatMatchPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	m_Sense = new Fl_Knob (15, 15, 50, 50, "Sensitivity");
    m_Sense->color(Info->GUI_COLOUR);
    m_Sense->labelsize(8);
	m_Sense->callback((Fl_Callback*)cb_Sense);
	
	end();
}



void BeatMatchPluginGUI::UpdateValues(SpiralPlugin *o)
{
	BeatMatchPlugin* Plugin = (BeatMatchPlugin*)o;
	m_Sense->value(Plugin->GetSensitivity());
}

//// Callbacks ////
inline void BeatMatchPluginGUI::cb_Sense_i(Fl_Knob* o, void* v)
{
	m_GUICH->Set("Sensitivity",(float)o->value());
}
void BeatMatchPluginGUI::cb_Sense(Fl_Knob* o, void* v)
{ ((BeatMatchPluginGUI*)(o->parent()))->cb_Sense_i(o,v);}

const string BeatMatchPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Produces an output pulse that slowly syncs to the input pulses,\n" 
	+ "the sensitivity sets the time it takes to sync up.\n"
	+ "Can be used to transition between different tempo speeds and\n" 
	+ "other stuff";
}
