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

#include "ScopePluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

using namespace std;

ScopeWidget::ScopeWidget (int x, int y, int w, int h, const char *l, int BUFSIZE) :
Fl_Widget (x, y, w, h, l),
m_Data (NULL),
//m_Channels (1),
m_WaveColour (FL_WHITE),
m_Attenuation (1.0),
m_TimeBase (1.0),
m_Bufsize (BUFSIZE)
{
	m_Data = new float[BUFSIZE];
}

ScopeWidget::~ScopeWidget()
{
	delete[] m_Data;
}

void ScopeWidget::draw() {
     int ho=h()/2;
     fl_color (color());
     fl_rectf (x(), y(), w(), h());
     fl_color (m_MarkColour);
     fl_line (x(), y()+ho, x()+w(), y()+ho);
     if (!m_Data) return;
     fl_push_clip (x(), y(), w(), h());
     float Value=0, NextValue=0;
     fl_color (m_WaveColour);
     for (int n=0; /*n<m_Bufsize-1 &&*/ n<w(); n++) {
         int p = int ((float)n*m_TimeBase);
         if (p>=m_Bufsize) break;
         Value = NextValue;
         NextValue = m_Data[p] * m_Attenuation * ho;
         fl_line ((int)(x()+n-2), (int)(y()+ho-Value), (int)(x()+n-1), (int)(y()+ho-NextValue));
     }
     fl_pop_clip();
}

////////////////////////////////////////////

ScopePluginGUI::ScopePluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
m_Bypass(false)
{
        m_BufSize = Info->BUFSIZE;
        m_Scope = new ScopeWidget(5, 20, 210, 85, "Scope", m_BufSize);
        m_Scope->color (Info->SCOPE_BG_COLOUR);
        m_Scope->SetColours (Info->SCOPE_MRK_COLOUR, Info->SCOPE_FG_COLOUR);
        m_Attenuation = new Fl_Knob (220, 10, 40, 40, "Attenuation");
        m_Attenuation->color(Info->GUI_COLOUR);
        m_Attenuation->type(Fl_Knob::LINELIN);
        m_Attenuation->labelsize (9);
        m_Attenuation->maximum (1);
        m_Attenuation->step (.001);
        m_Attenuation->value (0);
        m_Attenuation->callback ((Fl_Callback*)cb_Attenuation);
        m_TimeBase = new Fl_Knob (220, 60, 40, 40, "Time Base");
        m_TimeBase->color(Info->GUI_COLOUR);
        m_TimeBase->type(Fl_Knob::LINELIN);
        m_TimeBase->labelsize (9);
        m_TimeBase->minimum (0.001);
        //m_TimeBase->maximum (5); // It'd be better if we could do this, but it makes the display look crummy
        m_TimeBase->maximum (1);
        m_TimeBase->step (.001);
        m_TimeBase->value (1);
        m_TimeBase->callback ((Fl_Callback*)cb_TimeBase);
 	/*Bypass = new Fl_Button(175, 107, 40, 16, "Bypass");
  	Bypass->labelsize(10);
        Bypass->type(1);
  	Bypass->callback((Fl_Callback*)cb_Bypass);*/
	end();
}

void ScopePluginGUI::Update()
{
	redraw();
}

void ScopePluginGUI::draw()
{
    SpiralGUIType::draw();
    const float *data;
    //cerr<<"getting and drawing..."<<endl;
    if (m_GUICH->GetBool ("DataReady")) m_GUICH->GetData ("AudioData", (void*)m_Scope->m_Data);
    else memset ((void*)m_Scope->m_Data, 0, m_BufSize * sizeof (float));
    if (!m_Bypass) m_Scope->redraw();
}

void ScopePluginGUI::UpdateValues(SpiralPlugin* o)
{
}

inline void ScopePluginGUI::cb_Attenuation_i (Fl_Knob* o, void* v)
{
   m_Scope->SetAttenuation (1.0-(o->value()));
}

void ScopePluginGUI::cb_Attenuation (Fl_Knob* o, void* v) {
     ((ScopePluginGUI*)(o->parent()))->cb_Attenuation_i (o, v);
}

inline void ScopePluginGUI::cb_TimeBase_i (Fl_Knob* o, void* v)
{
   m_Scope->SetTimeBase (o->value());
}

void ScopePluginGUI::cb_TimeBase (Fl_Knob* o, void* v) {
     ((ScopePluginGUI*)(o->parent()))->cb_TimeBase_i (o, v);
}

//void ScopePluginGUI::cb_Bypass_i(Fl_Button* o, void* v)
//{m_Bypass=o->value();}
//void ScopePluginGUI::cb_Bypass(Fl_Button* o, void* v)
//{((ScopePluginGUI*)(o->parent()))->cb_Bypass_i(o,v);}

const string ScopePluginGUI::GetHelpText(const string &loc){
    return string("")
    + "The Scope lets you see a visual representation of the\n"
    + "data flowing through it. It does nothing to the signal,\n"
    + "but its very useful for checking the layouts, looking at\n"
    + "CV value etc.\n";
}
