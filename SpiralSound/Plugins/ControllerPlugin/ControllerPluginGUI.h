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
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Slider.H>
#include "ControllerPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef ControllerGUI
#define ControllerGUI

static int Numbers[MAX_CHANNELS];

class ControllerPluginGUI : public SpiralPluginGUI {
   public:
      ControllerPluginGUI(int w, int h, ControllerPlugin *o,ChannelHandler *ch,const HostInfo *Info);
      virtual void UpdateValues(SpiralPlugin *o);
      // void StreamIn(std::istream &s);
      // void StreamOut(std::ostream &s);
   protected:
      const std::string GetHelpText(const std::string &loc);
   private:
      void Clear();
      Fl_Color m_GUIColour;
      class CVGUI {
         public:
            CVGUI::CVGUI(int n, ControllerPluginGUI *p, Fl_Color SelColour);
            Fl_Group *m_SliderGroup;
            Fl_Input *m_Title, *m_Min, *m_Max;
            Fl_Slider *m_Chan;
      };
      friend class CVGUI;
      Fl_Pack *m_MainPack, *m_Buttons;
      Fl_Button *m_Add, *m_Delete;
      std::vector<CVGUI*> m_GUIVec;
      void AddCV();
      void DeleteCV();
      int m_CVCount;
      //// Callbacks ////
      inline void cb_Title_i(Fl_Input* o, void* v);
      static void cb_Title(Fl_Input* o, void* v);
      inline void cb_Max_i(Fl_Input* o, void* v);
      static void cb_Max(Fl_Input* o, void* v);
      inline void cb_Chan_i(Fl_Slider* o, void* v);
      static void cb_Chan(Fl_Slider* o, void* v);
      inline void cb_Min_i(Fl_Input* o, void* v);
      static void cb_Min(Fl_Input* o, void* v);
      inline void cb_Add_i(Fl_Button* o, void* v);
      static void cb_Add(Fl_Button* o, void* v);
      inline void cb_Delete_i(Fl_Button* o, void* v);
      static void cb_Delete(Fl_Button* o, void* v);
      // friend std::istream &operator>>(std::istream &s, ControllerPluginGUI &o);
};

// std::istream &operator>>(std::istream &s, ControllerPluginGUI &o);

#endif
