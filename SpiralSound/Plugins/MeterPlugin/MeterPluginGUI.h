/*  SpiralPlugin
 *  Copyleft (C) 2002 Andy Preston <andy@clublinux.co.uk>
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

#ifndef METERGUI
#define METERGUI

#include <sstream>
#include <FL/Fl.H>
#include "../Widgets/Fl_VU_Meter.h"
#include "../Widgets/Fl_SevenSeg.H"
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include "MeterPlugin.h"
#include "../SpiralPluginGUI.h"

class MeterPluginGUI : public SpiralPluginGUI {
  public:
    MeterPluginGUI (int w, int h, MeterPlugin *o, ChannelHandler *ch, const HostInfo *Info);
    ~MeterPluginGUI ();
    virtual void UpdateValues (SpiralPlugin* o);
    virtual void Update ();
    virtual void draw ();
  protected:
    const std::string GetHelpText (const std::string &loc);
  private:
    bool m_Bypass;
    float *m_Data, m_Min, m_Max;
    int m_BufSize;
    void SetMinMax (float NewMin, float NewMax);
    inline void DoReset (void);
    Fl_SevenSeg *Digits[8];
    Fl_Output *MaxBox, *MinBox;
    Fl_Button *Reset, *Bypass, *VUMode, *MMMode;
    Fl_VU_Meter *Meter;
    inline void cb_Reset_i (Fl_Button* o, void* v);
    static void cb_Reset (Fl_Button* o, void* v);
    inline void cb_Bypass_i (Fl_Button* o, void* v);
    static void cb_Bypass (Fl_Button* o, void* v);
    inline void cb_Mode_i (Fl_Button* o, void* v);
    static void cb_Mode (Fl_Button* o, void* v);
};

#endif
