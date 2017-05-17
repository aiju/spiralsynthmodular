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

#include "MidiPlugin.h"
#include "MidiPluginGUI.h"
#include <FL/Fl_Button.H>
#include "../../NoteTable.h"
#include "../../Midi.h"
#include "SpiralIcon.xpm"

using namespace std;

int MidiPlugin::m_RefCount = 0;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new MidiPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0002;
}

string SpiralPlugin_GetGroupName()
{
	return "InputOutput";
}
}

///////////////////////////////////////////////////////

MidiPlugin::MidiPlugin() :
m_DeviceNum(0),
m_NoteLevel(0),
m_TriggerLevel(0),
m_PitchBendLevel(0),
m_ChannelPressureLevel(0),
m_AfterTouchLevel(0),
m_NoteCut(false),
m_ContinuousNotes(false),
m_CurrentNote(0)
{
	m_Version=2;
	
	if (m_RefCount==0)
	{
		MidiDevice::Init("SpiralModular",MidiDevice::READ);
	}
	
	m_RefCount++;

	m_PluginInfo.Name="Midi";
	m_PluginInfo.Width=80;
	m_PluginInfo.Height=140;
	m_PluginInfo.NumInputs=2;
	m_PluginInfo.NumOutputs=6;
	m_PluginInfo.PortTips.push_back("Note CV");
	m_PluginInfo.PortTips.push_back("Trigger CV");
	m_PluginInfo.PortTips.push_back("Note CV");
	m_PluginInfo.PortTips.push_back("Trigger CV");
	m_PluginInfo.PortTips.push_back("PitchBend CV");
	m_PluginInfo.PortTips.push_back("ChannelPressure CV");
	m_PluginInfo.PortTips.push_back("Aftertouch CV");
	m_PluginInfo.PortTips.push_back("Clock CV");
	
	for (int n=0; n<128; n++) m_ControlLevel[n]=0;
	
	m_AudioCH->Register("DeviceNum",&m_DeviceNum);
	m_AudioCH->Register("NoteCut",&m_NoteCut);
	m_AudioCH->Register("CC",&m_GUIArgs.s);
	m_AudioCH->RegisterData("Name",ChannelHandler::INPUT,
		&m_GUIArgs.Name,sizeof(m_GUIArgs.Name));
}

MidiPlugin::~MidiPlugin()
{
        m_RefCount--;
	if (m_RefCount==0) MidiDevice::PackUpAndGoHome();
}

PluginInfo &MidiPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info= SpiralPlugin::Initialise(Host);		
	MidiDevice::SetDeviceName(Host->MIDIFILE);		
		
	return Info;
}

SpiralGUIType *MidiPlugin::CreateGUI()
{
	return new MidiPluginGUI(m_PluginInfo.Width,
										  m_PluginInfo.Height,
										  this,m_AudioCH,m_HostInfo);
}

