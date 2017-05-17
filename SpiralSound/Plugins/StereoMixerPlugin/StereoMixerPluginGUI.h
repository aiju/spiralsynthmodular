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
#include <FL/Fl_Slider.H>
#include "../Widgets/Fl_Knob.H"
#include "StereoMixerPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef StMixerGUI
#define StMixerGUI

class StereoMixerPluginGUI : public SpiralPluginGUI
{
public:
	StereoMixerPluginGUI(int w, int h, StereoMixerPlugin *o,ChannelHandler *ch,const HostInfo *Info);
	virtual void UpdateValues(SpiralPlugin *o);
	
protected:
    const std::string GetHelpText(const std::string &loc);	

private:
	
	int Numbers[NUM_CHANNELS];
	
	Fl_Slider* m_Chan[NUM_CHANNELS];
	Fl_Knob* m_Pan[NUM_CHANNELS];
	
	//// Callbacks ////
	inline void cb_Chan_i(Fl_Slider* o, void* v);
	static void cb_Chan(Fl_Slider* o, void* v); 
	
	inline void cb_Pan_i(Fl_Knob* o, void* v);
	static void cb_Pan(Fl_Knob* o, void* v); 
};

#endif
