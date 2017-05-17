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

#include "MixSwitchPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

MixSwitchPluginGUI::MixSwitchPluginGUI (int w, int h, MixSwitchPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch)
{
  m_Switch = new Fl_Counter (11, 14, 58, 20, "Select");
  m_Switch->labelsize (10);
  m_Switch->type (FL_SIMPLE_COUNTER);
  m_Switch->box (FL_PLASTIC_UP_BOX);
  m_Switch->color (Info->GUI_COLOUR);
  m_Switch->selection_color (Info->GUI_COLOUR);
  m_Switch->step (1);
  m_Switch->value (1);
  m_Switch->callback ((Fl_Callback*) cb_Switch);
  add (m_Switch);

  m_Chans = new Fl_Counter (15, 50, 50, 15, "Channels");
  m_Chans->labelsize (8);
  m_Chans->textsize (8);
  m_Chans->type (FL_SIMPLE_COUNTER);
  m_Chans->box (FL_PLASTIC_UP_BOX);
  m_Chans->color (Info->GUI_COLOUR);
  m_Chans->selection_color (Info->GUI_COLOUR);
  m_Chans->step (1);
  m_Chans->value (2);
  m_Chans->callback ((Fl_Callback*) cb_Chans);
  add (m_Chans);

  end();
}

inline void MixSwitchPluginGUI::cb_Chans_i (Fl_Counter* o, void* v) {
  if (o->value() < 2) o->value(2);
  else {
    m_GUICH->Set ("Chans", int (o->value ()));
    m_GUICH->SetCommand (MixSwitchPlugin::SETCHANS);
    m_GUICH->Wait ();
    Resize (w(), h());
  }
}

void MixSwitchPluginGUI::cb_Chans (Fl_Counter* o, void* v) {
  ((MixSwitchPluginGUI*) (o->parent ())) -> cb_Chans_i (o, v);
}


inline void MixSwitchPluginGUI::cb_Switch_i (Fl_Counter* o, void* v) {
  if (o->value() < 1 || o->value() > m_Chans->value ())
    o->value (1);
  m_GUICH->Set ("Switch", int (o->value ()));
}

void MixSwitchPluginGUI::cb_Switch (Fl_Counter* o, void* v) {
  ((MixSwitchPluginGUI*) (o->parent ())) -> cb_Switch_i (o, v);
}

void MixSwitchPluginGUI::Update () {
  if (m_GUICH->GetBool ("Auto")) {
    int e = m_GUICH->GetInt ("Echo");
    if (m_Switch->value () != e) m_Switch->value (e);
  }
}

void MixSwitchPluginGUI::UpdateValues (SpiralPlugin *o) {
  MixSwitchPlugin* Plugin = (MixSwitchPlugin*)o;
  m_Chans->value (Plugin->GetChans ());
  m_Switch->value (Plugin->GetSwitch ());
}

const string MixSwitchPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "It's a simple n-input 1-output rotary switch.\n\n"
    + "The 'Channels' parameter controls the number of outputs.\n\n"
    + "The 'Select' parameter selects which input is echoed at the output.\n\n"
    + "The 'CV' input also selects which input is echoed at the output.\n\n"
    + "The 'Clock' input is a trigger to select the next input chanel.\n\n"
    + "The 'CV' output enables you to gang several switches together, by\n"
    + "connecting it to the other switches' 'CV' input.\n";
}
