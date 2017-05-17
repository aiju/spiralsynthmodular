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
#include <FL/Fl.h>

#ifndef MasherPLUGIN
#define MasherPLUGIN

static const int MAX_GRAINSTORE_SIZE = 1000;

class MasherPlugin : public SpiralPlugin
{
public:
 	MasherPlugin();
	virtual ~MasherPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void StreamOut(std::ostream &s);
	virtual void StreamIn(std::istream &s);
	
	// has to be defined in the plugin	
	virtual void UpdateGUI() { Fl::check(); }
		
	float GetGrainPitch()       { return m_GrainPitch; }
	int GetGrainStoreSize()     { return m_GrainStoreSize;  } 
	int GetDensity()     		{ return m_Density;  } 
	int GetRandomness()     	{ return m_Randomness;  } 
	
	void Randomise();

private:
	int m_GrainStoreSize;
	int m_Density;
	int m_Randomness;
	float m_GrainPitch;
	
	int m_ReadGrain;
	int m_WriteGrain;
	
	Sample m_GrainStore[MAX_GRAINSTORE_SIZE];
	
	struct GrainDesc
	{
		int Pos;
		int Grain;
	};
	
	std::vector<GrainDesc> m_OverlapVec;
	
	friend std::istream &operator>>(std::istream &s, MasherPlugin &o);
	friend std::ostream &operator<<(std::ostream &s, MasherPlugin &o);
};
std::istream &operator>>(std::istream &s, MasherPlugin &o);
std::ostream &operator<<(std::ostream &s, MasherPlugin &o);

#endif
