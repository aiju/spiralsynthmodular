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

#include "FormantFilterPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

FormantFilterPluginGUI::FormantFilterPluginGUI(int w, int h,FormantFilterPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	Selection = new Fl_Knob(20, 18, 45, 45, "Vowel");
        Selection->color(Info->GUI_COLOUR);
	Selection->type(Fl_Knob::DOTLIN);
        Selection->labelsize(10);
        Selection->maximum(4);
        Selection->step(0.0001);
        Selection->value(0);
	Selection->callback((Fl_Callback*)cb_Selection);

	Display = new Fl_Output(30,80,30,20,"");
        Display->box (FL_PLASTIC_UP_BOX);
        Display->value ("A");
	Display->set_output();

        end();
}

void FormantFilterPluginGUI::UpdateValues(SpiralPlugin *o)
{
	FormantFilterPlugin *Plugin = (FormantFilterPlugin*)o;

	Selection->value(Plugin->GetVowel());

	float v=Plugin->GetVowel();

	if (v<1) Display->value("A");
	if (v>1 && v<2) Display->value("E");
	if (v>2 && v<3) Display->value("I");
	if (v>3 && v<4) Display->value("O");
	if (v==4) Display->value("U");
}

inline void FormantFilterPluginGUI::cb_Selection_i(Fl_Knob* o, void* v)
{
	m_GUICH->Set("Vowel",(float)o->value());

	float vf=o->value();
	if (vf<1) Display->value("A");
	if (vf>1 && vf<2) Display->value("E");
	if (vf>2 && vf<3) Display->value("I");
	if (vf>3 && vf<4) Display->value("O");
	if (vf==4) Display->value("U");
}
void FormantFilterPluginGUI::cb_Selection(Fl_Knob* o, void* v)
{ ((FormantFilterPluginGUI*)(o->parent()))->cb_Selection_i(o,v); }


const string FormantFilterPluginGUI::GetHelpText(const string &loc){
    return string("")
	 + "A formant filter for synthesising vowel sounds by filtering\n" 
	 + "high harmonic input waveforms.\n"
	 + "From the great www.musicdsp.org site, I've added linear\n"
	 + "interpolation between the filters so you can sweep them with\n"
	 + "an external CV if desired";
}
