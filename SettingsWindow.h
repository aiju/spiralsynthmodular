/*  SpiralSynthModular
 *  Copyleft (C) 2002 David Griffiths <dave@pawfal.org>
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

#ifndef SETTINGSWINDOW
#define SETTINGSWINDOW

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Input.H>

#include "SpiralSynthModular.h"

class SynthModular;

class SettingsWindow : public Fl_Double_Window
{
public:
	SettingsWindow();
	~SettingsWindow();
		
	void RegisterApp(SynthModular *s) { m_App=s; }
	
private:
	SynthModular *m_App;
	
	Fl_Pack      *m_Options;
	Fl_Int_Input *m_BufferSize;
	Fl_Int_Input *m_FragmentSize;
	Fl_Int_Input *m_FragmentCount;
	Fl_Int_Input *m_Samplerate;	
	Fl_Input     *m_OutputDevice;
	Fl_Input     *m_MidiDevice;
	Fl_Button    *m_Save;	
	Fl_Button    *m_Apply;
		
	inline void cb_Apply_i(Fl_Button* o, void* v);
    static void cb_Apply(Fl_Button* o, void* v);	
	inline void cb_Save_i(Fl_Button* o, void* v);
    static void cb_Save(Fl_Button* o, void* v);
};

#endif
