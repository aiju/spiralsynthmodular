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

#include "EnvelopePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

static const float TIMED_SLIDER_MAX = 3.0f;

////////////////////////////////////////////

EnvelopePluginGUI::EnvelopePluginGUI(int w, int h,EnvelopePlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
        m_Tabs = new Fl_Tabs (5, 12, 132, 115, "");
        m_Tabs->labelsize (10);
        m_Tabs->box (FL_PLASTIC_DOWN_BOX);
        add (m_Tabs);

        m_CtlGroup = new Fl_Group (5, 30, 132, 81, "Controls");
        m_CtlGroup->labelsize (10);
        m_Tabs->add (m_CtlGroup);

	m_Thresh = new Fl_Slider (10, 38, 20, 70, "T");
	m_Thresh->user_data ((void*)(this));
	m_Thresh->type (FL_VERT_NICE_SLIDER);
	m_Thresh->selection_color (Info->GUI_COLOUR);
        m_Thresh->box (FL_PLASTIC_DOWN_BOX);
        m_Thresh->labelsize (10);
	m_Thresh->maximum (1.0);
        m_Thresh->step (0.01);
        m_Thresh->value (0.99f);
        m_Thresh->callback ((Fl_Callback*)cb_Thresh);
	m_CtlGroup->add (m_Thresh);

	m_Attack = new Fl_Slider (30, 38, 20, 70, "A");
	m_Attack->user_data ((void*)(this));
	m_Attack->type (FL_VERT_NICE_SLIDER);
	m_Attack->selection_color (Info->GUI_COLOUR);
        m_Attack->box (FL_PLASTIC_DOWN_BOX);
        m_Attack->labelsize (10);
	m_Attack->maximum (TIMED_SLIDER_MAX);
        m_Attack->step (0.01);
        m_Attack->value (3.0f);
        m_Attack->callback ((Fl_Callback*)cb_Attack);
	m_CtlGroup->add (m_Attack);

	m_Decay = new Fl_Slider (50, 38, 20, 70, "D");
	m_Decay->user_data ((void*)(this));
	m_Decay->type (FL_VERT_NICE_SLIDER);
	m_Decay->selection_color (Info->GUI_COLOUR);
	m_Decay->box (FL_PLASTIC_DOWN_BOX);
        m_Decay->labelsize (10);
	m_Decay->maximum (TIMED_SLIDER_MAX);
        m_Decay->step (0.01);
        m_Decay->value (2.29);
        m_Decay->callback ((Fl_Callback*)cb_Decay);
	m_CtlGroup->add (m_Decay);

	m_Sustain = new Fl_Slider (70, 38, 20, 70, "S");
	m_Sustain->user_data ((void*)(this));
	m_Sustain->type (FL_VERT_NICE_SLIDER);
	m_Sustain->selection_color (Info->GUI_COLOUR);
	m_Sustain->box (FL_PLASTIC_DOWN_BOX);
        m_Sustain->labelsize (10);
	m_Sustain->maximum (1);
        m_Sustain->step (0.01);
        m_Sustain->value (0.0);
        m_Sustain->callback ((Fl_Callback*)cb_Sustain);
	m_CtlGroup->add (m_Sustain);

	m_Release = new Fl_Slider (90, 38, 20, 70, "R");
	m_Release->user_data ((void*)(this));
	m_Release->type (FL_VERT_NICE_SLIDER);
	m_Release->selection_color (Info->GUI_COLOUR);
	m_Release->box (FL_PLASTIC_DOWN_BOX);
        m_Release->labelsize (10);
	m_Release->maximum (TIMED_SLIDER_MAX);
        m_Release->step (0.01);
        m_Release->value (2.0);
        m_Release->callback ((Fl_Callback*)cb_Release);
	m_CtlGroup->add (m_Release);

	m_Volume = new Fl_Slider (110, 38, 20, 70, "V");
	m_Volume->user_data ((void*)(this));
	m_Volume->type (FL_VERT_NICE_SLIDER);
	m_Volume->selection_color (Info->GUI_COLOUR);
	m_Volume->box (FL_PLASTIC_DOWN_BOX);
        m_Volume->labelsize (10);
	m_Volume->maximum (1);
        m_Volume->step (0.01);
        m_Volume->value (0.5f);
        m_Volume->callback ((Fl_Callback*)cb_Volume);
	m_CtlGroup->add (m_Volume);

        m_NumGroup = new Fl_Group (5, 30, 132, 81, "Numbers");
        m_NumGroup->labelsize (10);
        m_Tabs->add (m_NumGroup);

	m_NumThresh = new Fl_Counter (10, 35, 60, 20, "Threshold (%)");
	m_NumThresh->user_data ((void*)(this));
	m_NumThresh->type (FL_SIMPLE_COUNTER);
	m_NumThresh->color (Info->GUI_COLOUR);
        m_NumThresh->box (FL_PLASTIC_UP_BOX);
        m_NumThresh->labelsize (8);
	m_NumThresh->maximum (100);
	m_NumThresh->minimum (0);
        m_NumThresh->step (1);
        m_NumThresh->value (1);
        m_NumThresh->callback ((Fl_Callback*)cb_NumThresh);
	m_NumGroup->add (m_NumThresh);

        m_NumAttack = new Fl_Counter (72, 35, 60, 20, "Attack (s)");
	m_NumAttack->user_data ((void*)(this));
	m_NumAttack->type (FL_SIMPLE_COUNTER);
	m_NumAttack->color (Info->GUI_COLOUR);
        m_NumAttack->box (FL_PLASTIC_UP_BOX);
        m_NumAttack->labelsize (8);
        m_NumAttack->maximum (TIMED_SLIDER_MAX * TIMED_SLIDER_MAX);
        m_NumAttack->minimum (0);
        m_NumAttack->step (0.1);
        m_NumAttack->value (0);
        m_NumAttack->callback ((Fl_Callback*)cb_NumAttack);
	m_NumGroup->add (m_NumAttack);

	m_NumDecay = new Fl_Counter (10, 65, 60, 20, "Decay (s)");
	m_NumDecay->user_data ((void*)(this));
	m_NumDecay->type (FL_SIMPLE_COUNTER);
	m_NumDecay->color (Info->GUI_COLOUR);
	m_NumDecay->box (FL_PLASTIC_UP_BOX);
        m_NumDecay->labelsize (8);
	m_NumDecay->maximum (TIMED_SLIDER_MAX * TIMED_SLIDER_MAX);
        m_NumDecay->minimum (0);
        m_NumDecay->step (0.1);
        m_NumDecay->value (0.5);
        m_NumDecay->callback ((Fl_Callback*)cb_NumDecay);
	m_NumGroup->add (m_NumDecay);

        m_NumSustain = new Fl_Counter (72, 65, 60, 20, "Sustain (%)");
	m_NumSustain->user_data ((void*)(this));
	m_NumSustain->type (FL_SIMPLE_COUNTER);
	m_NumSustain->color (Info->GUI_COLOUR);
	m_NumSustain->box (FL_PLASTIC_UP_BOX);
        m_NumSustain->labelsize (8);
        m_NumSustain->maximum (100);
        m_NumSustain->minimum (0);
        m_NumSustain->step (1);
        m_NumSustain->value (100);
        m_NumSustain->callback ((Fl_Callback*)cb_NumSustain);
	m_NumGroup->add (m_NumSustain);

	m_NumRelease = new Fl_Counter (10, 95, 60, 20, "Release (s)");
	m_NumRelease->user_data ((void*)(this));
	m_NumRelease->type (FL_SIMPLE_COUNTER);
	m_NumRelease->color (Info->GUI_COLOUR);
	m_NumRelease->box (FL_PLASTIC_UP_BOX);
        m_NumRelease->labelsize (8);
	m_NumRelease->maximum (TIMED_SLIDER_MAX * TIMED_SLIDER_MAX);
        m_NumRelease->minimum (0);
        m_NumRelease->step (0.1);
        m_NumRelease->value (1.0);
        m_NumRelease->callback ((Fl_Callback*)cb_NumRelease);
	m_NumGroup->add (m_NumRelease);

	m_NumVolume = new Fl_Counter (72, 95, 60, 20, "Volume (%)");
	m_NumVolume->user_data ((void*)(this));
	m_NumVolume->type (FL_SIMPLE_COUNTER);
	m_NumVolume->color (Info->GUI_COLOUR);
	m_NumVolume->box (FL_PLASTIC_UP_BOX);
        m_NumVolume->labelsize (8);
        m_NumVolume->maximum (100);
        m_NumVolume->minimum (0);
        m_NumVolume->step (1);
        m_NumVolume->value (50);
        m_NumVolume->callback ((Fl_Callback*)cb_NumVolume);
	m_NumGroup->add (m_NumVolume);

        end();
}

