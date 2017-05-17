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

#ifndef MixerGUI
#define MixerGUI

#include <FL/Fl.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Slider.H>
#include "../Widgets/Fl_LED_Button.H"
#include "MixerPlugin.h"
#include "../SpiralPluginGUI.h"

static int Numbers[MAX_CHANNELS];

class MixerPluginGUI : public SpiralPluginGUI {
   public:
      MixerPluginGUI (int w, int h, MixerPlugin *o, ChannelHandler *ch, const HostInfo *Info);
      virtual void UpdateValues(SpiralPlugin *o);
      virtual void Update ();
   protected:
      const std::string GetHelpText(const std::string &loc);
   private:
      void AddChan (bool SendData = false, bool ResizeIt = false);
      void DeleteChan (bool SendData = true);
      std::vector<Fl_Slider*> m_SlidVec;
      Fl_Pack *m_MainPack, *m_Buttons;
      Fl_Button *m_Add, *m_Delete;
      Fl_LED_Button *m_PeakInd;
      Fl_Color m_GUIColour;
      //// Callbacks ////
      inline void cb_Chan_i (Fl_Slider* o, void* v);
      static void cb_Chan (Fl_Slider* o, void* v);
      inline void cb_Add_i (Fl_Button* o, void* v);
      static void cb_Add (Fl_Button* o, void* v);
      inline void cb_Delete_i (Fl_Button* o, void* v);
      static void cb_Delete (Fl_Button* o, void* v);
};

#endif
