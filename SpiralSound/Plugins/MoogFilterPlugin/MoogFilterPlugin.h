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

#ifndef MOOGFILTERPLUGIN
#define MOOGFILTERPLUGIN

class MoogFilterPlugin : public SpiralPlugin
{
public:
 	MoogFilterPlugin();
	virtual ~MoogFilterPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void StreamOut(std::ostream &s);
	virtual void StreamIn(std::istream &s);
	
	float GetCutoff()    { return Cutoff; }
	float GetResonance() { return Resonance;  } 
	
	void Clear();
	void Randomise();

private:
	float Cutoff, Resonance;
	
	float fs, fc;
	float f,p,q;
	float b0,b1,b2,b3,b4;
	float t1,t2;
	
	float in1,in2,in3,in4,out1,out2,out3,out4;
	
	friend std::istream &operator>>(std::istream &s, MoogFilterPlugin &o);
	friend std::ostream &operator<<(std::ostream &s, MoogFilterPlugin &o);
};
std::istream &operator>>(std::istream &s, MoogFilterPlugin &o);
std::ostream &operator<<(std::ostream &s, MoogFilterPlugin &o);

#endif
