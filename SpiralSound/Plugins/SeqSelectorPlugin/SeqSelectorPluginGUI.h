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

#include "list"
#include <FL/Fl.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include "../Widgets/Fl_LED_Button.H"
#include "SeqSelectorPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef SEQ_SEL_GUI_H
#define SEQ_SEL_GUI_H

class CountLine : public Fl_Group
{
public:
	CountLine (int n, Fl_Color col1/*, Fl_Color col2*/);
	~CountLine();

	void  SetVal(int n, float val);
	float GetVal(int n);
	void  SetLED(bool s) { m_Flasher->value(s); }
	virtual int handle(int event);

	ChannelHandler *m_GUICH;

private:
	Fl_LED_Button *m_Flasher;
	Fl_Counter    *m_Counter[NUM_VALUES];
	int m_Num;
	char m_Count[32];
};

class SeqSelectorPluginGUI : public SpiralPluginGUI
{
public:
	SeqSelectorPluginGUI(int w, int h, SeqSelectorPlugin *o,ChannelHandler *ch,const HostInfo *Info);

	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update();

	void  AddLine(int* Val=NULL);
	void  RemoveLine();
	float GetVal(int l, int v);
	int   GetNumLines() { return m_LineVec.size(); }
	void  SetLED(int n);

	void StreamOut(std::ostream &s);
	void StreamIn(std::istream &s);

protected:
    const std::string GetHelpText(const std::string &loc);

private:

	int m_LastLight;
        Fl_Color m_Colour/*1, m_Colour2*/;
	Fl_Pack    *m_Main;
	Fl_Scroll  *m_Scroll;
	Fl_Button  *m_New;
	Fl_Button  *m_Delete;
	Fl_Counter *m_Begin;
	Fl_Counter *m_End;
	Fl_Button  *m_UseRange;

	std::list<CountLine*> m_LineVec;

	//// Callbacks ////
	inline void cb_New_i(Fl_Button* o, void* v);
	static void cb_New(Fl_Button* o, void* v);
	inline void cb_Delete_i(Fl_Button* o, void* v);
	static void cb_Delete(Fl_Button* o, void* v); 
	inline void cb_Begin_i(Fl_Counter* o, void* v);
	static void cb_Begin(Fl_Counter* o, void* v); 
	inline void cb_End_i(Fl_Counter* o, void* v);
	static void cb_End(Fl_Counter* o, void* v); 
	inline void cb_UseRange_i(Fl_Button* o, void* v);
	static void cb_UseRange(Fl_Button* o, void* v); 
};

#endif
