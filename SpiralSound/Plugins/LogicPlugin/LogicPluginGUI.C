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

#include "LogicPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>
#include <stdio.h>

using namespace std;

////////////////////////////////////////////

LogicPluginGUI::LogicPluginGUI(int w, int h,LogicPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	int Width=38, Depth=20;

	m_AND = new Fl_Button (2, 15, Width, Depth, "AND");
        m_AND->type (FL_TOGGLE_BUTTON);
	m_AND->value (1);
	m_AND->labelsize (10);
        m_AND->box (FL_PLASTIC_UP_BOX);
        m_AND->color (Info->GUI_COLOUR);
        m_AND->selection_color (Info->GUI_COLOUR);
 	m_AND->callback ((Fl_Callback*)cb_AND);

	m_OR = new Fl_Button(2, 36, Width, Depth, "OR");
        m_OR->type (FL_TOGGLE_BUTTON);
	m_OR->labelsize (10);
	m_OR->box (FL_PLASTIC_UP_BOX);
        m_OR->color (Info->GUI_COLOUR);
        m_OR->selection_color (Info->GUI_COLOUR);
	m_OR->callback ((Fl_Callback*)cb_OR);

	m_NOT = new Fl_Button (2, 57, Width, Depth, "NOT");
        m_NOT->type (FL_TOGGLE_BUTTON);
	m_NOT->labelsize (10);
	m_NOT->box (FL_PLASTIC_UP_BOX);
        m_NOT->color (Info->GUI_COLOUR);
        m_NOT->selection_color (Info->GUI_COLOUR);
 	m_NOT->callback ((Fl_Callback*)cb_NOT);

	m_NAND = new Fl_Button(2, 78, Width, Depth, "NAND");
        m_NAND->type (FL_TOGGLE_BUTTON);
 	m_NAND->labelsize (10);
 	m_NAND->box (FL_PLASTIC_UP_BOX);
  	m_NAND->color (Info->GUI_COLOUR);
  	m_NAND->selection_color (Info->GUI_COLOUR);
	m_NAND->callback ((Fl_Callback*)cb_NAND);

	m_NOR = new Fl_Button (41, 15, Width, Depth, "NOR");
        m_NOR->type (FL_TOGGLE_BUTTON);
	m_NOR->labelsize (10);
	m_NOR->box (FL_PLASTIC_UP_BOX);
        m_NOR->color (Info->GUI_COLOUR);
        m_NOR->selection_color (Info->GUI_COLOUR);
	m_NOR->callback ((Fl_Callback*)cb_NOR);

	m_XOR = new Fl_Button (41, 36, Width, Depth, "XOR");
        m_XOR->type (FL_TOGGLE_BUTTON);
	m_XOR->labelsize (10);
	m_XOR->box (FL_PLASTIC_UP_BOX);
        m_XOR->color (Info->GUI_COLOUR);
        m_XOR->selection_color (Info->GUI_COLOUR);
 	m_XOR->callback ((Fl_Callback*)cb_XOR);

	m_XNOR = new Fl_Button (41, 57, Width, Depth, "XNOR");
        m_XNOR->type (FL_TOGGLE_BUTTON);
	m_XNOR->labelsize (10);
	m_XNOR->box (FL_PLASTIC_UP_BOX);
        m_XNOR->color (Info->GUI_COLOUR);
        m_XNOR->selection_color (Info->GUI_COLOUR);
	m_XNOR->callback ((Fl_Callback*)cb_XNOR);

        m_Inputs = new Fl_Counter (10, 104, 60, 20, "Inputs");
        m_Inputs->labelsize (10);
        m_Inputs->type (FL_SIMPLE_COUNTER);
        m_Inputs->box (FL_PLASTIC_UP_BOX);
        m_Inputs->color (Info->GUI_COLOUR);
        m_Inputs->step (1);
        m_Inputs->value (2);
        m_Inputs->callback ((Fl_Callback*) cb_Inputs);

	end();
}

void LogicPluginGUI::ClearButtons()
{
	m_AND->value(false);
	m_OR->value(false);
	m_NOT->value(false);
	m_NAND->value(false);
	m_NOR->value(false);
	m_XOR->value(false);
	m_XNOR->value(false);
}

void LogicPluginGUI::UpdateValues(SpiralPlugin *o)
{
	LogicPlugin* Plugin = (LogicPlugin*)o;
	ClearButtons();
	switch (Plugin->GetOperator())
	{
		case LogicPlugin::AND  : m_AND->value (true); break;
		case LogicPlugin::OR   : m_OR->value (true); break;
		case LogicPlugin::NOT  : m_NOT->value (true); break;
		case LogicPlugin::NAND : m_NAND->value (true); break;
		case LogicPlugin::NOR  : m_NOR->value (true); break;
		case LogicPlugin::XOR  : m_XOR->value (true); break;
		case LogicPlugin::XNOR : m_XNOR->value (true); break;
                default: break;
	}
        m_Inputs->value (Plugin->GetInputs());
}

//// Callbacks ////

inline void LogicPluginGUI::cb_Inputs_i (Fl_Counter* o, void* v) {
  if (o->value() < 2) o->value(2);
  else {
    m_GUICH->Set ("Inputs", int (o->value ()));
    m_GUICH->SetCommand (LogicPlugin::SETINPUTS);
    m_GUICH->Wait ();
    Resize (w(), h());
  }
}

void LogicPluginGUI::cb_Inputs (Fl_Counter* o, void* v) {
  ((LogicPluginGUI*) (o->parent ())) -> cb_Inputs_i (o, v);
}

inline void LogicPluginGUI::cb_AND_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::AND);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_AND(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_AND_i(o,v);}

inline void LogicPluginGUI::cb_OR_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::OR);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_OR(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_OR_i(o,v);}

inline void LogicPluginGUI::cb_NOT_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::NOT);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_NOT(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_NOT_i(o,v);}

inline void LogicPluginGUI::cb_NAND_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::NAND);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_NAND(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_NAND_i(o,v);}

inline void LogicPluginGUI::cb_NOR_i(Fl_Button* o, void* v)
{	
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::NOR);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_NOR(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_NOR_i(o,v);}

inline void LogicPluginGUI::cb_XOR_i(Fl_Button* o, void* v)
{	
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::XOR);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_XOR(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_XOR_i(o,v);}

inline void LogicPluginGUI::cb_XNOR_i(Fl_Button* o, void* v)
{	
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::XNOR);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_XNOR(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_XNOR_i(o,v);}

const string LogicPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Note that NOT only uses input 1,\nand XOR/XNOR only use inputs 1 and 2\n\n"
    + "1001010111010101101111101010100101010101010100010100100101\n"
    + "0010101010111010010010101010001010011110001010101000101010\n"
    + "1110111101101001000010101010111110101010101010101111010101\n"
    + "0011011111010101101000001010101010001010100001100111010111";
}