void MidiPlugin::Execute()
{

	// Done to clear IsEmpty field... 
	GetOutputBuf(0)->Zero();
	GetOutputBuf(1)->Zero();
	GetOutputBuf(2)->Zero();
	GetOutputBuf(3)->Zero();
	GetOutputBuf(4)->Zero();
	GetOutputBuf(5)->Zero();

	for (unsigned int c=0; c<m_ControlList.size(); c++)
	{
		GetOutputBuf(c+5)->Zero();	
	}

	bool Triggered=false;

	// midi output	
	if (InputExists(0) && InputExists(1))
	{
		static bool TriggeredOut=false;
		if (GetInput(1,0)>0)
		{
			if (!TriggeredOut) // note on
			{				
				// get the midi note
				float Freq=GetInputPitch(0,0);
				int Note=0;
				for (int n=0; n<132; n++)
				{	
					if (feq(Freq,NoteTable[n],0.01f))
					{
						Note=n;
						break;
					}
				}
				
				MidiEvent NewEvent(MidiEvent::ON,Note,GetInput(1,0)*128.0f);
				MidiDevice::Get()->SendEvent(m_DeviceNum,NewEvent);
				TriggeredOut=true;
			}			
		}
		else
		{
			if (TriggeredOut) // note off
			{
				// get the midi note
				float Freq=GetInputPitch(0,0);
				int Note=0;
				for (int n=0; n<132; n++)
				{	
					if (feq(Freq,NoteTable[n],0.01f))
					{
						Note=n;
						break;
					}
				}

				MidiEvent NewEvent(MidiEvent::OFF,Note,0.0f);
				MidiDevice::Get()->SendEvent(m_DeviceNum,NewEvent);
				TriggeredOut=false;
			}			
		}		
	}
	
	MidiEvent Event=MidiDevice::Get()->GetEvent(m_DeviceNum);
	// get all the midi events since the last check
	while(Event.GetType()!=MidiEvent::NONE)
	{
		if (Event.GetType()==MidiEvent::ON)
		{	
			Triggered=true;
			m_CurrentNote=Event.GetNote();				
			m_NoteLevel=NoteTable[m_CurrentNote];			
			m_TriggerLevel=Event.GetVolume()/127.0f;
		}
			
		if (Event.GetType()==MidiEvent::OFF)
		{
			if (Event.GetNote()==m_CurrentNote)
			{
				m_TriggerLevel=0;
				if (m_NoteCut) m_NoteLevel=0;
			}
		}
		
		if (Event.GetType()==MidiEvent::PITCHBEND)
		{
			m_PitchBendLevel=Event.GetVolume()/127.0f*2.0f-1.0f;	
		}
		
		if (Event.GetType()==MidiEvent::CHANNELPRESSURE)
		{
			m_ChannelPressureLevel=Event.GetVolume()/127.0f;	
		}

		if (Event.GetType()==MidiEvent::AFTERTOUCH)
		{
			m_AfterTouchLevel=Event.GetVolume()/127.0f;
		}
		
		if (Event.GetType()==MidiEvent::PARAMETER)
		{			
			// just to make sure
			if (Event.GetNote()>=0 && Event.GetNote()<128)
			{
				m_ControlLevel[Event.GetNote()]=Event.GetVolume()/127.0f;							
			}
		}
		
		Event=MidiDevice::Get()->GetEvent(m_DeviceNum);
	}

	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		SetOutputPitch(0,n,m_NoteLevel);
		SetOutput(1,n,m_TriggerLevel);
		SetOutput(2,n,m_PitchBendLevel);
		SetOutput(3,n,m_ChannelPressureLevel);
		SetOutput(4,n,m_AfterTouchLevel);						
		SetOutput(5,n,MidiDevice::Get()->GetClock());						
	}
	
	for (unsigned int c=0; c<m_ControlList.size(); c++)
	{
		GetOutputBuf(c+5)->Set(m_ControlLevel[m_ControlList[c]]);	
	}
	
	// make sure the trigger is registered if it's 
	// note is pressed before releasing the previous one.
	if (Triggered && !m_ContinuousNotes) SetOutput(1,0,0);
}

void  MidiPlugin::ExecuteCommands()
{
	// Process any commands from the GUI
	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
			case (ADDCONTROL) : AddControl(m_GUIArgs.s,m_GUIArgs.Name); break;
			case (DELCONTROL) : DeleteControl();
		};
	}
}

void MidiPlugin::AddControl(int s, const string &Name)
{
	m_ControlList.push_back(s);
	AddOutput();
	m_PluginInfo.NumOutputs++;
	m_PluginInfo.PortTips.push_back(Name);
	UpdatePluginInfoWithHost();
}

void MidiPlugin::DeleteControl()
{
	if (m_ControlList.size()==0) return;
	
	m_ControlList.pop_back();
	RemoveOutput();
	m_PluginInfo.NumOutputs--;
	m_PluginInfo.PortTips.pop_back();
	UpdatePluginInfoWithHost();
}

void MidiPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<" "<<m_DeviceNum<<" "<<m_NoteCut<<" ";	
	s<<m_ControlList.size()<<endl;
	for (unsigned int n=0; n<m_ControlList.size(); n++)
	{
		string PortTip=m_PluginInfo.PortTips[5+n];
		s<<m_ControlList[n]<<" "<<PortTip.size()<<" "<<PortTip<<endl;
	}
	
}

void MidiPlugin::StreamIn(istream &s) 
{
	int version;
	s>>version;
		
	switch (version)
	{
		case 1:	s>>m_DeviceNum>>m_NoteCut; break;
		
		case 2:
		{
			s>>m_DeviceNum>>m_NoteCut; 
			
			int Num;
			s>>Num;
			
			for (int n=0; n<Num; n++)
			{
				int Control;
				s>>Control;
				
				char Buf[4096];	
				int size;		
				s>>size;
				s.ignore(1);
				s.get(Buf,size+1);								
				
				AddControl(Control, Buf);
			}			
		}
	}
}
