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

#include "SpiralPluginGUI.h"
#include "SpiralPlugin.h"
#include "../SpiralInfo.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

using namespace std;

Fl_Double_Window* SpiralPluginGUI::m_HelpWin=NULL;
Fl_Text_Display* SpiralPluginGUI::m_HelpWin_text=NULL;
SpiralPluginGUI* SpiralPluginGUI::Help_owner=NULL;

SpiralPluginGUI::SpiralPluginGUI(int w, int h, SpiralPlugin* o, ChannelHandler *ch) :
SpiralGUIType(0,0,w,h,"")
{
	Fl::visible_focus(false);

	m_GUICH = ch;
	box(FL_NO_BOX);

	m_Hide = new Fl_Button(2,2,10,10,"X");
	m_Hide->labeltype(FL_ENGRAVED_LABEL);
	m_Hide->labelsize(10);
	m_Hide->box(FL_NO_BOX);
	m_Hide->callback((Fl_Callback*)cb_Hide);
	add(m_Hide);

  	m_Help = new Fl_Button(w-11,2,10,10,"?");
	m_Help->labeltype(FL_ENGRAVED_LABEL);
	m_Help->labelsize(10);
	m_Help->box(FL_NO_BOX);
	m_Help->down_box(FL_NO_BOX);
	m_Help->callback((Fl_Callback*)cb_Help);
	add(m_Help);

        resizable(NULL);
}

SpiralPluginGUI::~SpiralPluginGUI()
{
	// Needed to properly remove the window.
        Fl::check();
}

void SpiralPluginGUI::Resize (int neww, int newh) {
     resize (x(), y(), neww, newh);
     m_Help->position (x()+neww-11, y()+2);
     DoResizeCallback ();
}

void SpiralPluginGUI::Update()
{
}

const string SpiralPluginGUI::GetHelpText(const string &loc)
{
	return "Help! I need some helptext!!!";
}

//// Callbacks ////

inline void SpiralPluginGUI::cb_Hide_i(Fl_Button* o, void* v)
{ hide(); }
void SpiralPluginGUI::cb_Hide(Fl_Button* o, void* v)
{ ((SpiralPluginGUI*)(o->parent()))->cb_Hide_i(o,v); }

// Boo - changed to use one 'global' help win. for all plugins, coz Fl_Text_Display
// seems to be too buggy to create and destroy it multiple times.
// (symptom was - ssm crashes after opening/closing plugin help window 4-7 times)
// (i use FLTK 1.1.0 rc7)
inline void SpiralPluginGUI::cb_Help_i(Fl_Button* o, void* v)
{
	//Boo - create 'global' help window
	if (m_HelpWin==NULL)
	{
		int h_w=450,h_h=200;

		m_HelpWin = new Fl_Double_Window(h_w,h_h,"Help");

		m_HelpWin_text = new Fl_Text_Display(0,0,h_w,h_h);
		m_HelpWin_text->buffer(new Fl_Text_Buffer);

		m_HelpWin_text->textsize(12);

		m_HelpWin->add(m_HelpWin_text);
		m_HelpWin->resizable(m_HelpWin_text);

		m_HelpWin->callback((Fl_Callback*)cb_Help_close);
	}

	if(Help_owner!=this)
	{
                m_HelpWin_text->buffer()->text(GetHelpText(SpiralInfo::LOCALE).c_str());
		m_HelpWin->show();
		Help_owner=this;
	}
	else cb_Help_close_i(m_HelpWin, NULL);
}
void SpiralPluginGUI::cb_Help(Fl_Button* o, void* v)
{ ((SpiralPluginGUI*)(o->parent()))->cb_Help_i(o,v); }

inline void SpiralPluginGUI::cb_Help_close_i(Fl_Double_Window* w, void* v)
{
	w->hide();
	Help_owner=NULL;
}

void SpiralPluginGUI::cb_Help_close(Fl_Double_Window* w, void* v)
{ ((SpiralPluginGUI*)(w->parent()))->cb_Help_close_i(w,v); }

