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

#include "WaveTablePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include "../GUI/WS_Square.h"
#include "../GUI/WS_Saw.h"
#include "../GUI/WS_RevSaw.h"
#include "../GUI/WS_Tri.h"
#include "../GUI/WS_Sine.h"
#include "../GUI/WS_Pulse1.h"
#include "../GUI/WS_Pulse2.h"
#include "../GUI/WS_InvSine.h"

using namespace std;

WaveTablePluginGUI::WaveTablePluginGUI (int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch),
m_PixmapSine (image_Sine),
m_PixmapSquare (image_Square),
m_PixmapSaw (image_Saw),
m_PixmapRevSaw (image_RevSaw),
m_PixmapTri (image_Tri),
m_PixmapPulse1 (image_Pulse1),
m_PixmapPulse2 (image_Pulse2),
m_PixmapInvSine (image_InvSine),
m_FineFreq (0)
{
	m_ShapeSine = new Fl_LED_Button (2, 15, 23, 23);
        m_ShapeSine->type (FL_RADIO_BUTTON);
        m_PixmapSine.label (m_ShapeSine);
	m_ShapeSine->set();
        m_ShapeSine->callback ((Fl_Callback*)cb_Sine);
        add (m_ShapeSine);

        m_ShapeSquare = new Fl_LED_Button (2, 45, 23, 23);
        m_ShapeSquare->type (FL_RADIO_BUTTON);
        m_PixmapSquare.label (m_ShapeSquare);
	m_ShapeSquare->callback ((Fl_Callback*)cb_Square);
	add (m_ShapeSquare);

	m_ShapeSaw = new Fl_LED_Button (2, 75, 23, 23);
        m_ShapeSaw->type (FL_RADIO_BUTTON);
        m_PixmapSaw.label (m_ShapeSaw);
        m_ShapeSaw->callback ((Fl_Callback*)cb_Saw);
        add (m_ShapeSaw);

	m_ShapeRevSaw = new Fl_LED_Button (2, 105, 23, 23);
        m_ShapeRevSaw->type (FL_RADIO_BUTTON);
        m_PixmapRevSaw.label (m_ShapeRevSaw);
        m_ShapeRevSaw->callback ((Fl_Callback*)cb_RevSaw);
        add (m_ShapeRevSaw);

	m_ShapeTri = new Fl_LED_Button(44, 15, 23, 23);
        m_ShapeTri->type (FL_RADIO_BUTTON);
        m_PixmapTri.label (m_ShapeTri);
        m_ShapeTri->callback ((Fl_Callback*)cb_Tri);
        add (m_ShapeTri);

        m_ShapePulse1 = new Fl_LED_Button (44, 45, 23, 23);
        m_ShapePulse1->type (FL_RADIO_BUTTON);
        m_PixmapPulse1.label (m_ShapePulse1);
	m_ShapePulse1->callback ((Fl_Callback*)cb_Pulse1);
	add (m_ShapePulse1);

	m_ShapePulse2 = new Fl_LED_Button (44, 75, 23, 23);
        m_ShapePulse2->type (FL_RADIO_BUTTON);
        m_PixmapPulse2.label (m_ShapePulse2);
	m_ShapePulse2->callback ((Fl_Callback*)cb_Pulse2);
	add (m_ShapePulse2);

	m_ShapeInvSine = new Fl_LED_Button (44, 105, 23, 23);
        m_ShapeInvSine->type (FL_RADIO_BUTTON);
        m_PixmapInvSine.label (m_ShapeInvSine);
	m_ShapeInvSine->callback ((Fl_Callback*)cb_InvSine);
	add (m_ShapeInvSine);

        m_Tabs = new Fl_Tabs (88, 10, 124, 124);
	m_Tabs->box (FL_PLASTIC_DOWN_BOX);
	m_Tabs->color (Info->GUI_COLOUR);
        add (m_Tabs);

	m_CtlGroup = new Fl_Group (88, 24, 124, 110, "Control");
	m_CtlGroup->labelsize (10);
        m_Tabs->add (m_CtlGroup);

        m_FineTune = new Fl_Knob (98, 32, 60, 60, "Fine Tune");
        m_FineTune->user_data ((void*)(this));
        m_FineTune->color (Info->GUI_COLOUR);
	m_FineTune->type (Fl_Knob::LINELIN);
        m_FineTune->labelsize (10);
        m_FineTune->scaleticks (20);
        m_FineTune->maximum (1.414);
        m_FineTune->step (0.000001);
        m_FineTune->value (1);
	m_FineTune->callback ((Fl_Callback*)cb_FineTune);
	m_CtlGroup->add (m_FineTune);

        m_Reset = new Fl_Button (108, 106, 40, 20, "Reset");
        m_Reset->user_data ((void*)(this));
        m_Reset->labelsize (10);
        m_Reset->box (FL_PLASTIC_UP_BOX);
        m_Reset->color (Info->GUI_COLOUR);
        m_Reset->selection_color (Info->GUI_COLOUR);
        m_Reset->callback ((Fl_Callback*)cb_Reset);
        m_CtlGroup->add (m_Reset);

        m_Octave = new Fl_Knob (160, 27, 40, 40, "Octave");
        m_Octave->user_data ((void*)(this));
        m_Octave->color (Info->GUI_COLOUR);
	m_Octave->type (Fl_Knob::LINELIN);
        m_Octave->labelsize (10);
        m_Octave->maximum (6);
        m_Octave->step (1);
        m_Octave->value (3);
	m_Octave->scaleticks (6);
	m_Octave->cursor (50);
	m_Octave->callback ((Fl_Callback*)cb_Octave);
        m_CtlGroup->add (m_Octave);

	m_ModAmount = new Fl_Knob (160, 79, 40, 40, "Mod Depth");
	m_ModAmount->user_data ((void*)(this));
        m_ModAmount->color (Info->GUI_COLOUR);
	m_ModAmount->type (Fl_Knob::LINELIN);
	m_ModAmount->labelsize (10);
        m_ModAmount->maximum (2.0f);
        m_ModAmount->step (0.001);
        m_ModAmount->value (1.0);
        m_ModAmount->callback ((Fl_Callback*)cb_ModAmount);
	m_CtlGroup->add (m_ModAmount);

	m_NumGroup = new Fl_Group (88, 24, 124, 110, "Numbers");
	m_NumGroup->labelsize (10);
        m_Tabs->add (m_NumGroup);

        m_NumFreq = new Fl_Counter (108, 32, 90, 20, "Freq (Hz)");
        m_NumFreq->user_data ((void*)(this));
        m_NumFreq->box (FL_PLASTIC_UP_BOX);
        m_NumFreq->color (Info->GUI_COLOUR);
        m_NumFreq->labelsize (10);
        m_NumFreq->textsize (10);
        m_NumFreq->minimum (0);
        m_NumFreq->step (0.1);
        m_NumFreq->lstep (10);
        m_NumFreq->callback ((Fl_Callback*)cb_NumFreq);
        m_NumGroup->add (m_NumFreq);

        m_NumOctave = new Fl_Counter (120, 66, 64, 20, "Octave");
        m_NumOctave->user_data ((void*)(this));
        m_NumOctave->type (FL_SIMPLE_COUNTER);
        m_NumOctave->box (FL_PLASTIC_UP_BOX);
        m_NumOctave->color (Info->GUI_COLOUR);
        m_NumOctave->labelsize (10);
        m_NumOctave->textsize (10);
        m_NumOctave->minimum (-3);
        m_NumOctave->maximum (3);
        m_NumOctave->step (1);
        m_NumOctave->value (0);
        m_NumOctave->callback ((Fl_Callback*)cb_NumOctave);
        m_NumGroup->add (m_NumOctave);

        m_NumModAmount = new Fl_Counter (120, 98, 64, 20, "Mod (%)");
        m_NumModAmount->user_data ((void*)(this));
        m_NumModAmount->type (FL_SIMPLE_COUNTER);
        m_NumModAmount->box (FL_PLASTIC_UP_BOX);
        m_NumModAmount->color (Info->GUI_COLOUR);
        m_NumModAmount->labelsize (10);
        m_NumModAmount->textsize (10);
        m_NumModAmount->minimum (0);
        m_NumModAmount->maximum (200);
        m_NumModAmount->step (1);
        m_NumModAmount->value (m_ModAmount->value() * 100);
        m_NumModAmount->callback ((Fl_Callback*)cb_NumModAmount);
        m_NumGroup->add (m_NumModAmount);

        end();
        UpdateFreq();
}

