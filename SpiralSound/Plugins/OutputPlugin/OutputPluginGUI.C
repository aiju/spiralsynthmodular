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

#include "OutputPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_file_chooser.H>

using namespace std;

OutputPluginGUI::OutputPluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
    Volume = new Fl_Knob(30, 22, 40, 40, "Volume");
    Volume->color(Info->GUI_COLOUR);
    Volume->type(Fl_Knob::DOTLIN);
    Volume->labelsize(10);
    Volume->maximum(1);
    Volume->step(0.001);
    Volume->value(0.5);
    Volume->callback((Fl_Callback*)cb_Volume);

    OpenRead = new Fl_Button(2, 80, 30, 15, "Read");
    OpenRead->type(FL_TOGGLE_BUTTON);
    OpenRead->box (FL_PLASTIC_UP_BOX);
    OpenRead->color (Info->GUI_COLOUR);
    OpenRead->selection_color (Info->GUI_COLOUR);
    OpenRead->labelsize(10);
    OpenRead->callback((Fl_Callback*)cb_OpenRead);

    OpenDuplex = new Fl_Button(34, 80, 31, 15, "Dplx");
    OpenDuplex->type(FL_TOGGLE_BUTTON);
    OpenDuplex->box (FL_PLASTIC_UP_BOX);
    OpenDuplex->color (Info->GUI_COLOUR);
    OpenDuplex->selection_color (Info->GUI_COLOUR);
    OpenDuplex->labelsize(10);
    OpenDuplex->callback((Fl_Callback*)cb_OpenDuplex);

    OpenWrite = new Fl_Button(68, 80, 30, 15, "Write");
    OpenWrite->type(FL_TOGGLE_BUTTON);
    OpenWrite->box (FL_PLASTIC_UP_BOX);
    OpenWrite->color (Info->GUI_COLOUR);
    OpenWrite->selection_color (Info->GUI_COLOUR);
    OpenWrite->labelsize(10);
    OpenWrite->callback((Fl_Callback*)cb_OpenWrite);

    end();
}

void OutputPluginGUI::Update()
{
     if (m_GUICH->GetBool ("OpenOut")) {
        OpenWrite->value (1);
        OpenRead->value (0);
        OpenDuplex->value (0);
        m_GUICH->SetCommand (OutputPlugin::CLEAR_NOTIFY);
     }
}

void OutputPluginGUI::UpdateValues(SpiralPlugin *o)
{
	Volume->value(OSSOutput::Get()->GetVolume());
}

//// Callbacks ////

inline void OutputPluginGUI::cb_Volume_i(Fl_Knob* o, void* v)
{
	m_GUICH->Set("Volume",(float)o->value());
 	m_GUICH->SetCommand(OutputPlugin::SET_VOLUME);
}
void OutputPluginGUI::cb_Volume(Fl_Knob* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_Volume_i(o,v); }

inline void OutputPluginGUI::cb_OpenRead_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		OpenWrite->value(0);
                OpenDuplex->value(0);
		m_GUICH->SetCommand(OutputPlugin::CLOSE);
		m_GUICH->Wait();
		m_GUICH->SetCommand(OutputPlugin::OPENREAD);
		m_GUICH->Wait();
	}
	else
	{
		m_GUICH->SetCommand(OutputPlugin::CLOSE);
		m_GUICH->Wait();
	}
}
void OutputPluginGUI::cb_OpenRead(Fl_Button* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_OpenRead_i(o,v); }

inline void OutputPluginGUI::cb_OpenDuplex_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		OpenWrite->value(0);
                OpenRead->value(0);
		m_GUICH->SetCommand(OutputPlugin::CLOSE);
		m_GUICH->Wait();
		m_GUICH->SetCommand(OutputPlugin::OPENDUPLEX);
		m_GUICH->Wait();
	}
	else
	{
		m_GUICH->SetCommand(OutputPlugin::CLOSE);
		m_GUICH->Wait();
	}
}
void OutputPluginGUI::cb_OpenDuplex(Fl_Button* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_OpenDuplex_i(o,v); }

inline void OutputPluginGUI::cb_OpenWrite_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		OpenDuplex->value(0);
                OpenRead->value(0);
		m_GUICH->SetCommand(OutputPlugin::CLOSE);
		m_GUICH->Wait();
		m_GUICH->SetCommand(OutputPlugin::OPENWRITE);
		m_GUICH->Wait();
	}
	else
	{
		m_GUICH->SetCommand(OutputPlugin::CLOSE);
		m_GUICH->Wait();
	}
}
void OutputPluginGUI::cb_OpenWrite(Fl_Button* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_OpenWrite_i(o,v); }


const string OutputPluginGUI::GetHelpText(const string &loc){
    return string("")
	+ "Your basic OSS i/o plugin, It opens the OSS sound driver, and outputs\n"
	+ "whatever is passed into it's inputs to the soundcard. It works in stereo,\n"
	+ "so you have seperate left and right inputs.\n\n"
	+ "There are three modes of operation: read, write and duplex. You can select\n"
	+ "read to record/process sound from your soundcard, write to play sound\n"
	+ "(default) and if your card supports it - duplex, to play and record \n"
	+ "simultaneously.";
}
