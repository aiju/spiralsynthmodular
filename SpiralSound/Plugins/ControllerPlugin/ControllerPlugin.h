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

#ifndef ControllerPLUGIN
#define ControllerPLUGIN

static const int MAX_CHANNELS = 99;

class ControllerPlugin : public SpiralPlugin
{
public:
 	ControllerPlugin();
	virtual ~ControllerPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void ExecuteCommands();
	virtual void StreamOut(std::ostream &s);
	virtual void StreamIn(std::istream &s);

	std::string GetName(int n) { return m_Names[n]; }
	int    GetNum()       { return m_Num; }
	float  GetVal(int n)  { return m_ChannelVal[n]; }
	float  GetMin(int n)  { return m_MinVal[n]; }
	float  GetMax(int n)  { return m_MaxVal[n]; }

	enum GUICommands{NONE,SETNUM,SETALL,SETNAME,SETMAX,SETCHANNEL,SETMIN};
	struct GUIArgs
	{
		int Number;
		float Value;
		float Min;
		float Max;
		char Name[256];
	};

private:
	GUIArgs m_GUIArgs;

	void SetNum(int n);
	void SetAll(int n, char *name, float max, float value, float min)
		{ m_Names[n]      = name;
		  m_MaxVal[n]     = max;
		  m_ChannelVal[n] = value;
		  m_MinVal[n]     = min;
		};
	void SetName(int n, char* name)     { m_Names[n]      = name;  };
	void SetChannel(int n, float value) { m_ChannelVal[n] = value; };
	void SetMax(int n, float max)       { m_MaxVal[n]     = max;   };
	void SetMin(int n, float min)       { m_MinVal[n]     = min;   };
	void Clear();

	int m_Num;
	float  m_ChannelVal[MAX_CHANNELS];
	std::string m_Names[MAX_CHANNELS];
	float  m_MinVal[MAX_CHANNELS];
	float  m_MaxVal[MAX_CHANNELS];
};

#endif
