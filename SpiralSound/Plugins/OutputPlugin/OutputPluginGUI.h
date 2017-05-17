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
#include "../Widgets/Fl_Knob.H"
#include "OutputPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef OUTPUT_GUI_H
#define OUTPUT_GUI_H

class OutputPluginGUI : public SpiralPluginGUI
{
public:
	OutputPluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info);

	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update();

protected:
    const std::string GetHelpText(const std::string &loc);

private:
	Fl_Knob 		*Volume;
	Fl_Button		*OpenRead;
	Fl_Button		*OpenWrite;
	Fl_Button		*OpenDuplex;
		
	//// Callbacks ////
	
	inline void cb_Volume_i(Fl_Knob* o, void* v);
    static void cb_Volume(Fl_Knob*, void*);
	inline void cb_Record_i(Fl_Button* o, void* v);
	static void cb_Record(Fl_Button* o, void* v);
	
	inline void cb_OpenRead_i(Fl_Button* o, void* v);
	static void cb_OpenRead(Fl_Button* o, void* v);
	inline void cb_OpenDuplex_i(Fl_Button* o, void* v);
	static void cb_OpenDuplex(Fl_Button* o, void* v);
	inline void cb_OpenWrite_i(Fl_Button* o, void* v);
	static void cb_OpenWrite(Fl_Button* o, void* v);

};

#endif
