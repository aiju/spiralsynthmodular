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

#include "OscillatorPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>
#include "../GUI/WS_Square.h"
#include "../GUI/WS_Noise.h"
#include "../GUI/WS_Saw.h"

using namespace std;

OscillatorPluginGUI::OscillatorPluginGUI (int w, int h, OscillatorPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch),
m_PixmapSquare (image_Square),
m_PixmapNoise (image_Noise),
m_PixmapSaw (image_Saw),
m_FineFreq (0)
{
        m_ShapeSquare = new Fl_LED_Button (2, 24, 23, 23);
        m_ShapeSquare->type (FL_RADIO_BUTTON);
        m_PixmapSquare.label (m_ShapeSquare);
        m_ShapeSquare->set();
        m_ShapeSquare->callback ((Fl_Callback*)cb_Square);
        add (m_ShapeSquare);

	m_ShapeSaw = new Fl_LED_Button (2, 58, 23, 23);
        m_ShapeSaw->type (FL_RADIO_BUTTON);
        m_PixmapSaw.label (m_ShapeSaw);
        m_ShapeSaw->callback ((Fl_Callback*)cb_Saw);
        add (m_ShapeSaw);

        m_ShapeNoise = new Fl_LED_Button (2, 92, 23, 23);
        m_ShapeNoise->type (FL_RADIO_BUTTON);
        m_PixmapNoise.label (m_ShapeNoise);
        m_ShapeNoise->callback ((Fl_Callback*)cb_Noise);
        add (m_ShapeNoise);

        m_Tabs = new Fl_Tabs (44, 10, 168, 124);
	m_Tabs->box (FL_PLASTIC_DOWN_BOX);
	m_Tabs->color (Info->GUI_COLOUR);
        add (m_Tabs);

	m_CtlGroup = new Fl_Group (44, 24, 168, 110, "Control");
	m_CtlGroup->labelsize (10);
        m_Tabs->add (m_CtlGroup);

        m_PulseWidth = new Fl_Slider (50, 32, 20, 86, "PW");
	m_PulseWidth->user_data ((void*)(this));
	m_PulseWidth->type (FL_VERT_NICE_SLIDER);
	m_PulseWidth->box (FL_PLASTIC_DOWN_BOX);
	m_PulseWidth->selection_color (Info->GUI_COLOUR);
        m_PulseWidth->labelsize (10);
	m_PulseWidth->maximum (1);
        m_PulseWidth->step (0.01);
        m_PulseWidth->value (0.5);
        m_PulseWidth->callback ((Fl_Callback*)cb_PulseWidth);
	m_CtlGroup->add (m_PulseWidth);

	m_SHLen = new Fl_Slider (74, 32, 20, 86, "SH");
	m_SHLen->user_data ((void*)(this));
	m_SHLen->type (FL_VERT_NICE_SLIDER);
	m_SHLen->box (FL_PLASTIC_DOWN_BOX);
	m_SHLen->selection_color (Info->GUI_COLOUR);
        m_SHLen->labelsize (10);
	m_SHLen->maximum (0.2);
        m_SHLen->step (0.001);
        m_SHLen->value (0.1);
        m_SHLen->callback ((Fl_Callback*)cb_SHLen);
	m_CtlGroup->add (m_SHLen);

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

        m_NumGroup = new Fl_Group (44, 24, 168, 110, "Numbers");
        m_NumGroup->labelsize (10);
        m_Tabs->add (m_NumGroup);

        m_NumPulseWidth = new Fl_Counter (54, 32, 55, 20, "PW (%)");
        m_NumPulseWidth->user_data ((void*)(this));
        m_NumPulseWidth->type (FL_SIMPLE_COUNTER);
        m_NumPulseWidth->box (FL_PLASTIC_UP_BOX);
        m_NumPulseWidth->color (Info->GUI_COLOUR);
        m_NumPulseWidth->labelsize (10);
        m_NumPulseWidth->textsize (10);
        m_NumPulseWidth->minimum (0);
        m_NumPulseWidth->maximum (100);
        m_NumPulseWidth->step (1);
        m_NumPulseWidth->value (m_PulseWidth->value() * 100);
        m_NumPulseWidth->callback ((Fl_Callback*)cb_NumPulseWidth);
        m_NumGroup->add (m_NumPulseWidth);

        m_NumSHLen = new Fl_Counter (54, 98, 55, 20, "S&H (Secs)");
        m_NumSHLen->user_data ((void*)(this));
        m_NumSHLen->type (FL_SIMPLE_COUNTER);
        m_NumSHLen->box (FL_PLASTIC_UP_BOX);
        m_NumSHLen->color (Info->GUI_COLOUR);
        m_NumSHLen->labelsize (10);
        m_NumSHLen->textsize (10);
        m_NumSHLen->minimum (0);
        m_NumSHLen->maximum (0.2f);
        m_NumSHLen->step (.001);
        //m_NumSHLen->lstep (.1);
        m_NumSHLen->value (0.2f - m_SHLen->value());
        m_NumSHLen->callback ((Fl_Callback*)cb_NumSHLen);
        m_NumGroup->add (m_NumSHLen);

        m_NumFreq = new Fl_Counter (114, 32, 90, 20, "Freq (Hz)");
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

        m_NumOctave = new Fl_Counter (128, 66, 64, 20, "Octave");
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

        m_NumModAmount = new Fl_Counter (128, 98, 64, 20, "Mod (%)");
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

void OscillatorPluginGUI::UpdateValues (SpiralPlugin *o) {
     float val;
     OscillatorPlugin *Plugin = (OscillatorPlugin*)o;

     m_ShapeSquare->value(0);
     m_ShapeNoise->value(0);
     m_ShapeSaw->value(0);
     switch (Plugin->GetType()) {
            case OscillatorPlugin::SQUARE : m_ShapeSquare->value(1); break;
            case OscillatorPlugin::NOISE  : m_ShapeNoise->value(1); break;
            case OscillatorPlugin::SAW    : m_ShapeSaw->value(1); break;
            case OscillatorPlugin::NONE   : break;
     }

     val = Plugin->GetPulseWidth();
     m_PulseWidth->value (val);
     m_NumPulseWidth->value (val * 100);

     val = 0.2f - Plugin->GetSHLen();
     m_SHLen->value (val);
     m_NumSHLen->value (val);

     val = Plugin->GetModAmount();
     m_ModAmount->value (val);
     m_NumModAmount->value (val * 100);

     m_NumOctave->value (Plugin->GetOctave());
     m_Octave->value ((int)m_NumOctave->value() + 3);
     m_FineTune->value (sqrt (Plugin->GetFineFreq()));
     UpdateFreq();
}

// Utility functions

float OscillatorPluginGUI::CalcFineFreq (float Fine) {
      int oct = (int)m_NumOctave->value();
      m_FineFreq = Fine * Fine;
      float fr = 110.0f * m_FineFreq;
      if (oct > 0) fr *= 1 << oct;
      if (oct < 0) fr /= 1 << (-oct);
      return fr;
}

void OscillatorPluginGUI::UpdateFreq (void) {
     // always do max first - CalcFineFreq leaves a value in m_FineFreq
     m_NumFreq->maximum (CalcFineFreq (m_FineTune->maximum()));
     m_NumFreq->value (CalcFineFreq (m_FineTune->value()));
}

// Callbacks - Controls outside the tabs

inline void OscillatorPluginGUI::cb_Square_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)OscillatorPlugin::SQUARE);
}

