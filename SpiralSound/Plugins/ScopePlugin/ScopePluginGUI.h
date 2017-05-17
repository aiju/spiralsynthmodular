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

#ifndef SCOPEGUI
#define SCOPEGUI

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include "../Widgets/Fl_Knob.H"
#include "ScopePlugin.h"
#include "../SpiralPluginGUI.h"

class ScopeWidget : public Fl_Widget
{
   public:
      ScopeWidget (int x, int y, int w, int h, const char *l, int BUFSIZE);
      ~ScopeWidget();
      void draw();
      //void SetNumChannels (int s) { m_Channels=s; }
      void SetColours (unsigned m, unsigned f) { m_MarkColour=(Fl_Color)m; m_WaveColour=(Fl_Color)f; }
      void SetAttenuation (float c) { m_Attenuation=c; }
      void SetTimeBase (float c) { m_TimeBase=c; }
      const float *m_Data;
      //int m_Channels;
   private:
      //int m_GUIColour, m_GUIBGColour;
      Fl_Color m_MarkColour, m_WaveColour;
      float m_Attenuation, m_TimeBase;
      int m_Bufsize;
};


class ScopePluginGUI : public SpiralPluginGUI
{
   public:
      ScopePluginGUI (int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual void UpdateValues (SpiralPlugin* o);
      virtual void Update();
      virtual void draw();
   protected:
      const std::string GetHelpText (const std::string &loc);
   private:
      bool m_Bypass;
      int m_BufSize;
      // Fl_Button *Bypass;
      ScopeWidget *m_Scope;
      Fl_Knob *m_Attenuation, *m_TimeBase;
      //// Callbacks ////
      inline void cb_Attenuation_i (Fl_Knob* o, void* v);
      static void cb_Attenuation (Fl_Knob* o, void* v);
      inline void cb_TimeBase_i (Fl_Knob* o, void* v);
      static void cb_TimeBase (Fl_Knob* o, void* v);
      // inline void cb_Bypass_i(Fl_Button* o, void* v);
      // static void cb_Bypass(Fl_Button* o, void* v);
};

#endif
