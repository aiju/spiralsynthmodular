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

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "FL/fl_draw.h"
#include "FL/fl_message.h"
#include "SpiralInfo.h"
#include "../SpiralSynthPluginLocation.h"

float RandFloat (float s, float e) {
      return s+((rand()%10000/10000.0)*(e-s));
}




string SpiralInfo::LOCALE      = "EN";
int    SpiralInfo::BUFSIZE     = 512;
int    SpiralInfo::FRAGSIZE    = 256;
int    SpiralInfo::FRAGCOUNT   = 8;
int    SpiralInfo::SAMPLERATE  = 44100;
long   SpiralInfo::MAXSAMPLE   = 32767;
float  SpiralInfo::VALUECONV   = 1.0f/MAXSAMPLE;
bool   SpiralInfo::WANTMIDI    = false;
int    SpiralInfo::FILTERGRAN  = 50;
string SpiralInfo::OUTPUTFILE  = "/dev/dsp";
string SpiralInfo::MIDIFILE    = "/dev/midi";
int    SpiralInfo::POLY        = 1;
bool   SpiralInfo::USEPLUGINLIST = false;
unsigned SpiralInfo::GUI_COLOUR = 179;
unsigned SpiralInfo::SCOPE_BG_COLOUR = fl_rgb_color (20, 60, 20);
unsigned SpiralInfo::SCOPE_FG_COLOUR = fl_rgb_color (100, 200, 100);
unsigned SpiralInfo::SCOPE_SEL_COLOUR = FL_WHITE;
unsigned SpiralInfo::SCOPE_IND_COLOUR = fl_rgb_color (203, 255, 0);
unsigned SpiralInfo::SCOPE_MRK_COLOUR = fl_rgb_color (155, 155, 50);

/*int SpiralInfo::GUICOL_Tool=179;
int SpiralInfo::GUICOL_Button=181;
int SpiralInfo::GUICOL_Canvas=181;
int SpiralInfo::GUICOL_Device=181;
int SpiralInfo::GUIDEVICE_Box=30;*/

int SpiralInfo::GUICOL_Tool=48;
int SpiralInfo::GUICOL_Button=42;
int SpiralInfo::GUICOL_Canvas=50;
int SpiralInfo::GUICOL_Device=52;
int SpiralInfo::GUIDEVICE_Box=30;

vector<string> SpiralInfo::PLUGINVEC;

string SpiralInfo::PLUGIN_PATH;

SpiralInfo* SpiralInfo::m_SpiralInfo=NULL;

SpiralInfo* SpiralInfo::Get() {
   if (!m_SpiralInfo) {
      m_SpiralInfo = new SpiralInfo;
   }
   return m_SpiralInfo;
}

SpiralInfo::SpiralInfo():
m_Version(1)
{
   m_ResFileName = getenv("HOME");
   m_ResFileName += "/.spiralmodular";
   PLUGIN_PATH = PLUGIN_PATH_LOCATION;
}

void SpiralInfo::LoadPrefs() {
     ifstream i (m_ResFileName.c_str());
     if (!i) {
        cerr << "Creating " << m_ResFileName << endl;
        SavePrefs ();
        return;
     }
     StreamInPrefs (i);
}

void SpiralInfo::SavePrefs() {
     ofstream o (m_ResFileName.c_str());
     StreamOutPrefs (o);
}

