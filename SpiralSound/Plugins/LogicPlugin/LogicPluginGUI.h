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

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Counter.H>
#include "LogicPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef PluginGUI
#define PluginGUI

static const int NUM_KEYS = 12;

class LogicPluginGUI : public SpiralPluginGUI
{
public:
	LogicPluginGUI(int w, int h, LogicPlugin *o,ChannelHandler *ch,const HostInfo *Info);
	virtual void UpdateValues(SpiralPlugin *o);
protected:
        const std::string GetHelpText(const std::string &loc);
private:
        Fl_Counter *m_Inputs;
	Fl_Button *m_AND,*m_OR,*m_NOT,*m_NAND,*m_NOR,*m_XOR,*m_XNOR;
    	void ClearButtons();
    	//// Callbacks ////
	inline void cb_AND_i(Fl_Button* o, void* v);
	static void cb_AND(Fl_Button* o, void* v);
	inline void cb_OR_i(Fl_Button* o, void* v);
	static void cb_OR(Fl_Button* o, void* v);
	inline void cb_NOT_i(Fl_Button* o, void* v);
	static void cb_NOT(Fl_Button* o, void* v);
	inline void cb_NAND_i(Fl_Button* o, void* v);
	static void cb_NAND(Fl_Button* o, void* v);
	inline void cb_NOR_i(Fl_Button* o, void* v);
	static void cb_NOR(Fl_Button* o, void* v);
	inline void cb_XOR_i(Fl_Button* o, void* v);
	static void cb_XOR(Fl_Button* o, void* v);
	inline void cb_XNOR_i(Fl_Button* o, void* v);
	static void cb_XNOR(Fl_Button* o, void* v);
        inline void cb_Inputs_i (Fl_Counter* o, void* v);
        static void cb_Inputs (Fl_Counter* o, void* v);
};

#endif
