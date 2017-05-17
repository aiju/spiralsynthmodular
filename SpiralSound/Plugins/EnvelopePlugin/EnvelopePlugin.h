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

#ifndef EnvelopePLUGIN
#define EnvelopePLUGIN

static const int NUM_CHANNELS = 4;

class EnvelopePlugin : public SpiralPlugin
{
public:
 	EnvelopePlugin();
	virtual ~EnvelopePlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void StreamOut(std::ostream &s);
	virtual void StreamIn(std::istream &s);
		
	float GetAttack()           { return m_Attack;     }
	float GetDecay()            { return m_Decay;      }	
	float GetSustain()          { return m_Sustain;    }	
	float GetRelease()          { return m_Release;    }	
	float GetVolume()           { return m_Volume;     }	
	float GetTrigThresh()       { return m_TrigThresh; }
		
private:
	// Voice specific parameters
	bool   m_Trigger;
	float  m_t;
	
	// Common voice parameters
	float m_Attack;
	float m_Decay;
	float m_Sustain;
	float m_Release;
	float m_Volume;
	float m_TrigThresh;
	float m_SampleTime;
	
	friend std::istream &operator>>(std::istream &s, EnvelopePlugin &o);
	friend std::ostream &operator<<(std::ostream &s, EnvelopePlugin &o);
};
std::istream &operator>>(std::istream &s, EnvelopePlugin &o);
std::ostream &operator<<(std::ostream &s, EnvelopePlugin &o);

#endif
