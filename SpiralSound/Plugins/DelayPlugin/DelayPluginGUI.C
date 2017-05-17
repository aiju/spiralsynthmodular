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

#include "DelayPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

DelayPluginGUI::DelayPluginGUI(int w, int h, DelayPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch)
{
       	m_TheTabs = new Fl_Tabs (2, 14, 118, 90, "");
	m_TheTabs->box (FL_PLASTIC_DOWN_BOX);
	m_TheTabs->color (Info->GUI_COLOUR);
	add (m_TheTabs);

	m_CtlGroup = new Fl_Group (2, 28, 118, 66, "Control");
	m_CtlGroup->labelsize (10);
        m_TheTabs->add (m_CtlGroup);

        m_Delay = new Fl_Knob (12, 38, 45, 45, "Delay");
        m_Delay->user_data ((void*)(this));
	m_Delay->color (Info->GUI_COLOUR);
	m_Delay->type (Fl_Knob::DOTLIN);
        m_Delay->labelsize (10);
	m_Delay->minimum (0);
	m_Delay->maximum (1);
        m_Delay->step (0.001);
        m_Delay->value (0.5);
        m_Delay->callback ((Fl_Callback*)cb_Delay);
        m_CtlGroup->add (m_Delay);

	m_Mix = new Fl_Knob (66, 38, 45, 45, "Mix");
        m_Mix->user_data ((void*)(this));
        m_Mix->color (Info->GUI_COLOUR);
	m_Mix->type (Fl_Knob::DOTLIN);
        m_Mix->labelsize (10);
        m_Mix->maximum (1);
        m_Mix->step (0.01);
        m_Mix->value (0);
	m_Mix->callback ((Fl_Callback*)cb_Mix);
        m_CtlGroup->add (m_Mix);

	m_NumGroup = new Fl_Group (2, 28, 118, 66, "Numbers");
	m_NumGroup->labelsize (10);
        m_TheTabs->add (m_NumGroup);

        m_NumDelay = new Fl_Counter (6, 36, 110, 20, "Delay (secs)");
        m_NumDelay->user_data ((void*)(this));
        m_NumDelay->labelsize (10);
        m_NumDelay->box (FL_PLASTIC_UP_BOX);
        m_NumDelay->color (Info->GUI_COLOUR);
        m_NumDelay->maximum (m_Delay->maximum());
	m_NumDelay->minimum (m_Delay->minimum());
        m_NumDelay->step (m_Delay->step());
        m_NumDelay->lstep (0.1);
        m_NumDelay->value (m_Delay->value());
        m_NumDelay->callback ((Fl_Callback*)cb_NumDelay);
        m_NumGroup->add (m_NumDelay);

	m_NumMix = new Fl_Counter (6, 70, 110, 20, "Mix");
        m_NumMix->user_data ((void*)(this));
        m_NumMix->labelsize (10);
        m_NumMix->box (FL_PLASTIC_UP_BOX);
        m_NumMix->color (Info->GUI_COLOUR);
        m_NumMix->maximum (m_Mix->maximum());
	m_NumMix->minimum (m_Mix->minimum());
        m_NumMix->step (m_Mix->step());
        m_NumMix->lstep (0.1);
        m_NumMix->value (m_Mix->value());
        m_NumMix->callback ((Fl_Callback*)cb_NumMix);
        m_NumGroup->add (m_NumMix);

        end();
}

void DelayPluginGUI::UpdateValues(SpiralPlugin *o)
{
        float value;
        DelayPlugin *Plugin = (DelayPlugin*)o;

        value = Plugin->GetDelay();
	m_Delay->value (value);
	m_NumDelay->value (value);

        value = Plugin->GetMix();
	m_Mix->value (value);
	m_NumMix->value (value);
}

inline void DelayPluginGUI::cb_Delay_i (Fl_Knob* o, void* v)
{
	float value = o->value();
        m_NumDelay->value (value);
	m_GUICH->Set ("Delay", value);
}

void DelayPluginGUI::cb_Delay (Fl_Knob* o, void* v)
{
     ((DelayPluginGUI*)(o->user_data()))->cb_Delay_i (o, v);
}

inline void DelayPluginGUI::cb_Mix_i (Fl_Knob* o, void* v)
{
     float value = o->value();
     m_NumMix->value (value);
     m_GUICH->Set ("Mix", value);
}

void DelayPluginGUI::cb_Mix (Fl_Knob* o, void* v)
{
     ((DelayPluginGUI*)(o->user_data()))->cb_Mix_i (o, v);
}

inline void DelayPluginGUI::cb_NumDelay_i (Fl_Counter* o, void* v)
{
	float value = o->value();
        m_Delay->value (value);
	m_GUICH->Set ("Delay", value);
}

void DelayPluginGUI::cb_NumDelay (Fl_Counter* o, void* v)
{
     ((DelayPluginGUI*)(o->user_data()))->cb_NumDelay_i (o, v);
}

inline void DelayPluginGUI::cb_NumMix_i (Fl_Counter* o, void* v)
{
     float value = o->value();
     m_Mix->value (value);
     m_GUICH->Set ("Mix", value);
}

void DelayPluginGUI::cb_NumMix (Fl_Counter* o, void* v)
{
     ((DelayPluginGUI*)(o->user_data()))->cb_NumMix_i (o, v);
}

const string DelayPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "The delay plugins delays the input signal, and can\n"
    + "mix the current signal into the output, the amount\n"
    + "is set by the dial in the plugin window.\n"
    + "\n"
    + "The delay time and read head position can be modified\n"
    + "by input CV's. The read head is the place in the buffer\n"
    + "the output sample is taken from, relative to the write\n"
    + "head.\n"
    + "\n"
    + "This plugin can be used as the base of a number of effects,\n"
    + "such as phasers, flangers and complex echos. If the output\n"
    + "is fed back into the input, you get a similar effect\n"
    + "to the echo, but you can add cool effects by routing\n"
    + "the signal back through a lowpass filter (for example).\n";
}
