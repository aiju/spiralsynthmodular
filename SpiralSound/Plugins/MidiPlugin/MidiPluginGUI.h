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
#include "MidiPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef MIDIGUI
#define MIDIGUI

int OptionsList(const std::vector<std::string> &List);

class MidiPluginGUI : public SpiralPluginGUI
{
public:
	MidiPluginGUI(int w, int h, MidiPlugin *o,ChannelHandler *ch,const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin *o);
		
protected:
    const std::string GetHelpText(const std::string &loc);
	
private:
	
	Fl_Counter* m_DeviceNum;
	Fl_Button*  m_NoteCut;
	Fl_Button*  m_ContinuousNotes;
	Fl_Button*  m_AddControl;
	Fl_Button*  m_RemoveControl;
	
	//// Callbacks ////
	inline void cb_DeviceNum_i(Fl_Counter* o, void* v);
	static void cb_DeviceNum(Fl_Counter* o, void* v); 
	inline void cb_NoteCut_i(Fl_Button* o, void* v);
	static void cb_NoteCut(Fl_Button* o, void* v); 
	inline void cb_ContinuousNotes_i(Fl_Button* o, void* v);
	static void cb_ContinuousNotes(Fl_Button* o, void* v); 
	inline void cb_AddControl_i(Fl_Button* o, void* v);
	static void cb_AddControl(Fl_Button* o, void* v); 
	inline void cb_RemoveControl_i(Fl_Button* o, void* v);
	static void cb_RemoveControl(Fl_Button* o, void* v); 
};

#endif