void EnvelopePluginGUI::UpdateValues (SpiralPlugin *o) {
     float val;
     EnvelopePlugin *Plugin = (EnvelopePlugin*)o;

     val = Plugin->GetTrigThresh();
     m_Thresh->value (1.0f - val);
     m_NumThresh->value (100 * val);

     val = Plugin->GetAttack();
     m_Attack->value (TIMED_SLIDER_MAX - sqrt (val));
     m_NumAttack->value (val);

     val = Plugin->GetDecay();
     m_Decay->value (TIMED_SLIDER_MAX - sqrt (val));
     m_NumDecay->value (val);

     val = Plugin->GetSustain();
     m_Sustain->value (1.0f - val);
     m_NumSustain->value (100 * val);

     val = Plugin->GetRelease();
     m_Release->value (TIMED_SLIDER_MAX - sqrt (val));
     m_NumRelease->value (val);

     val = Plugin->GetVolume();
     m_Volume->value (1.0f - val);
     m_NumVolume->value (100 * val);
}

// Slider Callbacks

inline void EnvelopePluginGUI::cb_Thresh_i (Fl_Slider* o, void* v) {
       float val = (float)(1.0f - o->value());
       m_GUICH->Set ("Trig", val);
       m_NumThresh->value (100 * val);
}

