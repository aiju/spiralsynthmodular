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

#include "NoisePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

NoisePluginGUI::NoisePluginGUI(int w, int h,NoisePlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
    White = new Fl_LED_Button (15, 12, 55, 20, "White");
    White->type (FL_RADIO_BUTTON);
    White->labelsize (10);
    White->set ();
    White->callback ((Fl_Callback*)cb_White);

    Pink = new Fl_LED_Button (15, 34, 55, 20, "Pink");
    Pink->type (FL_RADIO_BUTTON);
    Pink->labelsize (10);
    Pink->callback ((Fl_Callback*)cb_Pink);
    end();
}

void NoisePluginGUI::UpdateValues (SpiralPlugin *o) {
	NoisePlugin *Plugin = (NoisePlugin*)o;

	switch (Plugin->GetType()) {
	  case NoisePlugin::WHITE:
            White->value (1);
            Pink->value (0);
            break;
	  case NoisePlugin::PINK:
            White->value (0);
            Pink->value (1);
            break;
	}
}

//// Callbacks ////

inline void NoisePluginGUI::cb_White_i(Fl_LED_Button* o, void* v)
{ m_GUICH->Set("Type",(char)NoisePlugin::WHITE); }

void NoisePluginGUI::cb_White(Fl_LED_Button* o, void* v)
{ ((NoisePluginGUI*)(o->parent()))->cb_White_i(o,v); }

inline void NoisePluginGUI::cb_Pink_i(Fl_LED_Button* o, void* v)
{ m_GUICH->Set("Type",(char)NoisePlugin::PINK); }

void NoisePluginGUI::cb_Pink(Fl_LED_Button* o, void* v)
{ ((NoisePluginGUI*)(o->parent()))->cb_Pink_i(o,v); }


const string NoisePluginGUI::GetHelpText(const string &loc){
    return string("")
	+ "Makes noise, white noise is just raw randomness, pink noise\n"
	+ "is white noise filtered to contain equal amounts of each\n"
	+ "frequency, it should sound more natural and be more useful\n"
	+ "for synthesis uses";
}
