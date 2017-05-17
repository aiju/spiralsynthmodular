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

#include <stdio.h>
#include "SeqSelectorPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>

using namespace std;

////////////////////////////////////////////

CountLine::CountLine (int n, Fl_Color col) :
Fl_Group(0,0,250,14,"")
{
	box(FL_FLAT_BOX);
	if (n%4==0) color (col);
	// if (n%8==0) color (col2);
	m_Num=n;

	sprintf(m_Count,"%d",n);
	Fl_Box *Num = new Fl_Box(5,2,30,20,m_Count);
	Num->labelsize(10);
	Num->labeltype(FL_ENGRAVED_LABEL);
    Num->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	add(Num);

	m_Flasher = new Fl_LED_Button (15, -3, 20, 20, "");
	m_Flasher->selection_color (FL_RED);
	add (m_Flasher);

	for (int n=0; n<NUM_VALUES; n++)
	{
		m_Counter[n] = new Fl_Counter(30+n*25, 2, 25, 12, "");
		m_Counter[n]->labelsize(8);
		m_Counter[n]->textsize(8);
		m_Counter[n]->type(FL_SIMPLE_COUNTER);
		m_Counter[n]->step(1);
		m_Counter[n]->value(0);
		add(m_Counter[n]);
	}
	end();
	redraw();
}

CountLine::~CountLine()
{
}

void CountLine::SetVal(int n, float val)
{
	// range check
	assert(n>=0 && n<8);
	m_Counter[n]->value(val);
}

float CountLine::GetVal(int n)
{
	// range check
	assert(n>=0 && n<8);
	return m_Counter[n]->value();
}

int CountLine::handle(int event)
{
	int temp = Fl_Group::handle(event);
	if (event==FL_PUSH)
	{
		for(int n=0; n<NUM_VALUES; n++)
		{
			m_GUICH->Set("Line",m_Num);
			m_GUICH->Set("Num",n);
			m_GUICH->Set("Val",(int)GetVal(n));
			m_GUICH->SetCommand(SeqSelectorPlugin::SET_VAL);
			m_GUICH->Wait();
		}
	}

	return temp;
}

////////////////////////////////////////////

SeqSelectorPluginGUI::SeqSelectorPluginGUI(int w, int h,SeqSelectorPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
        m_Colour = (Fl_Color)Info->GUI_COLOUR;
        //m_Colour2 = (Fl_Color)Info->SCOPE_BG_COLOUR;

        m_Scroll = new Fl_Scroll(0, 20, w, h-50, "");
	m_Scroll->type(Fl_Scroll::VERTICAL_ALWAYS);
	m_Scroll->box(FL_NO_BOX);
	m_Scroll->position(0,0);
	add(m_Scroll);

	m_Main = new Fl_Pack(0,20,w,h-50,"");
	m_Main->box(FL_NO_BOX);
	m_Scroll->add(m_Main);

	m_New = new Fl_Button(5,h-25,50,20,"New");
	m_New->labelsize(10);
	m_New->callback((Fl_Callback*)cb_New);
	add(m_New);

	m_Delete = new Fl_Button(60,h-25,50,20,"Delete");
	m_Delete->labelsize(10);
	m_Delete->callback((Fl_Callback*)cb_Delete);
	add(m_Delete);

	m_Begin = new Fl_Counter(115,h-28,50,15,"Begin");
	m_Begin->labelsize(10);
	m_Begin->type(FL_SIMPLE_COUNTER);
	m_Begin->step(1);
	m_Begin->value(0);
	m_Begin->callback((Fl_Callback*)cb_Begin);
	add(m_Begin);

	m_End = new Fl_Counter(175,h-28,50,15,"End");
	m_End->labelsize(10);
	m_End->type(FL_SIMPLE_COUNTER);
	m_End->step(1);
	m_End->value(0);
	m_End->callback((Fl_Callback*)cb_End);
	add(m_End);

	m_UseRange = new Fl_Button(230,h-25,55,20,"UseRange");
	m_UseRange->labelsize(10);
	m_UseRange->type(1);
	m_UseRange->value(0);
	m_UseRange->callback((Fl_Callback*)cb_UseRange);
	add(m_UseRange);

	end();
}

float SeqSelectorPluginGUI::GetVal(int l, int v)
{
	int c=0;
	list<CountLine*>::iterator i = m_LineVec.end();
	do
	{
		i--;
		if (l==c) return (*i)->GetVal(v);
		c++;
	}
	while (i!=m_LineVec.begin());

	return 0;
}

