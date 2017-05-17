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

#include "TrigPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <stdio.h>

using namespace std;

////////////////////////////////////////////

TrigPluginGUI::TrigPluginGUI(int w, int h,TrigPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	m_Sin = new Fl_Button (10, 15, 60, 20, "Sin");
	m_Sin->labelsize (8);
        m_Sin->type (FL_TOGGLE_BUTTON);
        m_Sin->box (FL_PLASTIC_UP_BOX);
	m_Sin->value (1);
        m_Sin->color (Info->GUI_COLOUR);
        m_Sin->selection_color (Info->GUI_COLOUR);
 	m_Sin->callback ((Fl_Callback*)cb_Sin);

	m_Cos = new Fl_Button (10, 35, 60, 20, "Cos");
	m_Cos->labelsize (8);
        m_Cos->type (FL_TOGGLE_BUTTON);
        m_Cos->box (FL_PLASTIC_UP_BOX);
        m_Cos->color (Info->GUI_COLOUR);
        m_Cos->selection_color (Info->GUI_COLOUR);
	m_Cos->callback ((Fl_Callback*)cb_Cos);

	m_Tan = new Fl_Button (10, 55, 60, 20, "Tan");
	m_Tan->labelsize (8);
        m_Tan->type (FL_TOGGLE_BUTTON);
        m_Tan->box (FL_PLASTIC_UP_BOX);
        m_Tan->color (Info->GUI_COLOUR);
        m_Tan->selection_color (Info->GUI_COLOUR);
 	m_Tan->callback ((Fl_Callback*)cb_Tan);

	end();
}



void TrigPluginGUI::UpdateValues(SpiralPlugin *o)
{
	TrigPlugin* Plugin = (TrigPlugin*)o;
	m_Sin->value(false);
	m_Cos->value(false);
	m_Tan->value(false);
	switch (Plugin->GetOperator())
	{
		case TrigPlugin::SIN : m_Sin->value(true); break;
		case TrigPlugin::COS : m_Cos->value(true); break;
		case TrigPlugin::TAN : m_Tan->value(true); break;
                default: break;
	}
}

//// Callbacks ////
inline void TrigPluginGUI::cb_Sin_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		m_Cos->value(false);
		m_Tan->value(false);
		m_GUICH->Set("Operator",(int)TrigPlugin::SIN);
	}
	else
	{
		o->value(true);
	}
}
void TrigPluginGUI::cb_Sin(Fl_Button* o, void* v)
{ ((TrigPluginGUI*)(o->parent()))->cb_Sin_i(o,v);}

inline void TrigPluginGUI::cb_Cos_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		m_Sin->value(false);
		m_Tan->value(false);
		m_GUICH->Set("Operator",(int)TrigPlugin::COS);
	}
	else
	{
		o->value(true);
	}
}
void TrigPluginGUI::cb_Cos(Fl_Button* o, void* v)
{ ((TrigPluginGUI*)(o->parent()))->cb_Cos_i(o,v);}

inline void TrigPluginGUI::cb_Tan_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		m_Sin->value(false);
		m_Cos->value(false);
		m_GUICH->Set("Operator",(int)TrigPlugin::TAN);
	}
	else
	{
		o->value(true);
	}
}
void TrigPluginGUI::cb_Tan(Fl_Button* o, void* v)
{ ((TrigPluginGUI*)(o->parent()))->cb_Tan_i(o,v);}

const string TrigPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Calculates the sin/cos/tan of the input value, 1.0=360 degrees.";
}
