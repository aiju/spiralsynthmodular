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
#include "KeyboardPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef KEYBOARD_GUI_H
#define KEYBOARD_GUI_H

const int NUM_KEYS = 132;

class KeyboardPluginGUI : public SpiralPluginGUI
{
public:
	KeyboardPluginGUI(int w, int h, KeyboardPlugin *o,ChannelHandler *ch,const HostInfo *Info);

	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update();

protected:
    const std::string GetHelpText(const std::string &loc);

private:
	int         m_Num[NUM_KEYS];
	Fl_Button*  m_Key[NUM_KEYS];
	char        m_Label[NUM_KEYS][2];
	int         m_Last;
	int         m_Oct;

	//// Callbacks ////
	inline void cb_Key_i(Fl_Button* o, void* v);
	static void cb_Key(Fl_Button* o, void* v);
};

#endif
