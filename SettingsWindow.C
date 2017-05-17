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

#include <stdio.h>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>

#include "SettingsWindow.h"
#include "SpiralSound/SpiralInfo.h"
#include "GUI/options.xpm"

SettingsWindow::SettingsWindow() :
Fl_Double_Window(250,275,"SSM Options"),
m_App(NULL)
{
	color(SpiralInfo::GUICOL_Tool);
	box(FL_FLAT_BOX);

	Fl_Pixmap *Icon = new Fl_Pixmap(options_xpm);
	Fl_Box *IconBox = new Fl_Box(25,30,50,50,"");
	IconBox->image(Icon);

	Fl_Box *TextBox = new Fl_Box(180,10,50,20,"SpiralSynth Modular");
	TextBox->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	TextBox = new Fl_Box(180,30,50,20,"Constructed by Dave Griffiths");
	TextBox->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	TextBox->labelsize(10);
	TextBox = new Fl_Box(192,40,50,20,"dave@@pawfal.org");
	TextBox->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	TextBox->labelsize(10);
	TextBox = new Fl_Box(180,50,50,20,"www.pawfal.org");
	TextBox->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	TextBox->labelsize(10);
	TextBox = new Fl_Box(180,63,50,20,"We are the music makers");
	TextBox->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	TextBox->labelsize(10);
	TextBox = new Fl_Box(180,73,50,20,"We are the dreamers of dreams");
	TextBox->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
	TextBox->labelsize(10);


	Fl_Group *OptionsGrp = new Fl_Group(5,95,240,150,"");
	OptionsGrp->box(FL_FLAT_BOX);
	OptionsGrp->color(SpiralInfo::GUICOL_Button);

	m_Options = new Fl_Pack(5,115,230,100,"Settings");
	m_Options->color(SpiralInfo::GUICOL_Button);
	OptionsGrp->add(m_Options);

	Fl_Box *Name;
	Fl_Pack *Line;

	char BufferSize[256];
	sprintf(BufferSize,"%d",SpiralInfo::BUFSIZE);

	char FragmentSize[256];
	sprintf(FragmentSize,"%d",SpiralInfo::FRAGSIZE);

	char FragmentCount[256];
	sprintf(FragmentCount,"%d",SpiralInfo::FRAGCOUNT);

	char Samplerate[256];
	sprintf(Samplerate,"%d",SpiralInfo::SAMPLERATE);

	char OutputDevice[256];
	sprintf(OutputDevice,"%s",SpiralInfo::OUTPUTFILE.c_str());

	char MidiDevice[256];
	sprintf(MidiDevice,"%s",SpiralInfo::MIDIFILE.c_str());

	Line = new Fl_Pack(0,0,100,20,"");
	Line->type(FL_HORIZONTAL);
	Name = new Fl_Box(55,0,150,20,"Buffer Size");
	Name->labelsize(10);
	Name->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	Line->add(Name);
	m_BufferSize = new Fl_Int_Input(0,0,80,20,"");
	m_BufferSize->value(BufferSize);
	Line->add(m_BufferSize);
	Line->end();
	m_Options->add(Line);

	Line = new Fl_Pack(0,0,100,20,"");
	Line->type(FL_HORIZONTAL);
	Name = new Fl_Box(55,0,150,20,"Fragment Size");
	Name->labelsize(10);
	Name->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	Line->add(Name);
	m_FragmentSize = new Fl_Int_Input(0,0,80,20,"");
	m_FragmentSize->value(FragmentSize);
	Line->add(m_FragmentSize);
	Line->end();
	m_Options->add(Line);

	Line = new Fl_Pack(0,0,100,20,"");
	Line->type(FL_HORIZONTAL);
	Name = new Fl_Box(55,0,150,20,"Fragment Count (-1 = auto)");
	Name->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	Name->labelsize(10);
	Line->add(Name);
	m_FragmentCount = new Fl_Int_Input(0,0,80,20,"");
	m_FragmentCount->value(FragmentCount);
	Line->add(m_FragmentCount);
	Line->end();
	m_Options->add(Line);

	Line = new Fl_Pack(0,0,100,20,"");
	Line->type(FL_HORIZONTAL);
	Name = new Fl_Box(55,0,150,20,"SampleRate");
	Name->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	Name->labelsize(10);
	Line->add(Name);
	m_Samplerate = new Fl_Int_Input(0,0,80,20,"");
	m_Samplerate->value(Samplerate);
	Line->add(m_Samplerate);
	Line->end();
	m_Options->add(Line);

	Line = new Fl_Pack(0,0,100,20,"");
	Line->type(FL_HORIZONTAL);
	Name = new Fl_Box(55,0,150,20,"Output Device");
	Name->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	Name->labelsize(10);
	Line->add(Name);
	m_OutputDevice = new Fl_Input(0,0,80,20,"");
	m_OutputDevice->value(OutputDevice);
	Line->add(m_OutputDevice);
	Line->end();
	m_Options->add(Line);

	Line = new Fl_Pack(0,0,100,20,"");
	Line->type(FL_HORIZONTAL);
	Name = new Fl_Box(55,0,150,20,"Midi Device");
	Name->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	Name->labelsize(10);
	Line->add(Name);
	m_MidiDevice = new Fl_Input(0,0,80,20,"");
	m_MidiDevice->value(MidiDevice);
	Line->add(m_MidiDevice);
	Line->end();
	m_Options->add(Line);

	m_Options->end();

	m_Save = new Fl_Button(140,250,50,20,"Save");
 	m_Save->labelsize(10);
 	m_Save->tooltip("Save these settings");
	m_Save->callback((Fl_Callback*)cb_Save);
 	add(m_Save);

	m_Apply = new Fl_Button(195,250,50,20,"Apply");
	m_Apply->labelsize(10);
	m_Apply->tooltip("Some plugins may request to save data first");
	m_Apply->callback((Fl_Callback*)cb_Apply);
	add(m_Apply);

	end();
}