void WaveTablePluginGUI::UpdateValues (SpiralPlugin *o) {
     float val;
     WaveTablePlugin *Plugin = (WaveTablePlugin*)o;

     m_ShapeSquare->value(0);
     m_ShapeRevSaw->value(0);
     m_ShapeSaw->value(0);
     m_ShapeTri->value(0);
     m_ShapeSine->value(0);
     m_ShapePulse1->value(0);
     m_ShapePulse2->value(0);
     m_ShapeInvSine->value(0);
     switch (Plugin->GetType()) {
            case WaveTablePlugin::SQUARE   : m_ShapeSquare->value(1); break;
            case WaveTablePlugin::SINE     : m_ShapeSine->value(1); break;
            case WaveTablePlugin::SAW      : m_ShapeSaw->value(1); break;
            case WaveTablePlugin::REVSAW   : m_ShapeRevSaw->value(1); break;
            case WaveTablePlugin::TRIANGLE : m_ShapeTri->value(1); break;
            case WaveTablePlugin::PULSE1   : m_ShapePulse1->value(1); break;
            case WaveTablePlugin::PULSE2   : m_ShapePulse2->value(1); break;
            case WaveTablePlugin::INVSINE  : m_ShapeInvSine->value(1); break;
            default  : break;
     }
     val = Plugin->GetModAmount();
     m_ModAmount->value (val);
     m_NumModAmount->value (val * 100);

     m_NumOctave->value (Plugin->GetOctave());
     m_Octave->value ((int)m_NumOctave->value() + 3);
     m_FineTune->value (sqrt (Plugin->GetFineFreq()));
     UpdateFreq();
}

