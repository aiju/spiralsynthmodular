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

#include "MasherPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

MasherPluginGUI::MasherPluginGUI(int w, int h,MasherPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	m_Plugin=o;

	m_Pitch = new Fl_Knob (5, 20, 40, 40, "GrainPitch");
	m_Pitch->type(Fl_Knob::DOTLIN);
	m_Pitch->color(Info->GUI_COLOUR);
    m_Pitch->labelsize(10);
	m_Pitch->maximum(5);
    m_Pitch->step(0.001);
    m_Pitch->value(1);
    m_Pitch->callback((Fl_Callback*)cb_Pitch);

	m_Random = new Fl_Knob (70, 20, 40, 40, "Randomness");
	m_Random->type(Fl_Knob::DOTLIN);
	m_Random->color(Info->GUI_COLOUR);
    m_Random->labelsize(10);
	m_Random->maximum(100);
    m_Random->step(1);
    m_Random->value(1);
    m_Random->callback((Fl_Callback*)cb_Random);

	m_GrainStoreSize = new Fl_Knob (5, 80, 40, 40, "Num Grains");
	m_GrainStoreSize->type(Fl_Knob::DOTLIN);
	m_GrainStoreSize->color(Info->GUI_COLOUR);
    m_GrainStoreSize->labelsize(10);
	m_GrainStoreSize->maximum(MAX_GRAINSTORE_SIZE-2);
    m_GrainStoreSize->step(1);
    m_GrainStoreSize->value(1);
    m_GrainStoreSize->callback((Fl_Callback*)cb_GrainStoreSize);

	m_Density = new Fl_Knob (70, 80, 40, 40, "Density");
	m_Density->type(Fl_Knob::DOTLIN);
	m_Density->color(Info->GUI_COLOUR);
    m_Density->labelsize(10);
	m_Density->maximum(MAX_GRAINSTORE_SIZE);
    m_Density->step(1);
    m_Density->value(1);
    m_Density->callback((Fl_Callback*)cb_Density);

	end();
}

extern "C" int sprintf(char *,const char *,...);

void MasherPluginGUI::UpdateValues(SpiralPlugin *o)
{
	MasherPlugin* Plugin = (MasherPlugin*)o;
	m_Pitch->value(Plugin->GetGrainPitch());
	m_Random->value(Plugin->GetRandomness());
	m_GrainStoreSize->value(Plugin->GetGrainStoreSize());
	m_Density->value(Plugin->GetDensity());
}

inline void MasherPluginGUI::cb_Random_i(Fl_Knob* o, void* v) 
{ 
	m_GUICH->Set("Randomness",(int)o->value());
}
void MasherPluginGUI::cb_Random(Fl_Knob* o, void* v) 
{ ((MasherPluginGUI*)(o->parent()))->cb_Random_i(o,v); }

inline void MasherPluginGUI::cb_Pitch_i(Fl_Knob* o, void* v) 
{ 
	m_GUICH->Set("GrainPitch",(float)o->value());
}
void MasherPluginGUI::cb_Pitch(Fl_Knob* o, void* v) 
{ ((MasherPluginGUI*)(o->parent()))->cb_Pitch_i(o,v); }

inline void MasherPluginGUI::cb_MinGrainSize_i(Fl_Knob* o, void* v) 
{ 
	m_GUICH->Set("Period",(int)o->value()+1);
}
void MasherPluginGUI::cb_MinGrainSize(Fl_Knob* o, void* v) 
{ ((MasherPluginGUI*)(o->parent()))->cb_MinGrainSize_i(o,v); }

inline void MasherPluginGUI::cb_GrainStoreSize_i(Fl_Knob* o, void* v) 
{ 
	m_GUICH->Set("GrainStoreSize",(int)o->value()+1);
}
void MasherPluginGUI::cb_GrainStoreSize(Fl_Knob* o, void* v) 
{ ((MasherPluginGUI*)(o->parent()))->cb_GrainStoreSize_i(o,v); }

inline void MasherPluginGUI::cb_Density_i(Fl_Knob* o, void* v) 
{ 
	m_GUICH->Set("Density",(int)o->value());
}
void MasherPluginGUI::cb_Density(Fl_Knob* o, void* v) 
{ ((MasherPluginGUI*)(o->parent()))->cb_Density_i(o,v); }

const string MasherPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Top secret sample masher plugin. Granulates the incoming stream and\n"
	+ "carries out various patented algorithms on the result.\n\n"
	+ "Truth is I don't really know how it works, but makes sounds like\n"
	+ "tttttthhhIIiissssSSSSSSsssssSsSSsS.";
}
