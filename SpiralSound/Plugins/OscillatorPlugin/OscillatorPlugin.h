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

#ifndef OscillatorPLUGIN
#define OscillatorPLUGIN

class OscillatorPlugin : public SpiralPlugin
{
public:
 	OscillatorPlugin();
	virtual ~OscillatorPlugin();

	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType  *CreateGUI();
	virtual void 		Execute();
	virtual void	    StreamOut(std::ostream &s);
	virtual void	    StreamIn(std::istream &s);

	typedef char Type;
	enum {NONE,SQUARE,SAW,NOISE};

	void ModulateFreq(Sample *data) {m_FreqModBuf=data;}
	void ModulatePulseWidth(Sample *data) {m_PulseWidthModBuf=data;}
	void ModulateSHLen(Sample *data) {m_SHModBuf=data;}

	void NoteTrigger(int V,int s,int v);
	int   GetOctave() {return m_Octave;}
	float GetFineFreq() {return m_FineFreq;}
	float GetPulseWidth() {return m_PulseWidth;}
	Type  GetType() {return m_Type;}
	float GetSHLen() {return m_SHLen;}
	float GetModAmount() {return m_ModAmount;}

private:

	// Voice specific parameter
	int   m_Note;
	int   m_CyclePos;
	float m_LastFreq;

        // Common voice parameters
	Type   m_Type;
	int    m_Octave;
	float  m_FineFreq;
	float  m_PulseWidth;
	float  m_SHLen;
	float  m_ModAmount;
	short  m_Noisev;

	Sample *m_FreqModBuf;
	Sample *m_PulseWidthModBuf;
	Sample *m_SHModBuf;

	static const int FIXED;

	friend std::istream &operator>>(std::istream &s, OscillatorPlugin &o);
	friend std::ostream &operator<<(std::ostream &s, OscillatorPlugin &o);
};

std::istream &operator>>(std::istream &s, OscillatorPlugin &o);
std::ostream &operator<<(std::ostream &s, OscillatorPlugin &o);

#endif
