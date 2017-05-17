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

#include "EchoPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

EchoPluginGUI::EchoPluginGUI(int w, int h, EchoPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
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

        m_Feedback = new Fl_Knob (66, 38, 45, 45, "Feedback");
        m_Feedback->user_data ((void*)(this));
        m_Feedback->color (Info->GUI_COLOUR);
	m_Feedback->type (Fl_Knob::DOTLIN);
        m_Feedback->labelsize (10);
        m_Feedback->maximum (1.1);
        m_Feedback->step (0.01);
        m_Feedback->value (0.4);
	m_Feedback->callback ((Fl_Callback*)cb_Feedback);
        m_CtlGroup->add (m_Feedback);

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

	m_NumFeedback = new Fl_Counter (6, 70, 110, 20, "Feedback");
        m_NumFeedback->user_data ((void*)(this));
        m_NumFeedback->labelsize (10);
        m_NumFeedback->box (FL_PLASTIC_UP_BOX);
        m_NumFeedback->color (Info->GUI_COLOUR);
        m_NumFeedback->maximum (m_Feedback->maximum());
	m_NumFeedback->minimum (m_Feedback->minimum());
        m_NumFeedback->step (m_Feedback->step());
        m_NumFeedback->lstep (0.1);
        m_NumFeedback->value (m_Feedback->value());
        m_NumFeedback->callback ((Fl_Callback*)cb_NumFeedback);
        m_NumGroup->add (m_NumFeedback);

        m_Bounce = new Fl_LED_Button (62, 106, 23, 23, "Bounce");
        m_Bounce->labelsize (10);
        m_Bounce->callback ((Fl_Callback*)cb_Bounce);
        add (m_Bounce);

        end();
}

void EchoPluginGUI::UpdateValues (SpiralPlugin *o)
{
	EchoPlugin *Plugin = (EchoPlugin*)o;
        m_Delay->value (Plugin->GetDelay());
        m_NumDelay->value (m_Delay->value());
	m_Feedback->value (Plugin->GetFeedback());
        m_NumFeedback->value (m_Feedback->value());
        m_Bounce->value (Plugin->GetBounce());
}

// Button Callbacks

inline void EchoPluginGUI::cb_Bounce_i (Fl_LED_Button* o, void* v)
{
       m_GUICH->Set ("Bounce", o->value());
}

void EchoPluginGUI::cb_Bounce (Fl_LED_Button* o, void* v)
{
     ((EchoPluginGUI*)(o->parent()))->cb_Bounce_i (o, v);
}

// Knob Callbacks

inline void EchoPluginGUI::cb_Delay_i (Fl_Knob* o, void* v)
{
       float value = o->value();
       m_NumDelay->value (value);
       m_GUICH->Set ("Delay", value);
}

void EchoPluginGUI::cb_Delay (Fl_Knob* o, void* v)
{
     ((EchoPluginGUI*)(o->user_data()))->cb_Delay_i (o, v);
}

inline void EchoPluginGUI::cb_Feedback_i(Fl_Knob* o, void* v)
{
       float value = o->value();
       m_NumFeedback->value (value);
       m_GUICH->Set ("Feedback", value);
}

void EchoPluginGUI::cb_Feedback(Fl_Knob* o, void* v)
{
     ((EchoPluginGUI*)(o->user_data()))->cb_Feedback_i(o,v);
}

// Counter Callbacks

inline void EchoPluginGUI::cb_NumDelay_i (Fl_Counter* o, void* v)
{
        float value = o->value();
        m_Delay->value (value);
	m_GUICH->Set ("Delay", value);
}

void EchoPluginGUI::cb_NumDelay (Fl_Counter* o, void* v)
{
     ((EchoPluginGUI*)(o->user_data()))->cb_NumDelay_i (o, v);
}

inline void EchoPluginGUI::cb_NumFeedback_i(Fl_Counter* o, void* v)
{
        float value = o->value();
        m_Feedback->value (value);
        m_GUICH->Set ("Feedback", value);
}

void EchoPluginGUI::cb_NumFeedback(Fl_Counter* o, void* v)
{
     ((EchoPluginGUI*)(o->user_data()))->cb_NumFeedback_i (o, v);
}

const string EchoPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "This plugin is a fully operational delay with feedback\n"
    + "- providing a no-frills echo effect. The delay and\n"
    + "feedback controls can be modulated via CV inputs.\n\n"
    + "Bounce mode makes the echo bounce to-and-fro between\n"
    + "the 2 (stereo) outputs.\n";
}
