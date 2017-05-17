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

#include "XFadePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

XFadePluginGUI::XFadePluginGUI(int w, int h,XFadePlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
        m_Mix = new Fl_Slider(5, 20, 200, 20, "");
	m_Mix->type(FL_HOR_NICE_SLIDER);
        m_Mix->box (FL_PLASTIC_DOWN_BOX);
	m_Mix->selection_color(Info->GUI_COLOUR);
        m_Mix->labelsize(10);
	m_Mix->maximum(1);
        m_Mix->step(0.0001);
        m_Mix->value(0.5);
        m_Mix->callback((Fl_Callback*)cb_Mix);

	end();
}

void XFadePluginGUI::UpdateValues(SpiralPlugin *o)
{
	XFadePlugin *Plugin = (XFadePlugin*)o;
	m_Mix->value(Plugin->GetMix());
}

inline void XFadePluginGUI::cb_Mix_i(Fl_Slider* o, void* v)
{
	m_GUICH->Set("Mix",(float)o->value());
}
void XFadePluginGUI::cb_Mix(Fl_Slider* o, void* v)
{ ((XFadePluginGUI*)(o->parent()))->cb_Mix_i(o,v); }

const string XFadePluginGUI::GetHelpText(const string &loc)
{
	if (loc=="EN")
	{
		return "For mixing stereo inputs like a superstar DJ would.";
	}
	else if (loc=="FR")
	{
		return "FR: For mixing stereo inputs like a superstar DJ would.";
	}
	else if (loc=="DE")
	{
		return "DE: For mixing stereo inputs like a superstar DJ would.";
	}

	// default to english?
	return "DE: For mixing stereo inputs like a superstar DJ would.";
}
