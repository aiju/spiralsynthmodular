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

#include "MixerPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

MixerPluginGUI::MixerPluginGUI (int w, int h, MixerPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch)
{
        m_GUIColour = (Fl_Color)Info->GUI_COLOUR;
        for (int n=0; n<MAX_CHANNELS; n++) Numbers[n]=n;
	m_MainPack = new Fl_Pack (0, 15, w, 100);
	m_MainPack->type (FL_HORIZONTAL);
        add (m_MainPack);

        // start with four...
	AddChan(); AddChan(); AddChan(); AddChan();

        m_Buttons = new Fl_Pack (0, 118, 62, 20);
        m_Buttons->type (FL_HORIZONTAL);
        add (m_Buttons);

        m_Delete = new Fl_Button (2, 0, 20, 20, "-");
        m_Delete->user_data ((void*)(this));
        m_Delete->box (FL_PLASTIC_UP_BOX);
        m_Delete->color (Info->GUI_COLOUR);
        m_Delete->selection_color (Info->GUI_COLOUR);
	m_Delete->callback ((Fl_Callback*)cb_Delete);
	m_Buttons->add (m_Delete);

        m_Add = new Fl_Button (22, 0, 20, 20, "+");
        m_Add->user_data ((void*)(this));
        m_Add->box (FL_PLASTIC_UP_BOX);
        m_Add->color (Info->GUI_COLOUR);
        m_Add->selection_color (Info->GUI_COLOUR);
	m_Add->callback ((Fl_Callback*)cb_Add);
	m_Buttons->add (m_Add);

        m_PeakInd = new Fl_LED_Button (42, 0, 20, 20, "");
        m_Buttons->add (m_PeakInd);
}

void MixerPluginGUI::AddChan (bool SendData, bool ResizeIt) {
     Fl_Slider *NewSlide = new Fl_Slider (0, 0, 20, 100, "");
     NewSlide->user_data ((void*)(this));
     NewSlide->type (FL_VERT_NICE_SLIDER);
     NewSlide->selection_color (m_GUIColour);
     NewSlide->box (FL_PLASTIC_DOWN_BOX);
     NewSlide->labelsize (10);
     NewSlide->maximum (2);
     NewSlide->step (0.01);
     NewSlide->value (1.0);
     int num = (int)m_SlidVec.size();
     NewSlide->callback ((Fl_Callback*)cb_Chan, (void*)&Numbers[num]);
     m_MainPack->add (NewSlide);
     m_SlidVec.push_back (NewSlide);
     if (SendData) {
        m_GUICH->Set ("Num", ++num);
        m_GUICH->SetCommand (MixerPlugin::SETNUM);
        m_GUICH->Wait ();
        m_GUICH->Set ("Num", num);
        m_GUICH->Set ("Value", (float)(2.0f - NewSlide->value ()));
        m_GUICH->SetCommand(MixerPlugin::SETCH);
        m_GUICH->Wait ();
     }
     if (ResizeIt && num > 3) Resize (w()+20, h());
}

void MixerPluginGUI::DeleteChan (bool SendData) {
     vector<Fl_Slider*>::iterator i = m_SlidVec.end ();
     i--;
     m_MainPack->remove (*i);
     delete *i;
     m_SlidVec.erase (i);
     int num = (int)m_SlidVec.size();
     if (SendData) {
        m_GUICH->Set ("Num", num);
        m_GUICH->SetCommand (MixerPlugin::SETNUM);
        m_GUICH->Wait ();
     }
     if (num > 2) Resize (w()-20, h());
}

void MixerPluginGUI::UpdateValues(SpiralPlugin *o) {
     MixerPlugin *Plugin = (MixerPlugin *)o;
     unsigned int chans = Plugin->GetChannels();
     while (chans < m_SlidVec.size()) DeleteChan (false);
     while (chans > m_SlidVec.size()) AddChan (false, true);
     for (unsigned int n=0; n<chans; n++)
         m_SlidVec[n]->value (2.0f - Plugin->GetChannel (n));
     redraw();
}

void MixerPluginGUI::Update () {
     if (m_GUICH->GetBool ("Peak")) m_PeakInd->value (true);
}

inline void MixerPluginGUI::cb_Add_i (Fl_Button* o, void* v) {
       m_PeakInd->value (false);
       if ((int)m_SlidVec.size() < MAX_CHANNELS) AddChan (true, true);
}

void MixerPluginGUI::cb_Add (Fl_Button* o, void* v) {
     ((MixerPluginGUI*)(o->user_data()))->cb_Add_i (o, v);
}

inline void MixerPluginGUI::cb_Delete_i (Fl_Button* o, void* v) {
       m_PeakInd->value (false);
       if (m_SlidVec.size() > 2) DeleteChan ();
}

void MixerPluginGUI::cb_Delete (Fl_Button* o, void* v) {
     ((MixerPluginGUI*)(o->user_data()))->cb_Delete_i (o, v);
}

inline void MixerPluginGUI::cb_Chan_i (Fl_Slider* o, void* v) {
       // This line works fine
       // cerr << *(int*)(v) << endl << (float)(2.0f-o->value()) << endl;
       // The segfault comes when you do any of the following - don't know why
       m_PeakInd->value (false);
       m_GUICH->Set("Num", (*(int*)(v)));
       m_GUICH->Set("Value", (float)(2.0f-o->value()));
       m_GUICH->SetCommand (MixerPlugin::SETCH);
}

void MixerPluginGUI::cb_Chan(Fl_Slider* o, void* v) {
     // If you use user_data() instead of parent()->parent() you get a segfault - don't know why
     ((MixerPluginGUI*)(o->parent()->parent()))->cb_Chan_i (o, v);
}

// you sometimes get a segfault on exit too - again - don't know why

const string MixerPluginGUI::GetHelpText (const string &loc){
      return string("")
      + "A general purpose mixer.\n"
      + "Useful for mixing CV values as well as mono audio\n"
      + "signals.\n"
      + "The LED indicates the the mixer output is at peak\n"
      + "level, click on it, or change levels to reset it.\n"
      + "Add up to 16 channels using the '+' button.\n"
      + "Use the '-' button to remove unwanted channels.\n";
}
