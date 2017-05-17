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

#include <FL/fl_draw.H>
#include <FL/fl_draw.H>
#include "AmpPluginGUI.h"

using namespace std;

////////////////////////////////////////////

AmpPluginGUI::AmpPluginGUI(int w, int h,AmpPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
        m_TheTabs = new Fl_Tabs (2, 14, 118, 104, "");
	m_TheTabs->box (FL_PLASTIC_DOWN_BOX);
        m_TheTabs->color (Info->GUI_COLOUR);
	add (m_TheTabs);

	m_CtlGroup = new Fl_Group (2, 28, 118, 80, "Control");
	m_CtlGroup->labelsize (10);
        m_TheTabs->add (m_CtlGroup);

	m_Gain = new Fl_Slider (24, 32, 20, 70, "Gain");
        m_Gain->user_data ((void*)(this));
	m_Gain->type (FL_VERT_NICE_SLIDER);
        m_Gain->selection_color (Info->GUI_COLOUR);
        m_Gain->box (FL_PLASTIC_DOWN_BOX);
        m_Gain->labelsize(10);
	m_Gain->maximum (4.0);
	m_Gain->minimum (0.0);
        m_Gain->step (0.0001);
        m_Gain->value (1.0);
        m_Gain->callback((Fl_Callback*)cb_Gain);
        m_CtlGroup->add (m_Gain);

	m_DC = new Fl_Slider (74, 32, 20, 70, "DC Offset");
        m_DC->user_data ((void*)(this));
	m_DC->type (FL_VERT_NICE_SLIDER);
        m_DC->selection_color (Info->GUI_COLOUR);
        m_DC->box (FL_PLASTIC_DOWN_BOX);
        m_DC->labelsize(10);
	m_DC->maximum (4.0);
	m_DC->minimum (0.0);
        m_DC->step (0.0001);
        m_DC->value (2.0);
        m_DC->callback((Fl_Callback*)cb_DC);
        m_CtlGroup->add (m_DC);

        m_NumGroup = new Fl_Group (2, 28, 118, 66, "Numbers");
	m_NumGroup->labelsize (10);
        m_TheTabs->add (m_NumGroup);

        m_NumGain = new Fl_Counter (6, 40, 110, 20, "Gain");
        m_NumGain->user_data ((void*)(this));
        m_NumGain->labelsize (10);
        m_NumGain->box (FL_PLASTIC_UP_BOX);
        m_NumGain->color (Info->GUI_COLOUR);
        m_NumGain->maximum (2.0);
	m_NumGain->minimum (-2.0);
        m_NumGain->step (0.001);
        m_NumGain->lstep (0.1);
        m_NumGain->value (1.0);
        m_NumGain->callback ((Fl_Callback*)cb_NumGain);
        m_NumGroup->add (m_NumGain);

	m_NumDC = new Fl_Counter (6, 78, 110, 20, "DC Offset");
        m_NumDC->user_data ((void*)(this));
        m_NumDC->labelsize (10);
        m_NumDC->box (FL_PLASTIC_UP_BOX);
        m_NumDC->color (Info->GUI_COLOUR);
        m_NumDC->maximum (2.0);
	m_NumDC->minimum (-2.0);
        m_NumDC->step (0.001);
        m_NumDC->lstep (0.1);
        m_NumDC->value (0.0);
        m_NumDC->callback ((Fl_Callback*)cb_NumDC);
        m_NumGroup->add (m_NumDC);

        m_Reset = new Fl_Button (64, 119, 56, 18, "Reset");
        m_Reset->user_data ((void*)(this));
        m_Reset->labelsize (10);
        m_Reset->box (FL_PLASTIC_UP_BOX);
        m_Reset->color (Info->GUI_COLOUR);
        m_Reset->selection_color (Info->GUI_COLOUR);
        m_Reset->callback ((Fl_Callback*)cb_Reset);
        add (m_Reset);

        end();
}

void AmpPluginGUI::UpdateValues (SpiralPlugin *o) {
        float value;
        AmpPlugin* Plugin = (AmpPlugin*)o;

        value = Plugin->GetGain();
        m_NumGain->value (value);
        m_Gain->value (2.0f - value);

        value = Plugin->GetDC();
        m_NumDC->value (value);
	m_DC->value (2.0f - value);
}

// control callbacks

inline void AmpPluginGUI::cb_Gain_i (Fl_Slider* o, void* v) {
        float value = 2.0f - o->value();
	m_GUICH->Set ("Gain", value);
        m_NumGain->value (value);
}

void AmpPluginGUI::cb_Gain (Fl_Slider* o, void* v) {
        ((AmpPluginGUI*)(o->user_data()))->cb_Gain_i (o, v);
}

inline void AmpPluginGUI::cb_DC_i (Fl_Slider* o, void* v) {
	float value = 2.0f - o->value();
	m_GUICH->Set ("DC", value);
        m_NumDC->value (value);
}

void AmpPluginGUI::cb_DC (Fl_Slider* o, void* v) {
        ((AmpPluginGUI*)(o->user_data()))->cb_DC_i (o, v);
}

// numeric callbacks

inline void AmpPluginGUI::cb_NumGain_i (Fl_Counter* o, void* v) {
        float value = o->value();
	m_GUICH->Set ("Gain", value);
        m_Gain->value (2.0f - value);
}

void AmpPluginGUI::cb_NumGain (Fl_Counter* o, void* v) {
        ((AmpPluginGUI*)(o->user_data()))->cb_NumGain_i (o, v);
}

inline void AmpPluginGUI::cb_NumDC_i (Fl_Counter* o, void* v) {
	float value = o->value();
	m_GUICH->Set ("DC", value);
        m_DC->value (2.0f - value);
}

void AmpPluginGUI::cb_NumDC (Fl_Counter* o, void* v) {
        ((AmpPluginGUI*)(o->user_data()))->cb_NumDC_i (o, v);
}

// button callbacks

inline void AmpPluginGUI::cb_Reset_i (Fl_Button* o, void* v) {
        m_NumGain->value (1.0);
        m_Gain->value (1.0);
        m_GUICH->Set ("Gain", 1.0f);
        m_NumDC->value (0.0);
	m_DC->value (2.0);
	m_GUICH->Set ("DC", 0);
}

void AmpPluginGUI::cb_Reset (Fl_Button* o, void* v) {
        ((AmpPluginGUI*)(o->user_data()))->cb_Reset_i (o, v);
}

// help text

const string AmpPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "A CV controlled amplifer. You also can use this device to modify\n"
    + "the signal's DC offset (the up or down in the range of values).\n\n"
    + "Handy for fine tuning CV's by hand, or modulating complex\n"
    + "controls.\n\n"
    + "The reset button quickly resets the gain back to 1 and the\n"
    + "offset back to 0\n";
}
