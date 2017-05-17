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

#include "StreamPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <FL/fl_file_chooser.h>

using namespace std;

////////////////////////////////////////////

StreamPluginGUI::StreamPluginGUI(int w, int h,StreamPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
m_Playing (false),
m_PitchValue (1.0f)
{
        // 7 seg displays
        for (int dis=0; dis<6; dis++) {
            m_Display[dis] = new Fl_SevenSeg (5 + 28*dis, 20, 28, 60);
            m_Display[dis] -> bar_width (4);
            m_Display[dis] -> color (Info->SCOPE_FG_COLOUR);
	    m_Display[dis] -> color2 (Info->SCOPE_BG_COLOUR);
            if (dis > 0 && dis % 2 == 0) m_Display[dis] -> dp (colon);
            add (m_Display[dis]);
        }
        // volume control
        m_Volume = new Fl_Knob (180, 15, 50, 50, "Volume");
        m_Volume->color (Info->GUI_COLOUR);
	m_Volume->type (Fl_Knob::LINELIN);
        m_Volume->labelsize (10);
        m_Volume->maximum (2);
        m_Volume->step (0.001);
        m_Volume->value (1);
	m_Volume->callback ((Fl_Callback*)cb_Volume);
	add (m_Volume);
        // pitch indicator
	m_Pitch = new Fl_Slider (5, 85, 235, 20, "");
        m_Pitch->type (FL_HORIZONTAL);
        m_Pitch->labelsize (10);
	m_Pitch->labelcolor (Info->GUI_COLOUR);
        m_Pitch->label (m_PitchLabel);
	m_Pitch->selection_color (Info->GUI_COLOUR);
        m_Pitch->box (FL_PLASTIC_DOWN_BOX);
	m_Pitch->maximum (20);
        m_Pitch->step (0.001);
        m_Pitch->callback ((Fl_Callback*)cb_Pitch);
        add (m_Pitch);
        // position indicator
	m_Pos = new Fl_Slider (5, 108, 235, 20, "");
	m_Pos->type (FL_HORIZONTAL);
        m_Pos->box (FL_PLASTIC_DOWN_BOX);
	m_Pos->labelcolor (Info->GUI_COLOUR);
	m_Pos->selection_color (Info->GUI_COLOUR);
	m_Pos->maximum (1);
	m_Pos->callback ((Fl_Callback*)cb_Pos);
	add (m_Pos);
        // load btn
        m_Load = new Fl_Button (2, 130, 30, 30, "Load");
        m_Load->labelsize (9);
        m_Load->box (FL_PLASTIC_UP_BOX);
	m_Load->color (Info->GUI_COLOUR);
	m_Load->selection_color (Info->GUI_COLOUR);
	m_Load->callback ((Fl_Callback*)cb_Load);
	add (m_Load);
        // reset btn
	m_ToStart = new Fl_Button (32, 130, 30, 30, "@|<");
        m_ToStart->labelsize (10);
	m_ToStart->labeltype (FL_SYMBOL_LABEL);
        m_ToStart->box (FL_PLASTIC_UP_BOX);
	m_ToStart->color (Info->GUI_COLOUR);
	m_ToStart->selection_color (Info->GUI_COLOUR);
	m_ToStart->callback ((Fl_Callback*)cb_ToStart);
	add (m_ToStart);
        // play btn
	m_Play = new Fl_Button (62, 130, 30, 30, "@>");
        m_Play->labelsize (10);
	m_Play->labeltype (FL_SYMBOL_LABEL);
        m_Play->box (FL_PLASTIC_UP_BOX);
	m_Play->color (Info->GUI_COLOUR);
	m_Play->selection_color (Info->GUI_COLOUR);
	m_Play->callback ((Fl_Callback*)cb_Play);
	add (m_Play);
        // normal speed btn
	m_Reset = new Fl_Button (92, 130, 30, 30, "Reset");
        m_Reset->labelsize (9);
        m_Reset->box (FL_PLASTIC_UP_BOX);
	m_Reset->color (Info->GUI_COLOUR);
	m_Reset->selection_color (Info->GUI_COLOUR);
	m_Reset->callback ((Fl_Callback*)cb_Reset);
	add (m_Reset);
        // Reverse Button
        m_Rev = new  Fl_Button (122, 130, 30, 30, "@<-");
        m_Rev->labelsize (10);
	m_Rev->labeltype (FL_SYMBOL_LABEL);
        m_Rev->box (FL_PLASTIC_UP_BOX);
	m_Rev->color (Info->GUI_COLOUR);
	m_Rev->selection_color (Info->GUI_COLOUR);
	m_Rev->callback ((Fl_Callback*)cb_Rev);
	add (m_Rev);
        // 1/2 speed btn
	m_Div = new Fl_Button (152, 130, 30, 30, "/2");
        m_Div->labelsize (9);
        m_Div->box (FL_PLASTIC_UP_BOX);
	m_Div->color (Info->GUI_COLOUR);
	m_Div->selection_color (Info->GUI_COLOUR);
	m_Div->callback ((Fl_Callback*)cb_Div);
	add (m_Div);
        // dbl speed btn
	m_Dbl = new Fl_Button (182, 130, 30, 30, "X2");
        m_Dbl->labelsize (9);
        m_Dbl->box (FL_PLASTIC_UP_BOX);
	m_Dbl->color (Info->GUI_COLOUR);
	m_Dbl->selection_color (Info->GUI_COLOUR);
	m_Dbl->callback ((Fl_Callback*)cb_Dbl);
	add (m_Dbl);
        // nudge btn
	m_Nudge = new Fl_Repeat_Button (212, 130, 30, 30, "Nudge");
        m_Nudge->labelsize (9);
        m_Nudge->box (FL_PLASTIC_UP_BOX);
       	m_Nudge->color (Info->GUI_COLOUR);
	m_Nudge->selection_color (Info->GUI_COLOUR);
        m_Nudge->callback ((Fl_Callback*)cb_Nudge);
	add (m_Nudge);
        end ();
        UpdatePitch (true, false, false);
}

StreamPluginGUI::~StreamPluginGUI () {
}

// Update signalled from plugin

void StreamPluginGUI::Update() {
     float t=m_GUICH->GetFloat ("TimeOut");
     m_Pos->value (t);
     m_Display[5]->value ((int)(t*100) % 10);
     m_Display[4]->value ((int)(t*10) % 10);
     m_Display[3]->value ((int)t % 10);
     m_Display[2]->value ((int)(t/10) % 6);
     m_Display[1]->value ((int)(t/60) % 10);
     m_Display[0]->value ((int)(t/600) % 10);
     redraw();
     SetMaxTime (m_GUICH->GetFloat ("MaxTime"));
     if (m_Playing != m_GUICH->GetBool ("Playing")) UpdatePlayStatus ();
}

// Update GUI on load

void StreamPluginGUI::UpdateValues (SpiralPlugin *o) {
     StreamPlugin *Plugin = (StreamPlugin*)o;
     m_Volume->value (Plugin->GetVolume());
     m_PitchValue = Plugin->GetPitch();
     UpdatePitch (true, true, false);
}

// volume

inline void StreamPluginGUI::cb_Volume_i (Fl_Knob* o, void* v) {
       m_GUICH->Set ("Volume", (float)o->value ());
}

void StreamPluginGUI::cb_Volume (Fl_Knob* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Volume_i (o, v);
}

// pitch

void StreamPluginGUI::UpdatePitch (bool UpdateIt, bool DrawIt, bool SendIt) {
     if (m_PitchValue<0) {
        m_Pitch->align (FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
        m_Rev->label ("@->");
     }
     else {
        m_Pitch->align (FL_ALIGN_INSIDE | FL_ALIGN_RIGHT);
        m_Rev->label ("@<-");
     }
     sprintf (m_PitchLabel, "    %1.3f   ", m_PitchValue);
     if (UpdateIt) m_Pitch->value (m_PitchValue+10);
     if (DrawIt) redraw();
     if (SendIt) m_GUICH->Set ("Pitch", m_PitchValue);
}

inline void StreamPluginGUI::cb_Pitch_i (Fl_Slider* o, void* v) {
       m_PitchValue=o->value()-10;
       UpdatePitch (false, false);
}

void StreamPluginGUI::cb_Pitch (Fl_Slider* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Pitch_i(o,v);
}

// position

inline void StreamPluginGUI::cb_Pos_i (Fl_Slider* o, void* v) {
       m_GUICH->Set ("Time",(float)o->value());
       m_GUICH->SetCommand (StreamPlugin::SET_TIME);
}

void StreamPluginGUI::cb_Pos (Fl_Slider* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Pos_i (o, v);
}

// load

inline void StreamPluginGUI::cb_Load_i (Fl_Button* o, void* v) {
       char *fn=fl_file_chooser("Load a sample", "{*.wav,*.WAV}", NULL);
       if (fn && fn!='\0') {
          strcpy (m_TextBuf, fn);
	  m_GUICH->SetData ("FileName", (void*)m_TextBuf);
	  m_GUICH->SetCommand (StreamPlugin::LOAD);
	  m_GUICH->Wait();
	  SetMaxTime (m_GUICH->GetFloat ("MaxTime"));
	}
}

void StreamPluginGUI::cb_Load (Fl_Button* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Load_i (o, v);
}

// restart

inline void StreamPluginGUI::cb_ToStart_i (Fl_Button* o, void* v) {
       m_GUICH->SetCommand (StreamPlugin::RESTART);
}

void StreamPluginGUI::cb_ToStart (Fl_Button* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_ToStart_i (o, v);
}

// play

void StreamPluginGUI::UpdatePlayStatus (void) {
       m_Playing = ! m_Playing;
       if (m_Playing) m_Play->label ("@||");
       else m_Play->label ("@>");
}

inline void StreamPluginGUI::cb_Play_i (Fl_Button* o, void* v) {
       if (m_Playing) m_GUICH->SetCommand (StreamPlugin::STOP);
       else m_GUICH->SetCommand (StreamPlugin::PLAY);
}

void StreamPluginGUI::cb_Play (Fl_Button* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Play_i (o, v);
}

// reset

inline void StreamPluginGUI::cb_Reset_i (Fl_Button* o, void* v) {
       m_PitchValue = 1.0f;
       UpdatePitch ();
}

void StreamPluginGUI::cb_Reset (Fl_Button* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Reset_i (o, v);
}

// Rev

inline void StreamPluginGUI::cb_Rev_i (Fl_Button* o, void* v) {
       m_PitchValue = -m_PitchValue;
       UpdatePitch ();
}

void StreamPluginGUI::cb_Rev (Fl_Button* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Rev_i (o, v);
}

// div 2

inline void StreamPluginGUI::cb_Div_i (Fl_Button* o, void* v) {
       m_PitchValue /= 2.0f;
       UpdatePitch ();
}

void StreamPluginGUI::cb_Div (Fl_Button* o, void* v) {
     ((StreamPluginGUI*)(o->parent()))->cb_Div_i (o, v);
}

// mul 2

inline void StreamPluginGUI::cb_Dbl_i (Fl_Button* o, void* v) {
       m_PitchValue *= 2.0f;
       UpdatePitch ();
}

void StreamPluginGUI::cb_Dbl (Fl_Button* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Dbl_i (o, v);
}

//nudge

inline void StreamPluginGUI::cb_Nudge_i (Fl_Button* o, void* v) {
       int NudgeSize = 1;
       if (m_PitchValue < 0) NudgeSize = -NudgeSize;
       float P = m_Pos->value() + NudgeSize;
       if (P < 0) P = 0;
       if (P > m_Pos->maximum()) P = m_Pos->maximum();
       m_Pos->value (P);
       m_GUICH->Set ("Time", P);
       m_GUICH->SetCommand (StreamPlugin::SET_TIME);
}

void StreamPluginGUI::cb_Nudge (Fl_Button* o, void* v) {
     ((StreamPluginGUI*)(o->parent ()))->cb_Nudge_i (o, v);
}

// help text

const string StreamPluginGUI::GetHelpText (const string &loc){
    return string("")
	+ "If you want to mix whole tracks and add effects etc, then this is the\n"
	+ "way to do it. The StreamPlugin loads a wav in bit by bit, so it doesn't\n"
	+ "use much memory. The track can be pitched for mixing.\n"
	+ "Operates pretty much like a media player such as XMMS (only wav\n"
	+ "format though).\n\n"
        + "Connect the finish trigger to the stop trigger to play the wav only\nonce.\n\n"
	+ "Note: Not realtime safe, if you're using JACK, use a client such as\n"
	+ "alsaplayer.";
}

