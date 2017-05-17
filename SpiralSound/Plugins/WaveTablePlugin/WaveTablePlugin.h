/*  SpiralSound
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

#ifndef WaveTablePLUGIN
#define WaveTablePLUGIN

static const int NUM_TABLES = 8;
static const int DEFAULT_TABLE_LEN = 1024;

class WaveTablePlugin : public SpiralPlugin
{
public:
 	WaveTablePlugin();
	virtual ~WaveTablePlugin();

	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void 		Execute();
	virtual void	    StreamOut(std::ostream &s);
	virtual void	    StreamIn(std::istream &s);
	
	// has to be defined in the plugin	
	virtual void UpdateGUI() { Fl::check(); }

	typedef char Type;
	enum {SINE,SQUARE,SAW,REVSAW,TRIANGLE,PULSE1,PULSE2,INVSINE};
	
	void WriteWaves();
	void NoteTrigger(int V,int s,int v);
	
	Type GetType()      { return m_Type; }
	int  GetOctave()    { return m_Octave; }
	float GetFineFreq() { return m_FineFreq; } 
	float GetModAmount(){ return m_ModAmount; }
	
private:

	// Voice specific parameter
	int   m_Note;
	float m_CyclePos;
	
	// Common voice parameters
	Type   m_Type;
	int    m_Octave;
	float  m_FineFreq;
	float  m_ModAmount;
		
	Sample m_Table[NUM_TABLES];
	int    m_TableLength;
	
	friend std::istream &operator>>(std::istream &s, WaveTablePlugin &o);
	friend std::ostream &operator<<(std::ostream &s, WaveTablePlugin &o);
};

std::istream &operator>>(std::istream &s, WaveTablePlugin &o);
std::ostream &operator<<(std::ostream &s, WaveTablePlugin &o);

#endif