// Utility functions

float WaveTablePluginGUI::CalcFineFreq (float Fine) {
      int oct = (int)m_NumOctave->value();
      m_FineFreq = Fine * Fine;
      float fr = 110.0f * m_FineFreq;
      if (oct > 0) fr *= 1 << oct;
      if (oct < 0) fr /= 1 << (-oct);
      return fr;
}

void WaveTablePluginGUI::UpdateFreq (void) {
     // always do max first - CalcFineFreq leaves a value in m_FineFreq
     m_NumFreq->maximum (CalcFineFreq (m_FineTune->maximum()));
     m_NumFreq->value (CalcFineFreq (m_FineTune->value()));
}

// Callbacks - Controls outside the tabs

inline void WaveTablePluginGUI::cb_Sine_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)WaveTablePlugin::SINE);
}

void WaveTablePluginGUI::cb_Sine (Fl_LED_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->parent()))->cb_Sine_i (o, v);
}

inline void WaveTablePluginGUI::cb_Square_i(Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)WaveTablePlugin::SQUARE);
}

void WaveTablePluginGUI::cb_Square(Fl_LED_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->parent()))->cb_Square_i (o, v);
}

inline void WaveTablePluginGUI::cb_Saw_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)WaveTablePlugin::SAW);
}

void WaveTablePluginGUI::cb_Saw (Fl_LED_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->parent()))->cb_Saw_i (o, v);
}

inline void WaveTablePluginGUI::cb_RevSaw_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)WaveTablePlugin::REVSAW);
}

void WaveTablePluginGUI::cb_RevSaw (Fl_LED_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->parent()))->cb_RevSaw_i (o, v);
}

inline void WaveTablePluginGUI::cb_Tri_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)WaveTablePlugin::TRIANGLE);
}

void WaveTablePluginGUI::cb_Tri (Fl_LED_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->parent()))->cb_Tri_i (o, v);
}

inline void WaveTablePluginGUI::cb_Pulse1_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)WaveTablePlugin::PULSE1);
}

void WaveTablePluginGUI::cb_Pulse1 (Fl_LED_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->parent()))->cb_Pulse1_i (o, v);
}

inline void WaveTablePluginGUI::cb_Pulse2_i(Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)WaveTablePlugin::PULSE2);
}

void WaveTablePluginGUI::cb_Pulse2 (Fl_LED_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->parent()))->cb_Pulse2_i (o, v);
}

inline void WaveTablePluginGUI::cb_InvSine_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)WaveTablePlugin::INVSINE);
}

void WaveTablePluginGUI::cb_InvSine (Fl_LED_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->parent()))->cb_InvSine_i (o, v);
}

