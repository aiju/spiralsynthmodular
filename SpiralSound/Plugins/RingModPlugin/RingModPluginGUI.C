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

#include "RingModPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

RingModPluginGUI::RingModPluginGUI (int w, int h, RingModPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch)
{
	m_Amount = new Fl_Knob (15, 12, 50, 50, "Amount");
    m_Amount->color(Info->GUI_COLOUR);
	m_Amount->type(Fl_Knob::DOTLIN);
    m_Amount->labelsize(10);
    m_Amount->maximum(3);
    m_Amount->step(0.0001);
    m_Amount->value(1);   
	m_Amount->callback((Fl_Callback*)cb_Amount);
	
	end();
}

void RingModPluginGUI::UpdateValues(SpiralPlugin *o)
{
	RingModPlugin* Plugin = (RingModPlugin*)o;
	m_Amount->value(Plugin->GetAmount());
}

inline void RingModPluginGUI::cb_Amount_i(Fl_Knob* o, void* v) 
{ 
	m_GUICH->Set("Amount",(float)o->value()); 
}
void RingModPluginGUI::cb_Amount(Fl_Knob* o, void* v) 
{ ((RingModPluginGUI*)(o->parent()))->cb_Amount_i(o,v); }

const string RingModPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "A ring modulator, multiplies two inputs together.\n"
    + "An extra dial sets the amount of modulation. The\n"
    + "ring modulation effect creates an extra harmonic\n"
    + "in the signal, although this is often out of tune\n"
    + "with the other signals, its useful for creating bell\n"
    + "or struck metallic sounds.\n";
}
