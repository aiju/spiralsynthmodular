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

#ifndef SPIRALPLUGINGUI
#define SPIRALPLUGINGUI

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <iostream>
#include <math.h>
#include "SpiralPlugin.h" // for the channel handler
#include "../../GUI/Widgets/SpiralGUI.H"

class SpiralPluginGUI : public SpiralGUIType {
   public:
      SpiralPluginGUI (int w, int h, SpiralPlugin* o, ChannelHandler *ch);
      ~SpiralPluginGUI ();
      void Resize (int neww, int newh);
      virtual void Update ();
      // called while audio thread is suspended, so direct access to the spiralplugin is acceptable
      virtual void UpdateValues (SpiralPlugin *o) = 0;
   protected:
      ChannelHandler *m_GUICH;
      virtual const std::string GetHelpText (const std::string &loc);
   private:
      Fl_Button *m_Hide, *m_Help;
      static Fl_Double_Window* m_HelpWin;
      static Fl_Text_Display* m_HelpWin_text;
      static SpiralPluginGUI* Help_owner;
      std::string m_Title;
      //// Callbacks ////
      inline void cb_Hide_i (Fl_Button *o, void *v);
      static void cb_Hide (Fl_Button *o, void *v);
      inline void cb_Help_i (Fl_Button *o, void *v);
      static void cb_Help (Fl_Button *o, void *v);
      inline void cb_Help_close_i (Fl_Double_Window *w, void *v);
      static void cb_Help_close (Fl_Double_Window *w, void *v);
};

#endif
