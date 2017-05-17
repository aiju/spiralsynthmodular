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

#include "CounterPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <stdio.h>

using namespace std;

////////////////////////////////////////////

CounterPluginGUI::CounterPluginGUI(int w, int h,CounterPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	m_Count = new Fl_Input(15, 20, 50, 20, "Count");
    m_Count->color(Info->GUI_COLOUR);
    m_Count->labelsize(8);
	m_Count->align(FL_ALIGN_BOTTOM|FL_ALIGN_CENTER);
    m_Count->textsize(10);
    m_Count->value("4");
    m_Count->when(FL_WHEN_ENTER_KEY);
	m_Count->callback((Fl_Callback*)cb_Count);
	
	end();
}



void CounterPluginGUI::UpdateValues(SpiralPlugin *o)
{
	CounterPlugin* Plugin = (CounterPlugin*)o;
	
	char t[256];
	sprintf(t,"%d",Plugin->GetCount());
	m_Count->value(t);
}

//// Callbacks ////
inline void CounterPluginGUI::cb_Count_i(Fl_Input* o, void* v)
{
	m_GUICH->Set("Count",(int)strtod(o->value(),NULL));
}
void CounterPluginGUI::cb_Count(Fl_Input* o, void* v)
{ ((CounterPluginGUI*)(o->parent()))->cb_Count_i(o,v);}

const string CounterPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Counts input pulses (zero crossings) and flips the output\n"
	+ "CV every time the count is reached.";
}