void SeqSelectorPluginGUI::AddLine(int* Val)
{
	CountLine *NewLine = new CountLine(m_LineVec.size(), m_Colour/*1, m_Colour2*/);
        NewLine->m_GUICH = m_GUICH;

	// copy the last line
	list<CountLine*>::iterator i=m_LineVec.begin();

	if (Val)
	{
		for (int n=0; n<NUM_VALUES; n++)
		{
			NewLine->SetVal(n,Val[n]);
		}
	}
	else
	{
		if (m_LineVec.size()>0)
		{
			for (int n=0; n<NUM_VALUES; n++)
			{
				NewLine->SetVal(n,(*i)->GetVal(n));
			}
		}
	}

	m_Main->add(NewLine);
	m_LineVec.push_front(NewLine);
	redraw();

	Fl::check();
}

void SeqSelectorPluginGUI::RemoveLine()
{
	list<CountLine*>::iterator i=m_LineVec.begin();
	if (m_LineVec.size()>0)
	{
		m_Main->remove(*i);
		delete(*i);
		m_LineVec.erase(i);
		m_Main->redraw();
		m_Scroll->redraw();
	}
}

void SeqSelectorPluginGUI::Update()
{
	int p=m_GUICH->GetInt("Pos");
	if (m_LastLight!=p) 
	{
		m_LastLight=p;
		SetLED(p);
	}
}

void SeqSelectorPluginGUI::SetLED(int n)
{
	int c=0;
	if (m_LineVec.empty()) return;
	
	list<CountLine*>::iterator i = m_LineVec.end();
	do
	{
		i--;
		if (n==c) (*i)->SetLED(true);
		else (*i)->SetLED(false);
		c++;
	}
	while (i!=m_LineVec.begin());
}

void SeqSelectorPluginGUI::UpdateValues(SpiralPlugin *o)
{
	SeqSelectorPlugin *Plugin = (SeqSelectorPlugin *)o;
	
	int c=Plugin->m_Lines.size();
	for (int n=0; n<c; n++)
	{
		int temp[8];
		for (int i=0; i<8; i++) temp[i]=Plugin->m_Lines[n].Value[i];
		AddLine(temp);
	}
}

inline void SeqSelectorPluginGUI::cb_New_i(Fl_Button* o, void* v)
{
	AddLine();
	m_GUICH->SetCommand(SeqSelectorPlugin::ADD_LINE);
}
void SeqSelectorPluginGUI::cb_New(Fl_Button* o, void* v)
{ ((SeqSelectorPluginGUI*)(o->parent()))->cb_New_i(o,v); }

inline void SeqSelectorPluginGUI::cb_Delete_i(Fl_Button* o, void* v)
{
	RemoveLine();
	m_GUICH->SetCommand(SeqSelectorPlugin::REM_LINE);
}
void SeqSelectorPluginGUI::cb_Delete(Fl_Button* o, void* v)
{ ((SeqSelectorPluginGUI*)(o->parent()))->cb_Delete_i(o,v); }

inline void SeqSelectorPluginGUI::cb_Begin_i(Fl_Counter* o, void* v)
{
	m_GUICH->Set("Line",(int)o->value());
	m_GUICH->SetCommand(SeqSelectorPlugin::SET_BEGIN);
}
void SeqSelectorPluginGUI::cb_Begin(Fl_Counter* o, void* v)
{ ((SeqSelectorPluginGUI*)(o->parent()))->cb_Begin_i(o,v); }

inline void SeqSelectorPluginGUI::cb_End_i(Fl_Counter* o, void* v)
{
	m_GUICH->Set("Line",(int)o->value());
	m_GUICH->SetCommand(SeqSelectorPlugin::SET_END);
}
void SeqSelectorPluginGUI::cb_End(Fl_Counter* o, void* v)
{ ((SeqSelectorPluginGUI*)(o->parent()))->cb_End_i(o,v); }

inline void SeqSelectorPluginGUI::cb_UseRange_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Val",(int)o->value());
	m_GUICH->SetCommand(SeqSelectorPlugin::RANGE);
}
void SeqSelectorPluginGUI::cb_UseRange(Fl_Button* o, void* v)
{ ((SeqSelectorPluginGUI*)(o->parent()))->cb_UseRange_i(o,v); }

const string SeqSelectorPluginGUI::GetHelpText(const string &loc){
    return string("") 
	+ "The SeqSelector is designed to allow you to syncronise and select up\n"
	+ "to 8 Sequencer or Matrix patterns into tracks. When the SeqSelector\n"
	+ "recieves a trigger (which could come from a master pattern, or a clock\n"
	+ "oscillator) it will briefly flash the next set of values to its outputs,\n"
	+ "triggering the next patterns on it's slave sequencers or matrix plugins.\n\n"
	+ "You can also specify a loop, which will be used if the \"use range\"\n"
	+ "button is activated. This is useful for auditioning sections of a track.";
}