SettingsWindow::~SettingsWindow()
{
}

inline void SettingsWindow::cb_Apply_i(Fl_Button* o, void* v)
{
	SpiralInfo::BUFSIZE=(int)atof(m_BufferSize->value());
	SpiralInfo::FRAGSIZE=(int)atof(m_FragmentSize->value());
	SpiralInfo::FRAGCOUNT=(int)atof(m_FragmentCount->value());
	SpiralInfo::SAMPLERATE=(int)atof(m_Samplerate->value());
	SpiralInfo::OUTPUTFILE=m_OutputDevice->value();
	SpiralInfo::MIDIFILE=m_MidiDevice->value();

	assert(m_App);
	if (m_App)
	{
		m_App->UpdateHostInfo();
	}
}
void SettingsWindow::cb_Apply(Fl_Button* o, void* v)
{ ((SettingsWindow*)(o->parent()))->cb_Apply_i(o,v); }

inline void SettingsWindow::cb_Save_i(Fl_Button* o, void* v)
{
 	SpiralInfo::BUFSIZE=(int)atof(m_BufferSize->value());
 	SpiralInfo::FRAGSIZE=(int)atof(m_FragmentSize->value());
 	SpiralInfo::FRAGCOUNT=(int)atof(m_FragmentCount->value());
 	SpiralInfo::SAMPLERATE=(int)atof(m_Samplerate->value());
 	SpiralInfo::OUTPUTFILE=m_OutputDevice->value();
 	SpiralInfo::MIDIFILE=m_MidiDevice->value();
 	SpiralInfo::Get()->SavePrefs();
}
void SettingsWindow::cb_Save(Fl_Button* o, void* v)
{ ((SettingsWindow*)(o->parent()))->cb_Save_i(o,v); }
