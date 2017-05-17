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

#ifndef SVFILTERPLUGIN
#define SVFILTERPLUGIN

static const int NUM_CHANNELS = 4;

// A State Variable Filter
class SVFilterPlugin : public SpiralPlugin
{
public:
 	SVFilterPlugin();
	virtual ~SVFilterPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void StreamOut(std::ostream &s);
	virtual void StreamIn(std::istream &s);
	
	float GetCutoff()    { return Cutoff; }
	float GetResonance() { return Resonance;  } 
	
	void Randomise();
	void Clear();

private:
	float Cutoff, Resonance;
	
	double fs; // Sample Rate
	double fc; // Cutoff
	double q;  // Resonance
	
	// Calculation
	double m_f, m_q, m_qnrm;
	 
	// Outputs
	double m_h, m_b, m_l, m_p, m_n;
		
	friend std::istream &operator>>(std::istream &s, SVFilterPlugin &o);
	friend std::ostream &operator<<(std::ostream &s, SVFilterPlugin &o);
};
std::istream &operator>>(std::istream &s, SVFilterPlugin &o);
std::ostream &operator<<(std::ostream &s, SVFilterPlugin &o);

#endif
