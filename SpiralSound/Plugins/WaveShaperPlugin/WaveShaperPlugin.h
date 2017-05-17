/*  WaveShaper Plugin Copyleft (C) 2001 Yves Usson
 *  for SpiralSynthModular
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
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

#include "../SpiralPlugin.h"
#include <FL/Fl_Pixmap.H>

#ifndef WaveShaperPLUGIN
#define WaveShaperPLUGIN

class WaveShaperPlugin : public SpiralPlugin {
  public:
    WaveShaperPlugin();
    virtual ~WaveShaperPlugin ();
    virtual PluginInfo& Initialise (const HostInfo *Host);
    virtual SpiralGUIType* CreateGUI();
    virtual void Execute();
    virtual void ExecuteCommands();
    virtual void StreamOut(std::ostream &s);
    virtual void StreamIn(std::istream &s);
    float GetCoef(int);
    int GetWaveType(void);
    enum GUICommands { NONE, SETWAVETYPE, SETCOEF };
    struct GUIArgs {
      int WaveType, CoefNum;
      float CoefVal, *FuncPlot;
    };
  private:
    GUIArgs m_GUIArgs;
    float *m_wt, m_Coefs[6];
    int m_Wave;
    void calc (void);
    void set (int index, float v);
    friend std::istream &operator>> (std::istream &s, WaveShaperPlugin &o);
    friend std::ostream &operator<< (std::ostream &s, WaveShaperPlugin &o);
};

std::istream &operator>> (std::istream &s, WaveShaperPlugin &o);
std::ostream &operator<< (std::ostream &s, WaveShaperPlugin &o);

#endif
