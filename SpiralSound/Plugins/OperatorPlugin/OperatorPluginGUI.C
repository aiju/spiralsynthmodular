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

#include "OperatorPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
//#include <stdio.h>

using namespace std;

////////////////////////////////////////////

OperatorPluginGUI::OperatorPluginGUI(int w, int h,OperatorPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	m_Constant = new Fl_Counter (5, 15, 110, 20, "Constant");
        m_Constant->color (Info->GUI_COLOUR);
        m_Constant->box (FL_PLASTIC_UP_BOX);
        m_Constant->labelsize (10);
	m_Constant->align (FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);
        m_Constant->textsize (10);
        m_Constant->value (0);
        m_Constant->step (0.01);
        m_Constant->lstep (1);
        //m_Constant->when (FL_WHEN_ENTER_KEY);
	m_Constant->callback ((Fl_Callback*)cb_Constant);

	m_Add = new Fl_Button (5, 50, 20, 20, "+");
        m_Add->type (FL_RADIO_BUTTON);
        m_Add->box (FL_PLASTIC_UP_BOX);
        m_Add->color (Info->GUI_COLOUR);
        m_Add->selection_color (Info->GUI_COLOUR);
	m_Add->value (true);
 	m_Add->callback ((Fl_Callback*)cb_Add);

	m_Sub = new Fl_Button (35, 50, 20, 20, "-");
        m_Sub->type (FL_RADIO_BUTTON);
        m_Sub->box (FL_PLASTIC_UP_BOX);
        m_Sub->color (Info->GUI_COLOUR);
        m_Sub->selection_color (Info->GUI_COLOUR);
	m_Sub->callback ((Fl_Callback*)cb_Sub);

	m_Mul = new Fl_Button (65, 50, 20, 20, "x");
        m_Mul->type (FL_RADIO_BUTTON);
        m_Mul->box (FL_PLASTIC_UP_BOX);
        m_Mul->color (Info->GUI_COLOUR);
        m_Mul->selection_color (Info->GUI_COLOUR);
 	m_Mul->callback ((Fl_Callback*)cb_Mul);

	m_Div = new Fl_Button (95, 50, 20, 20, "÷");
        m_Div->type (FL_RADIO_BUTTON);
        m_Div->box (FL_PLASTIC_UP_BOX);
        m_Div->color (Info->GUI_COLOUR);
        m_Div->selection_color (Info->GUI_COLOUR);
	m_Div->callback ((Fl_Callback*)cb_Div);

        end();
}

void OperatorPluginGUI::UpdateValues(SpiralPlugin *o)
{
	OperatorPlugin* Plugin = (OperatorPlugin*)o;
	m_Add->value(false);
	m_Sub->value(false);
	m_Mul->value(false);
	m_Div->value(false);
	switch (Plugin->GetOperator())
	{
		case OperatorPlugin::ADD : m_Add->value(true); break;
		case OperatorPlugin::SUB : m_Sub->value(true); break;
		case OperatorPlugin::MUL : m_Mul->value(true); break;
		case OperatorPlugin::DIV : m_Div->value(true); break;
                default: break;
	}
	//char t[256];
	//sprintf(t,"%f",Plugin->GetConstant());
	m_Constant->value (Plugin->GetConstant());
}

//// Callbacks ////
inline void OperatorPluginGUI::cb_Add_i (Fl_Button* o, void* v) {
       if (o->value()) m_GUICH->Set ("Operator", (int)OperatorPlugin::ADD);
}

void OperatorPluginGUI::cb_Add (Fl_Button* o, void* v) {
     ((OperatorPluginGUI*)(o->parent()))->cb_Add_i (o, v);
}

inline void OperatorPluginGUI::cb_Sub_i (Fl_Button* o, void* v) {
       if (o->value()) m_GUICH->Set ("Operator",(int)OperatorPlugin::SUB);
}

void OperatorPluginGUI::cb_Sub (Fl_Button* o, void* v) {
     ((OperatorPluginGUI*)(o->parent()))->cb_Sub_i (o, v);
}

inline void OperatorPluginGUI::cb_Mul_i (Fl_Button* o, void* v) {
       if (o->value()) m_GUICH->Set ("Operator",(int)OperatorPlugin::MUL);
}

void OperatorPluginGUI::cb_Mul (Fl_Button* o, void* v) {
     ((OperatorPluginGUI*)(o->parent()))->cb_Mul_i (o, v);
}

inline void OperatorPluginGUI::cb_Div_i (Fl_Button* o, void* v) {
       if (o->value()) m_GUICH->Set("Operator",(int)OperatorPlugin::DIV);
}

void OperatorPluginGUI::cb_Div (Fl_Button* o, void* v) {
     ((OperatorPluginGUI*)(o->parent()))->cb_Div_i (o, v);
}

inline void OperatorPluginGUI::cb_Constant_i (Fl_Counter* o, void* v) {
       m_GUICH->Set ("Constant", (float) /*strtod(*/ o->value() /*,NULL)*/);
}

void OperatorPluginGUI::cb_Constant (Fl_Counter* o, void* v) {
     ((OperatorPluginGUI*)(o->parent()))->cb_Constant_i (o, v);
}

const string OperatorPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Simply performs the operation on the input data,\n"
    + "if there is only one input, it uses the constant.";
}
