/*  SpiralPlugin
 *  Copyleft (C) 2002 Andy Preston <andy@clubunix.co.uk>
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

#include "LFOPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>
#include "../GUI/WS_Sine.h"
#include "../GUI/WS_Tri.h"
#include "../GUI/WS_Square.h"
#include "../GUI/WS_Saw.h"

using namespace std;

LFOPluginGUI::LFOPluginGUI (int w, int h, LFOPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch),
m_PixmapSine (image_Sine),
m_PixmapTri (image_Tri),
m_PixmapSquare (image_Square),
m_PixmapSaw (image_Saw) {

        m_ShapeSine = new Fl_LED_Button (0, 10, 23, 23);
        m_ShapeSine->type (FL_RADIO_BUTTON);
        m_PixmapSine.label (m_ShapeSine);
	m_ShapeSine->set();
        m_ShapeSine->callback ((Fl_Callback*)cb_Sine);

	m_ShapeTri = new Fl_LED_Button (0, 31, 23, 23);
        m_ShapeTri->type (FL_RADIO_BUTTON);
        m_PixmapTri.label (m_ShapeTri);
        m_ShapeTri->callback ((Fl_Callback*)cb_Tri);

        m_ShapeSquare = new Fl_LED_Button (0, 53, 23, 23);
        m_ShapeSquare->type (FL_RADIO_BUTTON);
        m_PixmapSquare.label (m_ShapeSquare);
	m_ShapeSquare->callback ((Fl_Callback*)cb_Square);

        m_ShapeSaw = new Fl_LED_Button (0, 74, 23, 23);
        m_ShapeSaw->type (FL_RADIO_BUTTON);
        m_PixmapSaw.label (m_ShapeSaw);
        m_ShapeSaw->callback ((Fl_Callback*)cb_Saw);

       	m_TheTabs = new Fl_Tabs (50, 10, 119, 88, "");
	m_TheTabs->box (FL_PLASTIC_DOWN_BOX);
	m_TheTabs->color (Info->GUI_COLOUR);
	add (m_TheTabs);

	m_CtlGroup = new Fl_Group (50, 24, 119, 64, "Control");
	m_CtlGroup->labelsize (10);
        m_TheTabs->add (m_CtlGroup);

        m_Perd = new Fl_Knob (85, 30, 50, 50, "Period");
        m_Perd->user_data ((void*)(this));
        m_Perd->color (Info->GUI_COLOUR);
	m_Perd->type (Fl_Knob::LINELIN);
        m_Perd->labelsize (10);
        m_Perd->scaleticks (20);
        m_Perd->maximum (120);
        m_Perd->minimum (0.1);
        m_Perd->step (0.001);
        m_Perd->value (10);
	m_Perd->callback ((Fl_Callback*)cb_Perd);
        m_CtlGroup->add (m_Perd);

        m_NumGroup = new Fl_Group (50, 24, 119, 64, "Numbers");
	m_NumGroup->labelsize (10);
        m_TheTabs->add (m_NumGroup);

        m_NumPerd = new Fl_Counter (55, 30, 110, 20, "Period (Secs)");
        m_NumPerd->user_data ((void*)(this));
        m_NumPerd->textsize (10);
        m_NumPerd->labelsize (10);
        m_NumPerd->box (FL_PLASTIC_UP_BOX);
        m_NumPerd->color (Info->GUI_COLOUR);
        m_NumPerd->value (m_Perd->value());
        m_NumPerd->step (m_Perd->step());
        m_NumPerd->lstep (0.1);
        m_NumPerd->maximum (m_Perd->maximum());
        m_NumPerd->minimum (m_Perd->minimum());
        m_NumPerd->callback ((Fl_Callback*)cb_NumPerd);
        m_NumGroup->add (m_NumPerd);

        m_NumFreq = new Fl_Counter (55, 64, 110, 20, "Freq (Hz)");
        m_NumFreq->user_data ((void*)(this));
        m_NumFreq->textsize (10);
        m_NumFreq->labelsize (10);
        m_NumFreq->box (FL_PLASTIC_UP_BOX);
        m_NumFreq->color (Info->GUI_COLOUR);
        m_NumFreq->value (1 / m_Perd->value());
        m_NumFreq->step (0.001);
        m_NumFreq->lstep (0.1);
        m_NumFreq->maximum (1 / m_Perd->minimum());
        m_NumFreq->minimum (1 / m_Perd->maximum());
        m_NumFreq->callback ((Fl_Callback*)cb_NumFreq);
        m_NumGroup->add (m_NumFreq);

        end();
}

void LFOPluginGUI::UpdateValues (SpiralPlugin *o) {

     LFOPlugin* Plugin = (LFOPlugin*)o;
     m_ShapeSine->value (0);
     m_ShapeTri->value (0);
     m_ShapeSquare->value (0);
     m_ShapeSaw->value (0);
     switch (Plugin->GetType()) {
            case LFOPlugin::SINE : m_ShapeSine->value (1); break;
            case LFOPlugin::TRIANGLE : m_ShapeTri->value (1); break;
            case LFOPlugin::SQUARE : m_ShapeSquare->value (1); break;
            case LFOPlugin::SAW : m_ShapeSaw->value (1); break;
     }
     float x = Plugin->GetFreq();
     m_NumFreq->value (x);
     x = 1 / x;
     m_Perd->value (x);
     m_NumPerd->value (x);
}

// Callbacks for Knob

inline void LFOPluginGUI::cb_Perd_i (Fl_Knob* o, void* v) {
       m_NumPerd->value ((double)o->value());
       float f = 1.0f / o->value();
       m_NumFreq->value (f);
       m_GUICH->Set ("Freq", f);
}

void LFOPluginGUI::cb_Perd (Fl_Knob* o, void* v) {
     ((LFOPluginGUI*)(o->user_data()))->cb_Perd_i (o, v);
}

// Callbacks for Counters

inline void LFOPluginGUI::cb_NumPerd_i (Fl_Knob* o, void* v) {
       m_Perd->value (o->value());
       float f = 1.0f / o->value();
       m_NumFreq->value (f);
       m_GUICH->Set ("Freq", f);
}

void LFOPluginGUI::cb_NumPerd (Fl_Knob* o, void* v) {
     ((LFOPluginGUI*)(o->user_data()))->cb_NumPerd_i (o, v);
}

inline void LFOPluginGUI::cb_NumFreq_i (Fl_Knob* o, void* v) {
       float p = 1.0f / o->value();
       m_Perd->value (p);
       m_NumPerd->value (p);
       m_GUICH->Set ("Freq", (float)(o->value()));
}

void LFOPluginGUI::cb_NumFreq (Fl_Knob* o, void* v) {
     ((LFOPluginGUI*)(o->user_data()))->cb_NumFreq_i (o, v);
}

// Callbacks for waveform buttons

inline void LFOPluginGUI::cb_Sine_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set("Type", LFOPlugin::SINE);
}
void LFOPluginGUI::cb_Sine(Fl_LED_Button* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Sine_i (o, v);
}


inline void LFOPluginGUI::cb_Tri_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set("Type", LFOPlugin::TRIANGLE);
}
void LFOPluginGUI::cb_Tri (Fl_LED_Button* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Tri_i (o, v);
}


inline void LFOPluginGUI::cb_Square_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set("Type", LFOPlugin::SQUARE);
}
void LFOPluginGUI::cb_Square (Fl_LED_Button* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Square_i (o, v);
}


inline void LFOPluginGUI::cb_Saw_i (Fl_LED_Button* o, void* v) {
       m_GUICH->Set("Type", LFOPlugin::SAW);
}
void LFOPluginGUI::cb_Saw (Fl_LED_Button* o, void* v) {
     ((LFOPluginGUI*)(o->parent()))->cb_Saw_i (o, v);
}

const string LFOPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "A very low frequency oscillator. Also produces inverted and \"cosine\"\n"
	+ "outputs (output phase shifted by 180 degrees)\n"
	+ "Period length goes down to 2 minutes :]\n";
}
