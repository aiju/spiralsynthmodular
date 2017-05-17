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

#include "KeyboardPlugin.h"
#include "KeyboardPluginGUI.h"
#include "../../NoteTable.h"
#include "SpiralIcon.xpm"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new KeyboardPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 40;
}

string SpiralPlugin_GetGroupName()
{
	return "InputOutput";
}
}

///////////////////////////////////////////////////////

KeyboardPlugin::KeyboardPlugin() :
m_NoteLevel(0),
m_TriggerLevel(0)
{
	m_Version=0;

	m_PluginInfo.Name="Keyboard";
	m_PluginInfo.Width=300;
	m_PluginInfo.Height=90;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=2;
	m_PluginInfo.PortTips.push_back("Note CV");
	m_PluginInfo.PortTips.push_back("Trigger CV");
		
	m_AudioCH->Register("Note",&m_GUIArgs.Note);
}

KeyboardPlugin::~KeyboardPlugin()
{
}

SpiralGUIType *KeyboardPlugin::CreateGUI()
{
	return new KeyboardPluginGUI(m_PluginInfo.Width,
										  m_PluginInfo.Height,
										  this,m_AudioCH,m_HostInfo);
}

void KeyboardPlugin::Execute()
{
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		SetOutputPitch(0,n,m_NoteLevel);
		SetOutput(1,n,m_TriggerLevel);				
	}
}

void  KeyboardPlugin::ExecuteCommands()
{
	// Process any commands from the GUI
	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
			case NOTE_ON  : 
				m_NoteLevel=NoteTable[m_GUIArgs.Note]; 
				m_TriggerLevel=1.0f; 
				break;
			case NOTE_OFF : m_TriggerLevel=0.0f; break;
		};
	}
}

void KeyboardPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<endl;
}

void KeyboardPlugin::StreamIn(istream &s) 
{
	int version;
	s>>version;
}

