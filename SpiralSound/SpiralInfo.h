/*  SpiralSound
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

#ifndef SpiralINFO
#define SpiralINFO

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "Sample.h"

using namespace std;

// todo: better place for these util funcs
float RandFloat (float s=0.0f, float e=1.0f);

// Loads info from the resource file

class SpiralInfo {
   public:
      SpiralInfo();
      ~SpiralInfo() {}
      void LoadPrefs();
      void SavePrefs();
      static void Alert (string Text);
      static void Log (string Text);
      static int    BUFSIZE;
      static int    FRAGSIZE;
      static int    FRAGCOUNT;
      static int    SAMPLERATE;
      static long   MAXSAMPLE;
      static float  VALUECONV;
      static bool   WANTMIDI;
      static int    FILTERGRAN;
      static string OUTPUTFILE;
      static string MIDIFILE;
      static bool   USEPLUGINLIST;
      static int    POLY;
      static string LOCALE;
      static unsigned GUI_COLOUR;
      static unsigned SCOPE_BG_COLOUR;
      static unsigned SCOPE_FG_COLOUR;
      static unsigned SCOPE_SEL_COLOUR;
      static unsigned SCOPE_IND_COLOUR;
      static unsigned SCOPE_MRK_COLOUR;
      static SpiralInfo* Get();
      void SetColours();
      static string PLUGIN_PATH;
      static vector<string> PLUGINVEC;
      static int GUICOL_Tool;
      static int GUICOL_Button;
      static int GUICOL_Canvas;
      static int GUICOL_Device;
      static int GUIDEVICE_Box;
   private:
      string m_ResFileName;
      int m_Version;
      static SpiralInfo *m_SpiralInfo;
      void StreamInPrefs (istream &s);
      void StreamOutPrefs (ostream &s);
};

#endif