// Callbacks - Control Group

inline void WaveTablePluginGUI::cb_FineTune_i(Fl_Knob* o, void* v)
{
       UpdateFreq();
       m_GUICH->Set ("FineFreq", m_FineFreq);
}

void WaveTablePluginGUI::cb_FineTune (Fl_Knob* o, void* v) {
     ((WaveTablePluginGUI*)(o->user_data()))->cb_FineTune_i (o, v);
}

inline void WaveTablePluginGUI::cb_Reset_i (Fl_Button* o, void* v) {
       m_FineTune->value (1);
       UpdateFreq();
       m_GUICH->Set ("FineFreq", m_FineFreq);
}

void WaveTablePluginGUI::cb_Reset (Fl_Button* o, void* v) {
     ((WaveTablePluginGUI*)(o->user_data()))->cb_Reset_i (o, v);
}

inline void WaveTablePluginGUI::cb_Octave_i (Fl_Knob* o, void* v) {
       m_NumOctave->value ((int)m_Octave->value() - 3);
       UpdateFreq();
       m_GUICH->Set ("Octave", (int)m_NumOctave->value ());
}

void WaveTablePluginGUI::cb_Octave (Fl_Knob* o, void* v) {
     ((WaveTablePluginGUI*)(o->user_data()))->cb_Octave_i (o, v);
}

inline void WaveTablePluginGUI::cb_ModAmount_i (Fl_Knob* o, void* v) {
       float val = o->value();
       m_GUICH->Set ("ModAmount", val);
       m_NumModAmount->value (val * 100);
}

void WaveTablePluginGUI::cb_ModAmount (Fl_Knob* o, void* v) {
     ((WaveTablePluginGUI*)(o->user_data()))->cb_ModAmount_i (o, v);
}

// Callbacks - Number Group

inline void WaveTablePluginGUI::cb_NumFreq_i (Fl_Counter* o, void* v) {
       int oct = (int)m_NumOctave->value();
       float fr = o->value ();
       if (oct > 0) fr /= 1 << oct;
       if (oct < 0) fr *= 1 << (-oct);
       m_FineFreq = fr / 110.0f;
       m_GUICH->Set ("FineFreq", m_FineFreq);
       m_FineTune->value (sqrt (m_FineFreq));
}

void WaveTablePluginGUI::cb_NumOctave (Fl_Counter* o, void* v) {
     ((WaveTablePluginGUI*)(o->user_data()))->cb_NumOctave_i (o, v);
}

inline void WaveTablePluginGUI::cb_NumOctave_i (Fl_Counter* o, void* v) {
       m_Octave->value ((int)m_NumOctave->value() + 3);
       UpdateFreq();
       m_GUICH->Set ("Octave", (int)m_NumOctave->value ());
}

void WaveTablePluginGUI::cb_NumFreq (Fl_Counter* o, void* v) {
     ((WaveTablePluginGUI*)(o->user_data()))->cb_NumFreq_i (o, v);
}

inline void WaveTablePluginGUI::cb_NumModAmount_i (Fl_Counter* o, void* v) {
       float val = o->value() / 100;
       m_GUICH->Set ("ModAmount", val);
       m_ModAmount->value (val);
}

void WaveTablePluginGUI::cb_NumModAmount (Fl_Counter* o, void* v) {
     ((WaveTablePluginGUI*)(o->user_data()))->cb_NumModAmount_i (o, v);
}

// Help Text

const string WaveTablePluginGUI::GetHelpText(const string &loc){
    return string("")
	+ "The WaveTable plugin is a fast multifunction oscillator with a variety \n"
	+ "of wave shapes:\n"
	+ "Sine, Square, Saw, Reverse Saw, Triangle, Two pulse shapes and an inverse\n"
	+ "sinewave.\n\n"
	+ "These wave shapes are internally represented as samples, rather than\n" 
	+ "being continually calculated like the conventional oscillator. This \n"
	+ "makes the plugin fast, but restricts the modulations you can do on the\n" 
	+ "wave forms (no pulsewidth).\n\n"
	+ "The oscillator can be pitched very low for use as a LFO CV generator,\n" 
	+ "using any of the supported wave shapes. User wave shapes are planned,\n" 
	+ "so you will be able to load your own samples in.";
}
