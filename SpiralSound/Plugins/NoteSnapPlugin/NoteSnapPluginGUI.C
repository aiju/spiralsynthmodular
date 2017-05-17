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

#include "NoteSnapPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

NoteSnapPluginGUI::NoteSnapPluginGUI(int w, int h,NoteSnapPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	int KeyWidth=10,Note,Pos=0,Count=0;

	for (int n=0; n<NUM_KEYS; n++)
	{
		m_Num[n]=n;

		Note = n%12;
		if (Note!=1 && Note!=3 && Note!=6 && Note!=8 && Note!=10)
		{
			Pos=Count*KeyWidth;
			Count++;
			m_Key[n] = new Fl_Button (Pos+5, 20, KeyWidth, 50, "");
			m_Key[n]->type(1);
			m_Key[n]->selection_color(FL_RED);
			m_Key[n]->box(FL_THIN_UP_BOX);
			m_Key[n]->labelsize(10);
			m_Key[n]->when(FL_WHEN_CHANGED);

			m_Key[n]->color(FL_WHITE);
			m_Key[n]->callback((Fl_Callback*)cb_Key, &m_Num[n]);
			add(m_Key[n]);
		}
	}

	Count=0;
	for (int n=0; n<NUM_KEYS; n++)
	{
		Note = n%12;
		if (Note==1 || Note==3 || Note==6 || Note==8 || Note==10)
		{
			m_Key[n] = new Fl_Button (Pos+10, 20, KeyWidth, 30, "");
			m_Key[n]->type(1);
			m_Key[n]->selection_color(FL_RED);
			m_Key[n]->box(FL_THIN_UP_BOX);
			m_Key[n]->labelsize(10);
			m_Key[n]->when(FL_WHEN_CHANGED);
			m_Key[n]->color(FL_BLACK);
			m_Key[n]->callback((Fl_Callback*)cb_Key, &m_Num[n]);
			add(m_Key[n]);
		}
		else
		{
			Pos=Count*KeyWidth;
			Count++;
		}
	}

	end();
}



void NoteSnapPluginGUI::UpdateValues(SpiralPlugin *o)
{
	NoteSnapPlugin *Plugin = (NoteSnapPlugin *)o;
	for (int n=0; n<12; n++)
	{
		m_Key[n]->value(!Plugin->GetFilter(n));
	}
}

//// Callbacks ////
inline void NoteSnapPluginGUI::cb_Key_i(Fl_Button* o, void* v) 
{ 
	int k=*(int*)(v);
	if (o->value()) 
	{
		m_GUICH->Set("Note",k);
		m_GUICH->SetCommand(NoteSnapPlugin::NOTE_OFF); 
	}
	else
	{
		m_GUICH->Set("Note",k);
		m_GUICH->SetCommand(NoteSnapPlugin::NOTE_ON); 
	}
	parent()->redraw();
}
void NoteSnapPluginGUI::cb_Key(Fl_Button* o, void* v) 
{ ((NoteSnapPluginGUI*)(o->parent()))->cb_Key_i(o,v);}
	
const string NoteSnapPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Quantises the input value into a note frequency\n"
    + "(using the midi note data).\n"
	+ "Use the keyboard to select notes to be filtered out\n"
	+ "for generating scales and chords";
}