void SpiralInfo::StreamInPrefs (istream &s) {
     // These lines are from SpiralInfo
     char temp[256];
     s >> temp >> temp >> temp;
     s >> temp >> temp >> m_Version;
     s >> temp >> temp >> LOCALE;
     s >> temp >> temp >> BUFSIZE;
     s >> temp >> temp >> FRAGSIZE;
     s >> temp >> temp >> FRAGCOUNT;
     s >> temp >> temp >> SAMPLERATE;
     s >> temp >> temp >> WANTMIDI;
     s >> temp >> temp >> FILTERGRAN;
     s >> temp >> temp >> OUTPUTFILE;
     s >> temp >> temp >> MIDIFILE;
     s >> temp >> temp >> USEPLUGINLIST;
     s >> temp >> temp >> POLY;
     if (m_Version>0) {
        s >> temp >> temp >> GUI_COLOUR;
        s >> temp >> temp >> SCOPE_BG_COLOUR;
        s >> temp >> temp >> SCOPE_FG_COLOUR;
        s >> temp >> temp >> SCOPE_SEL_COLOUR;
        s >> temp >> temp >> SCOPE_IND_COLOUR;
        s >> temp >> temp >> SCOPE_MRK_COLOUR;
     }
     s >> temp >> temp;
     s >> temp >> temp >> GUICOL_Tool;
     s >> temp >> temp >> GUICOL_Button;
     s >> temp >> temp >> GUICOL_Canvas;
     s >> temp >> temp >> GUICOL_Device;
     s >> temp >> temp >> GUIDEVICE_Box;
     s >> temp >> temp >> PLUGIN_PATH;
     s >> temp >> temp;
     string st;
     PLUGINVEC.clear();
     if (USEPLUGINLIST) {
        while (st!="end" && !s.eof()) {
              s >> st;
              if (st!="end") PLUGINVEC.push_back (st);
        }
     }

#if __APPLE__
     // ignore custom paths, plugins are encapsulated in the app anyway
     // this prevents the program to fail if the user move the application icon
     PLUGIN_PATH = PLUGIN_PATH_LOCATION;
#endif
}

void SpiralInfo::StreamOutPrefs (ostream &s) {
     // These lines are from SpiralInfo
     s << "SpiralSound resource file" << endl << endl;
     s << "Version           = " << m_Version << endl;
     s << "Locale            = " << LOCALE << endl;
     s << "BufferSize        = " << BUFSIZE << endl;
     s << "FragmentSize      = " << FRAGSIZE << endl;
     s << "FragmentCount     = " << FRAGCOUNT << endl;
     s << "Samplerate        = " << SAMPLERATE << endl;
     s << "WantMidi          = " << WANTMIDI << endl;
     s << "FilterGranularity = " << FILTERGRAN << endl;
     s << "Output            = " << OUTPUTFILE << endl;
     s << "Midi              = " << MIDIFILE << endl;
     s << "UsePluginList     = " << USEPLUGINLIST << endl;
     s << "Polyphony         = " << POLY << endl;
     s << "GUIColour         = " << GUI_COLOUR << endl;
     s << "ScopeBGColour     = " << SCOPE_BG_COLOUR << endl;
     s << "ScopeFGColour     = " << SCOPE_FG_COLOUR << endl;
     s << "ScopeSelColour    = " << SCOPE_SEL_COLOUR << endl;
     s << "ScopeIndColour    = " << SCOPE_IND_COLOUR << endl;
     s << "ScopeMrkColour    = " << SCOPE_MRK_COLOUR << endl;
     s << endl << "SpiralSynthModular Info:" << endl << endl;
     s << "ToolBoxColour     = " << GUICOL_Tool << endl;
     s << "ButtonColour      = " << GUICOL_Button << endl;
     s << "CanvasColour      = " << GUICOL_Canvas << endl;
     s << "DeviceColour      = " << GUICOL_Device << endl;
     s << "DeviceBoxType     = " << GUIDEVICE_Box << endl << endl;
     s << "PluginPath        = " << PLUGIN_PATH << endl << endl;
     s << "PluginList        = " << endl;
     for (vector<string>::iterator i=PLUGINVEC.begin(); i!=PLUGINVEC.end(); i++) {
         s << *i << endl;
     }
     s << "end" << endl;
}

void SpiralInfo::Alert (string Text) {
     // fl_message (Text.c_str());
     cerr << "Spiral alert: " << Text << endl;
}

void SpiralInfo::Log (string Text) {

}