void OscillatorPluginGUI::cb_Square (Fl_LED_Button* o, void* v) {
     ((OscillatorPluginGUI*)(o->parent()))->cb_Square_i (o, v);
}

inline void OscillatorPluginGUI::cb_Saw_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type", (char)OscillatorPlugin::SAW);
}

void OscillatorPluginGUI::cb_Saw (Fl_LED_Button* o, void* v) {
     ((OscillatorPluginGUI*)(o->parent()))->cb_Saw_i (o, v);
}

inline void OscillatorPluginGUI::cb_Noise_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set ("Type",(char)OscillatorPlugin::NOISE);
}

void OscillatorPluginGUI::cb_Noise (Fl_LED_Button* o, void* v) {
     ((OscillatorPluginGUI*)(o->parent()))->cb_Noise_i (o, v);
}

// Callbacks - Control Group

inline void OscillatorPluginGUI::cb_PulseWidth_i (Fl_Slider* o, void* v)  {
       float val = o->value();
       m_GUICH->Set ("PulseWidth", val);
       m_NumPulseWidth->value (val * 100);
}

void OscillatorPluginGUI::cb_PulseWidth (Fl_Slider* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_PulseWidth_i (o, v);
}

inline void OscillatorPluginGUI::cb_SHLen_i (Fl_Slider* o, void* v) {
       float val = 0.2f - o->value();
       m_GUICH->Set ("SHLen", val);
       m_NumSHLen->value (val);
}

void OscillatorPluginGUI::cb_SHLen (Fl_Slider* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_SHLen_i (o, v);
}

inline void OscillatorPluginGUI::cb_FineTune_i (Fl_Knob* o, void* v) {
       UpdateFreq();
       m_GUICH->Set ("FineFreq", m_FineFreq);
}

