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

#include "EnvFollowerPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

EnvFollowerPluginGUI::EnvFollowerPluginGUI(int w, int h,EnvFollowerPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	m_Attack = new Fl_Knob (8, 18, 40, 40, "Attack");
	m_Attack->color(Info->GUI_COLOUR);
	m_Attack->type(Fl_Knob::DOTLIN);
   	m_Attack->labelsize(10);
   	m_Attack->maximum(1);
   	m_Attack->step(0.001);
   	m_Attack->value(0.5);
	m_Attack->callback((Fl_Callback*)cb_Attack);
	add(m_Attack);

	m_Decay = new Fl_Knob (64, 18, 40, 40, "Decay");
	m_Decay->color(Info->GUI_COLOUR);
	m_Decay->type(Fl_Knob::DOTLIN);
   	m_Decay->labelsize(10);
   	m_Decay->maximum(1);
   	m_Decay->step(0.001);
   	m_Decay->value(0.5);
	m_Decay->callback((Fl_Callback*)cb_Decay);
	add(m_Decay);

	end();
}



void EnvFollowerPluginGUI::UpdateValues(SpiralPlugin *o)
{
	EnvFollowerPlugin *Plugin = (EnvFollowerPlugin*)o;
	m_Attack->value(Plugin->GetAttack());
	m_Decay->value(Plugin->GetDecay());
}
	
inline void EnvFollowerPluginGUI::cb_Decay_i(Fl_Knob* o, void* v)
{ m_GUICH->Set("Decay",o->value()); }
void EnvFollowerPluginGUI::cb_Decay(Fl_Knob* o, void* v)
{ ((EnvFollowerPluginGUI*)(o->parent()))->cb_Decay_i(o,v);}

inline void EnvFollowerPluginGUI::cb_Attack_i(Fl_Knob* o, void* v)
{ m_GUICH->Set("Attack",o->value()); }
void EnvFollowerPluginGUI::cb_Attack(Fl_Knob* o, void* v)
{ ((EnvFollowerPluginGUI*)(o->parent()))->cb_Attack_i(o,v);}

const string EnvFollowerPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "The Envelope Follower takes an audio signal input and\n"
    + "attempts to recreate the envelope that shaped it. This\n"
    + "is particually useful for extracting information from\n"
    + "samples, for use in a vocoder for example.\n"
    + "\n"
    + "The sensitivity of the EnvFollower can be set with the\n"
    + "Attack and Decay controls on the plugin window.\n";
}
