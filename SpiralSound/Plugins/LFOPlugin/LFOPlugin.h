/*  SpiralSound
 *  Copyleft (C) 2002 Andy Preston <andy@clubunix.co.uk>
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

#ifndef LFOPLUGIN
#define LFOPLUGIN

#include "../SpiralPlugin.h"
#include <FL/Fl_Pixmap.H>

static const int NUM_TABLES = 6;
static const int DEFAULT_TABLE_LEN = 1024;

class LFOPlugin : public SpiralPlugin {
   public:
      LFOPlugin();
      virtual ~LFOPlugin();
      virtual PluginInfo &Initialise (const HostInfo *Host);
      virtual SpiralGUIType* CreateGUI();
      virtual void Execute();
      virtual void StreamOut (std::ostream &s);
      virtual void StreamIn (std::istream &s);
      //typedef char Type;
      enum Type {SINE, TRIANGLE, SQUARE, SAW};
      void WriteWaves();
      void NoteTrigger (int V, int s, int v);
      float GetFreq() { return m_Freq; }
      Type GetType() { return m_Type; }
   private:
      float AdjustPos (float pos);
      // Voice specific parameter
      int m_Note;
      float m_CyclePos;
      // Common voice parameters
      Type m_Type;
      float m_Freq;
      Sample m_Table[NUM_TABLES];
      int m_TableLength;
      friend std::istream &operator>> (std::istream &s, LFOPlugin &o);
      friend std::ostream &operator<< (std::ostream &s, LFOPlugin &o);
};

std::istream &operator>> (std::istream &s, LFOPlugin &o);
std::ostream &operator<< (std::ostream &s, LFOPlugin &o);

#endif
