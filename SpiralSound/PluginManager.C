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

#include <dlfcn.h>
#include <stdio.h>
#include "SpiralInfo.h"
#include "PluginManager.h"

PluginManager *PluginManager::m_Singleton = NULL;

PluginManager::PluginManager()
{
}
 
PluginManager::~PluginManager()
{
	UnloadAll();
}
	
PluginID PluginManager::LoadPlugin(const char *PluginName)
{
	// Make a new plugin
	HostsideInfo *NewPlugin = new HostsideInfo;
	NewPlugin->Handle=NULL;
	NewPlugin->ID=PluginError;
	
	// Attempt to open the plugin
	NewPlugin->Handle = dlopen (PluginName, RTLD_NOW);
	
    if (NewPlugin->Handle==NULL)
    {
		SpiralInfo::Alert("Error loading ["+string(PluginName)+"]: \n"+string(dlerror()));
        return PluginError;
    }
		
	// Link the neccesary functions 
	char *error;
	
	NewPlugin->CreateInstance  = (SpiralPlugin*(*)()) dlsym(NewPlugin->Handle, "SpiralPlugin_CreateInstance"); 	
	
	if ((error = dlerror()) != NULL)
    {
         SpiralInfo::Alert("Error linking to plugin "+string(PluginName)+"\n"+string(error));
         return PluginError;
    }
	
	NewPlugin->GetIcon = (char **(*)()) dlsym(NewPlugin->Handle, "SpiralPlugin_GetIcon");

    if ((error = dlerror()) != NULL)
    {
         SpiralInfo::Alert("Error linking to plugin "+string(PluginName)+"\n"+string(error));
         return PluginError;
    }

	NewPlugin->GetID  = (int(*)()) dlsym(NewPlugin->Handle, "SpiralPlugin_GetID"); 	
			
    if ((error = dlerror()) != NULL)
    {
		SpiralInfo::Alert("Error linking to plugin "+string(PluginName)+"\n"+string(error));
        return PluginError;
    }  
	     
	NewPlugin->GetGroupName  = (string(*)()) dlsym(NewPlugin->Handle, "SpiralPlugin_GetGroupName"); 	
			
    if ((error = dlerror()) != NULL)
    {
		SpiralInfo::Alert("Error linking to plugin "+string(PluginName)+"\n"+string(error));
        return PluginError;
    }       
		
	// We've succesfully open and linked the
	// plugin, so add it to the vector.
	int ID;
	ID = NewPlugin->GetID();
	NewPlugin->ID = ID;
	m_PluginVec.push_back(NewPlugin);
	
    return ID;
}

void PluginManager::UnLoadPlugin(PluginID ID)
{
	if (IsValid(ID))
    {
		dlclose(GetPlugin(ID)->Handle);
		char *error;				
	    if ((error = dlerror()) != NULL)
	    {
			SpiralInfo::Alert("Error unlinking plugin: \n"+string(error));
   		}       
	}
}

void PluginManager::UnloadAll()
{
	for (vector<HostsideInfo*>::iterator i=m_PluginVec.begin();
		 i!=m_PluginVec.end(); i++)
	{		
		dlclose((*i)->Handle);
		char *error;				
	    if ((error = dlerror()) != NULL)
	    {
			SpiralInfo::Alert("Error unlinking plugin: \n"+string(error));
   		}       
	}
}

const HostsideInfo *PluginManager::GetPlugin(PluginID ID)
{
	return GetPlugin_i(ID);
}

HostsideInfo *PluginManager::GetPlugin_i(PluginID ID)
{

	HostsideInfo *ret = NULL;

	for (vector<HostsideInfo*>::iterator i=m_PluginVec.begin();
		 i!=m_PluginVec.end(); i++)
	{
		if ((*i)->ID==ID)
		{
			ret = *i;
			break;
		}
	}

	if (!ret)
	{
		char t[256];
		sprintf(t,"%d",ID);
		SpiralInfo::Alert("Plugin "+string(t)+" not found.");
	}

	return ret;
}
	
bool PluginManager::IsValid(PluginID ID) 
{ 
	const HostsideInfo *t = GetPlugin(ID);
	return (t && t->Handle!=NULL); 
}


