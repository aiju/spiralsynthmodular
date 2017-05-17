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
#include "iir_filter.h"

#ifndef FilterPLUGIN
#define FilterPLUGIN

static const int NUM_CHANNELS = 4;

class FilterPlugin : public SpiralPlugin
{
public:
 	FilterPlugin();
	virtual ~FilterPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void StreamOut(std::ostream &s);
	virtual void StreamIn(std::istream &s);
		
	void Reset();
	void SetupCoeffs();

	double GetCutoff() {return fc;}
	double GetResonance() {return Q;}
	bool GetRevCutoffMod() {return m_RevCutoffMod;}
	bool GetRevResonanceMod() {return m_RevResonanceMod;}
	
private:
	// Voice specifics
	FILTER   iir; 
   	
	// Voice independant
	float    *coef; 
    float   fs, fc;     // Sampling frequency, cutoff frequency  
    float   Q;          // Resonance > 1.0 < 1000  
	float   m_LastFC;
	float   m_LastQ;
    unsigned nInd; 
    double   a0, a1, a2, b0, b1, b2; 
    double   k;          // overall gain factor 
	bool	 m_RevCutoffMod;
	bool	 m_RevResonanceMod;

	friend std::istream &operator>>(std::istream &s, FilterPlugin &o);
	friend std::ostream &operator<<(std::ostream &s, FilterPlugin &o);
};
std::istream &operator>>(std::istream &s, FilterPlugin &o);
std::ostream &operator<<(std::ostream &s, FilterPlugin &o);

#endif
