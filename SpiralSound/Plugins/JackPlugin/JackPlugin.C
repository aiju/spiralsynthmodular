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

#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "JackPlugin.h"
#include "JackPluginGUI.h"
#include "SpiralIcon.xpm"

using namespace std;

int JackClient::JackProcessInstanceID = -1;
int JackPlugin::JackInstanceCount = 0;

/////////////////////////////////////////////////////////////////////////////////////////////
inline void JackClient::JackProcess_i(jack_nframes_t nframes)
{	
	SetBufferSize(nframes);
  		
	for (int n=0; n<GetJackInputCount(); n++)
	{
		if (jack_port_connected(m_InputPortMap[n]->Port))
		{
			sample_t *in = (sample_t *) jack_port_get_buffer(m_InputPortMap[n]->Port, nframes);
			memcpy (m_InputPortMap[n]->Buf, in, sizeof (sample_t) * GetBufferSize());
		}			
	}
	
	for (int n=0; n<GetJackOutputCount(); n++)
	{
		if (jack_port_connected(m_OutputPortMap[n]->Port))
		{
			if (m_OutputPortMap[n]->Buf)
			{ 
				sample_t *out = (sample_t *) jack_port_get_buffer(m_OutputPortMap[n]->Port, nframes);
				memcpy (out, m_OutputPortMap[n]->Buf, sizeof (sample_t) * GetBufferSize());
			}
			else // no output availible, clear
			{ 
				sample_t *out = (sample_t *) jack_port_get_buffer(m_OutputPortMap[n]->Port, nframes);
				memset (out, 0, sizeof (sample_t) * GetBufferSize());
			}
		}
	}
		
	if (RunCallback&&RunContext)
	{
		if (JackProcessInstanceID==-1)
			JackProcessInstanceID = m_JackInstanceID;

		if (JackProcessInstanceID==m_JackInstanceID)
			RunCallback(RunContext,true);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

inline void JackClient::SampleRateChange_i(jack_nframes_t nframes)
{
	SetSampleRate(nframes);
}

/////////////////////////////////////////////////////////////////////////////////////////////

inline void JackClient::JackShutdown_i()
{
	cerr<<"Shutdown"<<endl;

	SetAttached(false);

	if (JackProcessInstanceID==m_JackInstanceID)
		JackProcessInstanceID = -1;
		
	// tells ssm to go back to non callback mode
	RunCallback(RunContext, false);
}

///////////////////////////////////////////////////////

JackClient::JackClient()
{
  m_JackInstanceID = 0;
  m_Attached = false;
  m_SampleRate = 0;
  m_BufferSize = 0;
  m_JackInputCount = 4;
  m_JackOutputCount = 4;        
  m_Client=NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////

JackClient::~JackClient()	
{	
   if (IsAttached()) Detach();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void JackClient::AddInputPort(int NewPortNumber)
{
  char Name[256];
  JackPort *NewPort;

  if (!(m_Client)) return;
  
  NewPort = new JackPort;

  sprintf(Name,"In%d", NewPortNumber);	
  
  NewPort->PortNo = NewPortNumber;
  NewPort->Name=Name;
  NewPort->Buf=NULL;		
  NewPort->Port = jack_port_register (m_Client, Name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

  m_InputPortMap[NewPortNumber]=NewPort;
}

void JackClient::AddOutputPort(int NewPortNumber)
{
  char Name[256];
  JackPort *NewPort;
  
  if (!(m_Client)) return;
  
  NewPort = new JackPort;

  sprintf(Name,"Out%d", NewPortNumber);	
  
  NewPort->PortNo = NewPortNumber;
  NewPort->Name=Name;
  NewPort->Buf=NULL;		
  NewPort->Port = jack_port_register (m_Client, Name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  m_OutputPortMap[NewPortNumber]=NewPort;  
}

void JackClient::RemoveInputPort(int PortNumber)
{
  char Name[256];
  JackPort *OldPort;

  if (!(m_Client)) return;
  
  OldPort = m_InputPortMap[PortNumber];
  m_InputPortMap[PortNumber] = NULL;
  jack_port_unregister (m_Client, OldPort->Port);
  delete OldPort;
}

void JackClient::RemoveOutputPort(int PortNumber)
{
  char Name[256];
  JackPort *OldPort;

  if (!(m_Client)) return;
  
  OldPort = m_OutputPortMap[PortNumber];
  m_OutputPortMap[PortNumber] = NULL;
  jack_port_unregister (m_Client, OldPort->Port);
  delete OldPort;
}

bool JackClient::Attach()
{
	char JackClientName[256];

	if (m_Attached) return true;

	sprintf(JackClientName,"SSM%d",GetJackInstanceID());
	if (!(m_Client = jack_client_new(JackClientName))) 
	{
		cerr<<"jack server not running?"<<endl;
		return false;
	}

	jack_set_process_callback(m_Client, JackProcess, this);
	jack_set_sample_rate_callback (m_Client, SampleRateChange, this);
	jack_on_shutdown (m_Client, JackShutdown, this);

	// create the ports 
	m_InputPortMap.clear();
	for (int n=0; n<GetJackInputCount(); n++)
	  AddInputPort(n);

	m_OutputPortMap.clear();
	for (int n=0; n<GetJackOutputCount(); n++)
	  AddOutputPort(n);

	// tell the JACK server that we are ready to roll 
	if (jack_activate (m_Client)) 
	{
		cerr<<"cannot activate client"<<endl;
		return false;
	}
	
	m_Attached=true;
	
	cerr<<"connected to jack..."<<endl;
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void JackClient::Detach()
{
	if (m_Client)
	{
		cerr<<"Detaching from JACK"<<endl;
		jack_client_close(m_Client);
		m_Client=NULL;
		m_Attached=false;
	}
	
	if (JackProcessInstanceID==m_JackInstanceID)
		JackProcessInstanceID = -1;

	// tells ssm to go back to non callback mode
	RunCallback(RunContext, false);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void JackClient::GetPortNames(vector<string> &InputNames, vector<string> &OutputNames)
{
	InputNames.clear();
	OutputNames.clear();

	if (!m_Attached) return;

	//Outputs first
	const char **PortNameList=jack_get_ports(m_Client,NULL,NULL,JackPortIsOutput);	
	
	int n=0;
	while(PortNameList[n]!=NULL)
	{		
		OutputNames.push_back(PortNameList[n]);
		n++;
	}	
	
	delete PortNameList;
	
	//Inputs second
	PortNameList=jack_get_ports(m_Client,NULL,NULL,JackPortIsInput);
	
	n=0;
	while(PortNameList[n]!=NULL)
	{		
		InputNames.push_back(PortNameList[n]);
		n++;
	}
	
	delete PortNameList;		
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Input means input of SSM, so this connects jack sources to the plugin outputs
void JackClient::ConnectInput(int n, const string &JackPort)
{
	if (!IsAttached()) return;

	cerr<<"JackClient::ConnectInput: connecting source ["<<JackPort<<"] to dest ["<<m_InputPortMap[n]->Name<<"]"<<endl;

	if (m_InputPortMap[n]->ConnectedTo!="")
	{
		if (jack_disconnect (m_Client, m_InputPortMap[n]->ConnectedTo.c_str(), jack_port_name(m_InputPortMap[n]->Port))) 
			cerr<<"JackClient::ConnectInput: cannot disconnect input port ["
				<<m_InputPortMap[n]->ConnectedTo<<"] from ["<<m_InputPortMap[n]->Name<<"]"<<endl;
	}
	
	m_InputPortMap[n]->ConnectedTo = JackPort;
		
	if (jack_connect (m_Client, JackPort.c_str(), jack_port_name(m_InputPortMap[n]->Port))) 
		cerr<<"JackClient::ConnectInput: cannot connect input port ["
			<<JackPort<<"] to ["<<m_InputPortMap[n]->Name<<"]"<<endl;
			
	m_InputPortMap[n]->Connected=true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Output means output of SSM, so this connects plugin inputs to a jack destination
void JackClient::ConnectOutput(int n, const string &JackPort)
{
	if (!IsAttached()) return;
	cerr<<"JackClient::ConnectOutput: connecting source ["<<m_OutputPortMap[n]->Name<<"] to dest ["<<JackPort<<"]"<<endl;

	if (m_OutputPortMap[n]->ConnectedTo!="")
	{
		if (jack_disconnect (m_Client, jack_port_name(m_OutputPortMap[n]->Port), m_OutputPortMap[n]->ConnectedTo.c_str())) 
			cerr<<"JackClient::ConnectOutput: cannot disconnect output port ["
				<<m_OutputPortMap[n]->ConnectedTo<<"] from ["<<m_OutputPortMap[n]->Name<<"]"<<endl;
	}
	
	m_OutputPortMap[n]->ConnectedTo = JackPort;
	if (jack_connect (m_Client, jack_port_name(m_OutputPortMap[n]->Port), JackPort.c_str()))
		cerr<<"JackClient::ConnectOutput: cannot connect output port ["
			<<m_OutputPortMap[n]->Name<<"] to ["<<JackPort<<"]"<<endl;
	m_OutputPortMap[n]->Connected=true; 
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Input means input of SSM, so this connects jack sources to the plugin outputs
void JackClient::DisconnectInput(int n)
{
	if (!IsAttached()) return;
	cerr<<"JackClient::DisconnectInput: Disconnecting input "<<n<<endl;

	if (m_InputPortMap[n]->ConnectedTo!="")
	{
		if (jack_disconnect (m_Client, m_InputPortMap[n]->ConnectedTo.c_str(), jack_port_name(m_InputPortMap[n]->Port))) 
			cerr<<"JackClient::ConnectInput: cannot disconnect input port ["
				<<m_InputPortMap[n]->ConnectedTo<<"] from ["<<m_InputPortMap[n]->Name<<"]"<<endl;
	}

	m_InputPortMap[n]->Connected=false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Output means output of SSM, so this connects plugin inputs to a jack destination
void JackClient::DisconnectOutput(int n)
{
	if (!IsAttached()) return;
	cerr<<"JackClient::DisconnectInput: Disconnecting input "<<n<<endl;

	if (m_OutputPortMap[n]->ConnectedTo!="")
	{
		if (jack_disconnect (m_Client, jack_port_name(m_OutputPortMap[n]->Port), m_OutputPortMap[n]->ConnectedTo.c_str())) 
			cerr<<"JackClient::ConnectOutput: cannot disconnect output port ["
				<<m_OutputPortMap[n]->ConnectedTo<<"] from ["<<m_OutputPortMap[n]->Name<<"]"<<endl;
	}

	m_OutputPortMap[n]->Connected=false;
}
/////////////////////////////////////////////////////////////////////////////////////////////

void JackClient::SetInputBuf(int ID, float* s)  
{ 
	if(m_InputPortMap.find(ID)!=m_InputPortMap.end()) m_InputPortMap[ID]->Buf=s; 
} 

/////////////////////////////////////////////////////////////////////////////////////////////
	
void JackClient::SetOutputBuf(int ID, float* s) 
{ 
	if(m_OutputPortMap.find(ID)!=m_OutputPortMap.end()) m_OutputPortMap[ID]->Buf=s; 
}

/////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new JackPlugin;
}

char** SpiralPlugin_GetIcon()
{	
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 31;
}

string SpiralPlugin_GetGroupName()
{
	return "InputOutput";
}
}

///////////////////////////////////////////////////////

JackPlugin::JackPlugin() :
m_UpdateNames(false),
m_Connected(false)
{
        m_JackClient=new JackClient;

	//clunky way to ensure unique JackID - JackInstanceCount is never dec 
	//so new JackInstances per session always get a higher number even on 
	//reload and new Patch
	
	m_JackInstanceID = JackInstanceCount;
	JackInstanceCount++;

        m_JackClient->SetJackInstanceID(m_JackInstanceID);
        
	// we are an output
	m_IsTerminal = true;
	
	m_Version = 2;
	
	m_PluginInfo.Name="Jack";
	m_PluginInfo.Width=225;
	m_PluginInfo.Height=230;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=0;
	
     	m_PluginInfo.PortTips.clear();

     	m_PluginInfo.NumInputs = m_JackClient->GetJackOutputCount();
	m_GUIArgs.NumInputs = m_PluginInfo.NumInputs;

     	for (int n=0; n<m_JackClient->GetJackInputCount(); n++)
     	{
		char Temp[256];
		sprintf(Temp,"SSM Input %d",n);
		m_PluginInfo.PortTips.push_back(Temp);
     	}
	
     	m_PluginInfo.NumOutputs = m_JackClient->GetJackOutputCount();
	m_GUIArgs.NumOutputs = m_PluginInfo.NumOutputs;

	for (int n=0; n<m_JackClient->GetJackOutputCount(); n++)
	{
		char Temp[256];
		sprintf(Temp,"SSM Output %d",n);
		m_PluginInfo.PortTips.push_back(Temp);
	}
     	
	m_AudioCH->Register("NumInputs",&m_GUIArgs.NumInputs);
	m_AudioCH->Register("NumOutputs",&m_GUIArgs.NumOutputs);
	m_AudioCH->RegisterData("Port",ChannelHandler::INPUT,&m_GUIArgs.Port,sizeof(m_GUIArgs.Port));
	m_AudioCH->Register("NumInputPortNames",&m_NumInputPortNames,ChannelHandler::OUTPUT);
	m_AudioCH->Register("NumOutputPortNames",&m_NumOutputPortNames,ChannelHandler::OUTPUT);
	m_AudioCH->RegisterData("InputPortNames",ChannelHandler::OUTPUT,&m_InputPortNames,sizeof(m_InputPortNames));
	m_AudioCH->RegisterData("OutputPortNames",ChannelHandler::OUTPUT,&m_OutputPortNames,sizeof(m_OutputPortNames));
	m_AudioCH->Register("UpdateNames",&m_UpdateNames,ChannelHandler::OUTPUT);	
	m_AudioCH->Register("Connected",&m_Connected,ChannelHandler::OUTPUT);	
}

JackPlugin::~JackPlugin()
{
	if (m_JackClient)
	{
		m_JackClient->Detach();
		delete m_JackClient; 
		m_JackClient=NULL;
	}	
}

bool JackPlugin::Kill()
{
	m_IsDead=true;

	UpdatePluginInfoWithHost();
	RemoveAllInputs ();
	RemoveAllOutputs ();
	UpdatePluginInfoWithHost();

	m_JackClient->Detach();
	delete m_JackClient; 
	m_JackClient=NULL;
	return true;
}

PluginInfo &JackPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info= SpiralPlugin::Initialise(Host);
	host=Host;		
	
	m_JackClient->SetCallback(cb_Update,m_Parent);	

	return Info;
}

SpiralGUIType *JackPlugin::CreateGUI()
{
	return new JackPluginGUI(m_PluginInfo.Width,
						  m_PluginInfo.Height,
					      this,m_AudioCH,m_HostInfo);
}

void  JackPlugin::SetNumberPorts (int nInputs, int nOutputs) {
     UpdatePluginInfoWithHost();
     RemoveAllInputs ();
     RemoveAllOutputs ();
     m_PluginInfo.NumInputs = 0;
     m_PluginInfo.NumOutputs = 0;
     m_PluginInfo.PortTips.clear ();
     CreatePorts (nInputs, nOutputs, true);
     UpdatePluginInfoWithHost ();
}

void  JackPlugin::CreatePorts (int nInputs, int nOutputs, bool AddPorts) {
    	m_PluginInfo.PortTips.clear();

    	m_PluginInfo.NumInputs = nInputs;
     	m_JackClient->SetJackInputCount(nInputs);

     	for (int n=0; n<nInputs; n++)
     	{
		char Temp[256];
		sprintf(Temp,"SSM Input %d",n);
		m_PluginInfo.PortTips.push_back(Temp);
     	}
	
    	m_PluginInfo.NumOutputs = nOutputs;
     	m_JackClient->SetJackOutputCount(nOutputs);

	for (int n=0; n<nOutputs; n++)
	{
		char Temp[256];
		sprintf(Temp,"SSM Output %d",n);
		m_PluginInfo.PortTips.push_back(Temp);
	}

     if (AddPorts) {
        for (int n=0; n<nInputs; n++) AddInput();
        for (int n=0; n<nOutputs; n++) AddOutput();
     }

 }

void JackPlugin::Execute()
{
}

void JackPlugin::ExecuteCommands()
{
	if (m_IsDead) return;
	
        bool commandwaiting = m_AudioCH->IsCommandWaiting();
        int command = (commandwaiting)?(int)m_AudioCH->GetCommand():0;

	if (commandwaiting)
	{
		switch (command) {
			case SET_PORT_COUNT :
				SetNumberPorts (m_GUIArgs.NumInputs, m_GUIArgs.NumOutputs);				
                }    
        }   
        

	// we want to process this whether we are connected to stuff or not
	JackClient* pJack=m_JackClient;
	
	// connect the buffers up if we are plugged into something		
	for (int n=0; n<pJack->GetJackOutputCount(); n++)
	{
		if (InputExists(n)) 
		{			
			pJack->SetOutputBuf(n,(float*)GetInput(n)->GetBuffer());		
		}
		else 
		{	
			pJack->SetOutputBuf(n,NULL);
		}
	}
	
	for (int n=0; n<pJack->GetJackInputCount(); n++)
	{
		if (OutputExists(n)) 
		{
			pJack->SetInputBuf(n,(float*)GetOutputBuf(n)->GetBuffer());		
		} 
		else	
		{	
			pJack->SetInputBuf(n,NULL);
		}
  	} 
        
	if (commandwaiting)
	{
		switch (command)
		{
			case UPDATE_NAMES :
			{
				int c=0;
	
			    std::vector<string> InputNames,OutputNames;
				GetPortNames(InputNames,OutputNames);
				for (vector<string>::iterator i=InputNames.begin();
					 i!=InputNames.end(); ++i)
				{
					strcpy(m_InputPortNames[c],i->c_str());
					c++;
				}
		
				c=0;
		
				for (std::vector<string>::iterator i=OutputNames.begin();
					 i!=OutputNames.end(); ++i)
				{
					strcpy(m_OutputPortNames[c],i->c_str());
					c++;
				}
		
				m_NumInputPortNames=InputNames.size();
				m_NumOutputPortNames=OutputNames.size();
			}
			break;
			
			case CHECK_PORT_CHANGES :
				if ((m_JackClient->IsAttached()) && (!m_JackClient->CheckingPortChanges)) {
					m_JackClient->CheckingPortChanges = true;
				
					for (int n=0; n<m_PluginInfo.NumInputs; n++) {
						if (jack_port_connected(m_JackClient->m_OutputPortMap[n]->Port)!=m_JackClient->m_OutputPortMap[n]->Connected)
							m_JackClient->m_OutputPortsChanged.push_back(m_JackClient->m_OutputPortMap[n]);

						if (jack_port_connected(m_JackClient->m_InputPortMap[n]->Port)!=m_JackClient->m_InputPortMap[n]->Connected)
							m_JackClient->m_InputPortsChanged.push_back(m_JackClient->m_InputPortMap[n]);						
					}		

					m_JackClient->CheckingPortChanges = false;
				}
			
			break;
			default : break;
		}
	}
	m_Connected=m_JackClient->IsAttached();
}

void JackPlugin::StreamOut (ostream &s) 
{
	s << m_Version << " " << m_GUIArgs.NumInputs << " " << m_GUIArgs.NumOutputs << " ";
}

void JackPlugin::StreamIn (istream &s) 
{
	char Test;
	int Version, NumInputs, NumOutputs;

	s.seekg (2, ios::cur );  //skip to next line
	Test = s.peek();         //peek first char
	s.seekg (-2, ios::cur ); //jump back to prior line
	
	if ( (Test >= '0') && (Test <= '9') )
	{
		s >> Version;
	}
	else
	{
		//No Version, so use Version 1
		Version = 1;
	}
	
	switch (Version)
	{
		case 2:
		{
			s >> NumInputs >> NumOutputs;
			m_GUIArgs.NumOutputs = min(max(NumOutputs, MIN_PORTS), MAX_PORTS);
			m_GUIArgs.NumInputs = min(max(NumInputs, MIN_PORTS), MAX_PORTS);
			
			SetNumberPorts (m_GUIArgs.NumInputs, m_GUIArgs.NumOutputs);				
		}
		break;
		
		case 1:
		{
			//use original fixed defaults
			m_GUIArgs.NumInputs = 16;
			m_GUIArgs.NumOutputs = 16;

			SetNumberPorts (m_GUIArgs.NumInputs, m_GUIArgs.NumOutputs);				
		}
		break;
	}	
}
