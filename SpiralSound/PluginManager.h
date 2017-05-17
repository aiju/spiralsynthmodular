/*  SpiralSound
 *  Copyleft (C) 2002 David Griffiths <dave@pawfal.org>
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

#include <FL/Fl_Double_Window.h>
#include <FL/Fl_Pixmap.h>
#include <iostream>
#include <vector>
#include "Plugins/SpiralPlugin.h"

struct HostsideInfo
{
	void* Handle;
	int   ID;
	char **(*GetIcon)(void);
	SpiralPlugin *(*CreateInstance)(void);
	int           (*GetID)(void);
	string        (*GetGroupName)(void);
};

//////////////////////////////////////////////////////////

typedef int PluginID;
#define     PluginError -1

class PluginManager
{
public:
	static PluginManager *Get() { if(!m_Singleton) m_Singleton=new PluginManager; return m_Singleton; }
	static void         PackUpAndGoHome() { if(m_Singleton) delete m_Singleton; }

	PluginID            LoadPlugin(const char *PluginName);
	void                UnLoadPlugin(PluginID ID);
	void                UnloadAll();
	const HostsideInfo* GetPlugin(PluginID ID);
	bool                IsValid(PluginID ID);
	int					GetIdByName(string Name);
				   
private:

	PluginManager();
	~PluginManager();
	HostsideInfo *GetPlugin_i(PluginID ID);
	
	vector<HostsideInfo*> m_PluginVec;
	static PluginManager *m_Singleton;
};	
