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
#include "../Widgets/Fl_Knob.H"
#include "MasherPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef MASHER_GUI_H
#define MASHER_GUI_H

class MasherPluginGUI : public SpiralPluginGUI
{
public:
	MasherPluginGUI(int w, int h, MasherPlugin *o,ChannelHandler *ch,const HostInfo *Info);

	virtual void UpdateValues(SpiralPlugin *o);
	virtual SpiralPlugin* GetPlugin() { return m_Plugin; }
	MasherPlugin *m_Plugin;

protected:
    const std::string GetHelpText(const std::string &loc);

private:

	Fl_Knob 		*m_Random;
	Fl_Knob 		*m_Pitch;
	Fl_Knob 		*m_GrainStoreSize;
	Fl_Knob 		*m_Density;

	//// Callbacks ////

	inline void cb_Random_i(Fl_Knob* o, void* v);
    static void cb_Random(Fl_Knob*, void*);
	inline void cb_Pitch_i(Fl_Knob* o, void* v);
    static void cb_Pitch(Fl_Knob*, void*);
	inline void cb_MinGrainSize_i(Fl_Knob* o, void* v);
    static void cb_MinGrainSize(Fl_Knob*, void*);
	inline void cb_GrainStoreSize_i(Fl_Knob* o, void* v);
    static void cb_GrainStoreSize(Fl_Knob*, void*);
	inline void cb_Density_i(Fl_Knob* o, void* v);
    static void cb_Density(Fl_Knob*, void*);

};

#endif