void OscillatorPluginGUI::cb_FineTune (Fl_Knob* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_FineTune_i (o, v);
}

inline void OscillatorPluginGUI::cb_Reset_i (Fl_Button* o, void* v) {
       m_FineTune->value (1);
       UpdateFreq();
       m_GUICH->Set ("FineFreq", m_FineFreq);
}

void OscillatorPluginGUI::cb_Reset (Fl_Button* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_Reset_i (o, v);
}

inline void OscillatorPluginGUI::cb_Octave_i (Fl_Knob* o, void* v) {
       m_NumOctave->value ((int)m_Octave->value() - 3);
       UpdateFreq();
       m_GUICH->Set ("Octave", (int)m_NumOctave->value ());
}

void OscillatorPluginGUI::cb_Octave (Fl_Knob* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_Octave_i (o, v);
}

inline void OscillatorPluginGUI::cb_ModAmount_i (Fl_Knob* o, void* v) {
       float val = o->value();
       m_GUICH->Set ("ModAmount", val);
       m_NumModAmount->value (val * 100);
}

void OscillatorPluginGUI::cb_ModAmount (Fl_Knob* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_ModAmount_i (o, v);
}

// Callbacks - Number Group

inline void OscillatorPluginGUI::cb_NumPulseWidth_i (Fl_Counter* o, void* v) {
       float val = o->value() / 100;
       m_GUICH->Set ("PulseWidth", val);
       m_PulseWidth->value (val);
}

void OscillatorPluginGUI::cb_NumPulseWidth (Fl_Counter* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_NumPulseWidth_i (o, v);
}

inline void OscillatorPluginGUI::cb_NumSHLen_i (Fl_Counter* o, void* v) {
       float val = o->value();
       m_GUICH->Set ("SHLen", val);
       m_SHLen->value (0.2f - val);
}

void OscillatorPluginGUI::cb_NumSHLen (Fl_Counter* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_NumSHLen_i (o, v);
}

inline void OscillatorPluginGUI::cb_NumFreq_i (Fl_Counter* o, void* v) {
       int oct = (int)m_NumOctave->value();
       float fr = o->value ();
       if (oct > 0) fr /= 1 << oct;
       if (oct < 0) fr *= 1 << (-oct);
       m_FineFreq = fr / 110.0f;
       m_GUICH->Set ("FineFreq", m_FineFreq);
       m_FineTune->value (sqrt (m_FineFreq));
}

void OscillatorPluginGUI::cb_NumOctave (Fl_Counter* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_NumOctave_i (o, v);
}

inline void OscillatorPluginGUI::cb_NumOctave_i (Fl_Counter* o, void* v) {
       m_Octave->value ((int)m_NumOctave->value() + 3);
       UpdateFreq();
       m_GUICH->Set ("Octave", (int)m_NumOctave->value ());
}

void OscillatorPluginGUI::cb_NumFreq (Fl_Counter* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_NumFreq_i (o, v);
}

inline void OscillatorPluginGUI::cb_NumModAmount_i (Fl_Counter* o, void* v) {
       float val = o->value() / 100;
       m_GUICH->Set ("ModAmount", val);
       m_ModAmount->value (val);
}

void OscillatorPluginGUI::cb_NumModAmount (Fl_Counter* o, void* v) {
     ((OscillatorPluginGUI*)(o->user_data()))->cb_NumModAmount_i (o, v);
}

// Help Text

const string OscillatorPluginGUI::GetHelpText(const string &loc){
    return string("")
	+ "The Oscillator generates raw waveforms from CV controls. Three wave \n"
	+ "shapes are included, Square wave, Triangle wave and white noise.\n\n"
	+ "In the square and triangle shapes, the Frequency CV controls the pitch \n"
	+ "of the signal generated, and the pulse-width turns the squarewave into \n"
	+ "a pulse wave of varying harmonics, and the triangle wave into a sawtooth,\n"
	+ "or reverse sawtooth wave.\n\n"
	+ "The sample & hold CV changes the time between samples with the white noise.\n"
	+ "This is usful for making the Oscillator into a random CV generator.\n\n"
	+ "The plugin window allows you to select the wave shape, set the octave and\n"
	+ "fine tune the frequency (use the reset button to reset the frequency back\n"
        + "to it's default value). There are also controls to set the pulse-width,\n"
	+ "sample and hold manually, and control the modulation depth of the input CV's.\n\n"
	+ "The frequency can be set extremely low on this oscillator, so you can use\n"
	+ "it as an LFO for controlling other plugins.";
}
