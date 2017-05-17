/*  SpiralPlugin
 *  Copyleft (C) 2002 Andy Preston <andy@clublinux.co.uk>
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

#include "MeterPluginGUI.h"
#include <stdio.h>

using namespace std;

char label_buf[65];

MeterPluginGUI::~MeterPluginGUI () {
   delete m_Data;
}

MeterPluginGUI::MeterPluginGUI (int w, int h, MeterPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch),
m_Bypass (false)
{
  m_BufSize = Info->BUFSIZE;
  m_Data = new float[m_BufSize];
  // Create the widgets and stuff!
  Bypass = new Fl_Button (2, 18, 54, 20, "Bypass");
  Bypass->labelsize (10);
  Bypass->type (FL_TOGGLE_BUTTON);
  Bypass->box (FL_PLASTIC_UP_BOX);
  Bypass->color (Info->GUI_COLOUR);
  Bypass->selection_color (Info->GUI_COLOUR);
  Bypass->callback ((Fl_Callback*)cb_Bypass);
  add (Bypass);
  VUMode = new Fl_Button (118, 18, 54, 20, "VU");
  VUMode->type (FL_RADIO_BUTTON);
  VUMode->box (FL_PLASTIC_UP_BOX);
  VUMode->color (Info->GUI_COLOUR);
  VUMode->selection_color (Info->GUI_COLOUR);
  VUMode->labelsize (10);
  VUMode->callback ((Fl_Callback*)cb_Mode);
  VUMode->set();
  add (VUMode);
  MMMode = new Fl_Button (174, 18, 54, 20, "Min/Max");
  MMMode->type (FL_RADIO_BUTTON);
  MMMode->box (FL_PLASTIC_UP_BOX);
  MMMode->color (Info->GUI_COLOUR);
  MMMode->selection_color (Info->GUI_COLOUR);
  MMMode->labelsize (10);
  MMMode->callback ((Fl_Callback*)cb_Mode);
  add (MMMode);
  for (int display=0; display<8; display++) {
    Digits[display] = new Fl_SevenSeg ((display*28)+2, 40, 28, 40);
    Digits[display]->bar_width (4);
    Digits[display]->color (Info->SCOPE_FG_COLOUR);
    Digits[display]->color2 (Info->SCOPE_BG_COLOUR);
    add (Digits[display]);
  }
  MinBox = new Fl_Output (2, 104, 84, 20);
  MinBox->box (FL_PLASTIC_DOWN_BOX);
  MinBox->set_output();
  add (MinBox);
  Reset = new Fl_Button (88, 104, 54, 20, "Reset");
  Reset->labelsize (10);
  Reset->type (0);
  Reset->box (FL_PLASTIC_UP_BOX);
  Reset->color (Info->GUI_COLOUR);
  Reset->selection_color (Info->GUI_COLOUR);
  Reset->callback ((Fl_Callback*)cb_Reset);
  add (Reset);
  MaxBox = new Fl_Output (144, 104, 84, 20);
  MaxBox->set_output();
  MaxBox->box (FL_PLASTIC_DOWN_BOX);
  add (MaxBox);
  Meter = new Fl_VU_Meter (2, 82, 226, 20);
  Meter->color (Info->SCOPE_BG_COLOUR);
  Meter->vu_mode (true);
  cb_Reset_i (Reset, NULL);
  end ();
  DoReset ();
}

void MeterPluginGUI::draw() {
  SpiralGUIType::draw ();
  if (! m_Bypass) {
    float datum = 0.0;
    if (m_GUICH->GetBool ("DataReady")) m_GUICH->GetData ("AudioData", m_Data);
    else memset (m_Data, 0, m_BufSize * sizeof (float));
    // The min and max values are based on the whole buffer
    for (int c=0; c<m_BufSize; c++) {
      datum = m_Data[c];
      if (VUMode->value ()) datum = fabs (datum);
      if (datum < m_Min) m_Min = datum;
      if (datum > m_Max) m_Max = datum;
    }
    SetMinMax (m_Min, m_Max);
    // The meter displays the last datum we touched (it's a quick average)
    Meter->value (datum);
    Meter->redraw();
    // Yeuck - have I REALLY used stdio for that - this is supposed to be C++
    //snprintf (label_buf, 64, "%1.5f", *m_Data);
    char* c = label_buf;
    for (int display=0; display<8; display++) {
      Digits[display] -> dp (off);
      if (*c == 0) Digits[display] -> value (0);
      else {
        if (*c == '.') {
          Digits[display] -> dp (point);
          c++;
        }
        int val;
        if (*c == '-') val = -1; else val = (int)*c - (int)'0';
        Digits[display] -> value (val);
        c++;
      }
    }
  }
}

void MeterPluginGUI::Update () {
  redraw();
}

void MeterPluginGUI::UpdateValues (SpiralPlugin* o) {
  MeterPlugin* Plugin = (MeterPlugin*)o;
  VUMode->value (Plugin->GetVUMode ());
  MMMode->value (! Plugin->GetVUMode ());
}

void MeterPluginGUI::SetMinMax (float NewMin, float NewMax) {
  m_Min = NewMin;
  m_Max = NewMax;
  snprintf (label_buf, 64, "%1.5f", m_Min);
  MinBox->value (label_buf);
  snprintf (label_buf, 64, "%1.5f", m_Max);
  MaxBox->value (label_buf);
  if (MMMode->value ()) {
    Meter->minimum (m_Min);
    Meter->maximum (m_Max);
  }
  else {
    Meter->minimum (0.0);
    Meter->maximum (1.0);
    if (m_Max > 1.0) {
      MaxBox->color (FL_RED);
    }
  }
}

void MeterPluginGUI::cb_Bypass_i (Fl_Button* o, void* v) {
  m_Bypass = o->value();
}

void MeterPluginGUI::cb_Bypass (Fl_Button* o, void* v) {
  ((MeterPluginGUI*)(o->parent()))->cb_Bypass_i (o, v);
}

void MeterPluginGUI::DoReset (void) {
  MaxBox->color (MinBox->color());
  SetMinMax (10, -10); // Yes, I know that LOOKS the wrong way round, but it isn't!
}

void MeterPluginGUI::cb_Reset_i (Fl_Button* o, void* v) {
  DoReset ();
}

void MeterPluginGUI::cb_Reset (Fl_Button* o, void* v) {
  ((MeterPluginGUI*)(o->parent()))->cb_Reset_i (o, v);
}

void MeterPluginGUI::cb_Mode_i (Fl_Button* o, void* v) {
  DoReset ();
  if (o==VUMode) m_GUICH->SetCommand (MeterPlugin::SETVU);
  else m_GUICH->SetCommand (MeterPlugin::SETMM);
  Meter->vu_mode (o==VUMode);
}

void MeterPluginGUI::cb_Mode (Fl_Button* o, void* v) {
  ((MeterPluginGUI*)(o->parent()))->cb_Mode_i (o, v);
}

const string MeterPluginGUI::GetHelpText (const string &loc) {
    return string ("")
    + "The Meter lets you see a numeric representation of the\n"
    + "data flowing through it. It does nothing to the signal,\n"
    + "but its very useful for checking the layouts, looking at\n"
    + "CV value etc.\n";
}