void EnvelopePluginGUI::cb_Thresh (Fl_Slider* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_Thresh_i (o, v);
}

inline void EnvelopePluginGUI::cb_Attack_i (Fl_Slider* o, void* v) {
       float val = TIMED_SLIDER_MAX - o->value();
       val *= val;
       m_GUICH->Set ("Attack", val);
       m_NumAttack->value (val);
}

void EnvelopePluginGUI::cb_Attack (Fl_Slider* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_Attack_i (o, v);
}

inline void EnvelopePluginGUI::cb_Decay_i (Fl_Slider* o, void* v) {
       float val = TIMED_SLIDER_MAX - o->value();
       val *= val;
       m_GUICH->Set ("Decay", val);
       m_NumDecay->value (val);
}

void EnvelopePluginGUI::cb_Decay (Fl_Slider* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_Decay_i (o, v);
}

inline void EnvelopePluginGUI::cb_Sustain_i (Fl_Slider* o, void* v) {
       float val = (float)(1.0f - o->value());
       m_GUICH->Set ("Sustain", val);
       m_NumSustain->value (100 * val);
}

void EnvelopePluginGUI::cb_Sustain (Fl_Slider* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_Sustain_i (o, v);
}

inline void EnvelopePluginGUI::cb_Release_i (Fl_Slider* o, void* v) {
       float val = TIMED_SLIDER_MAX - o->value();
       val *= val;
       m_GUICH->Set ("Release", val);
       m_NumRelease->value (val);
}

void EnvelopePluginGUI::cb_Release (Fl_Slider* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_Release_i (o, v);
}

inline void EnvelopePluginGUI::cb_Volume_i (Fl_Slider* o, void* v) {
       float val = (float)(1.0f - o->value());
       m_GUICH->Set ("Volume", val);
       m_NumVolume->value (100 * val);
}

void EnvelopePluginGUI::cb_Volume (Fl_Slider* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_Volume_i (o, v);
}

// Counter Callbacks

inline void EnvelopePluginGUI::cb_NumThresh_i (Fl_Counter* o, void* v) {
       float val = o->value() / 100;
       m_GUICH->Set ("Trig", val);
       m_Thresh->value (1.0f - val);
}

void EnvelopePluginGUI::cb_NumThresh (Fl_Counter* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_NumThresh_i (o, v);
}

inline void EnvelopePluginGUI::cb_NumAttack_i (Fl_Counter* o, void* v) {
       float val = o->value();
       m_GUICH->Set ("Attack", val);
       m_Attack->value (TIMED_SLIDER_MAX - sqrt (val));
}

void EnvelopePluginGUI::cb_NumAttack (Fl_Counter* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_NumAttack_i (o, v);
}

inline void EnvelopePluginGUI::cb_NumDecay_i (Fl_Counter* o, void* v) {
       float val = o->value();
       m_GUICH->Set ("Decay", val);
       m_Decay->value (TIMED_SLIDER_MAX - sqrt (val));
}

void EnvelopePluginGUI::cb_NumDecay (Fl_Counter* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_NumDecay_i (o, v);
}

inline void EnvelopePluginGUI::cb_NumSustain_i (Fl_Counter* o, void* v) {
       float val = o->value() / 100;
       m_GUICH->Set ("Sustain", val);
       m_Sustain->value (1.0f - val);
}

void EnvelopePluginGUI::cb_NumSustain (Fl_Counter* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_NumSustain_i (o, v);
}

inline void EnvelopePluginGUI::cb_NumRelease_i (Fl_Counter* o, void* v) {
       float val = o->value();
       m_GUICH->Set ("Release", val);
       m_Release->value (TIMED_SLIDER_MAX - sqrt (val));
}

void EnvelopePluginGUI::cb_NumRelease (Fl_Counter* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_NumRelease_i (o, v);
}

inline void EnvelopePluginGUI::cb_NumVolume_i (Fl_Counter* o, void* v) {
       float val = o->value() / 100;
       m_GUICH->Set ("Volume", val);
       m_Volume->value (1.0f - val);
}

void EnvelopePluginGUI::cb_NumVolume (Fl_Counter* o, void* v) {
     ((EnvelopePluginGUI*)(o->user_data()))->cb_NumVolume_i (o, v);
}

// Help Text

const string EnvelopePluginGUI::GetHelpText (const string &loc) {
    return string("")
    + "An ADSR envelope. This plugin also has a built in\n"
    + "amplifier for signals, so data fed through the Input\n"
    + "and Output are amplified according to the envelope value.\n"
    + "The envelope is triggered with an input CV, and the output\n"
    + "CV is the value of the envelope at that time. The signal\n"
    + "level needed to trigger the envelope can be set using\n"
    + "the \"T\" slider, also the volume level of the output can\n"
    + "be set with the \"V\" slider.\n\n"
    + "This envelope can be used to mimic audio triggered effects\n"
    + "units, by feeding the audio signal into the trigger CV.\n"
    + "In fact, the envelope plugin itself can be made into a\n"
    + "compressor, by feeding the audio signal into both inputs,\n"
    + "and tweaking the ADSR values. \n";
}
