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

#include "SwitchPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

SwitchPluginGUI::SwitchPluginGUI(int w, int h,SwitchPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	m_Mix = new Fl_Button (15, 20, 50, 20, "Mix");
        m_Mix->selection_color (Info->GUI_COLOUR);
        m_Mix->color (Info->GUI_COLOUR);
        m_Mix->box (FL_PLASTIC_UP_BOX);
        m_Mix->labelsize(8);
        m_Mix->type(1);
	m_Mix->callback((Fl_Callback*)cb_Mix);

	end();
}



void SwitchPluginGUI::UpdateValues(SpiralPlugin *o)
{
	SwitchPlugin* Plugin = (SwitchPlugin*)o;
	m_Mix->value(Plugin->GetMix());
}

//// Callbacks ////
inline void SwitchPluginGUI::cb_Mix_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Mix",(bool)o->value());
}
void SwitchPluginGUI::cb_Mix(Fl_Button* o, void* v)
{ ((SwitchPluginGUI*)(o->parent()))->cb_Mix_i(o,v);}

const string SwitchPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Switches between inputs, if mix is turned on the inputs are\n"
	+ "crossfaded between, according to the CV input";
}
