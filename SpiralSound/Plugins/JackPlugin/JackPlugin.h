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
#include <jack/jack.h>

using namespace std;

typedef jack_default_audio_sample_t sample_t;

#ifndef JackPLUGIN
#define JackPLUGIN

const int MAX_PORTS = 64;
const int MIN_PORTS = 2;

class JackClient
{
public:
 	JackClient();
	virtual ~JackClient();

	void   AddInputPort(int NewPortNumber);
	void   AddOutputPort(int NewPortNumber);

	void   RemoveInputPort(int PortNumber);
	void   RemoveOutputPort(int PortNumber);

	bool   Attach();
	void   Detach();
	bool   IsAttached()                   { return m_Attached; }
	void   SetAttached(bool Attached)                   { m_Attached = Attached; }
	void   SetCallback(void(*Run)(void*, bool m),void *Context) { RunCallback=Run; RunContext=Context; }
	void   GetPortNames(std::vector<std::string> &InputNames,std::vector<std::string> &OutputNames);
	void   ConnectInput(int n, const std::string &JackPort);
	void   ConnectOutput(int n, const std::string &JackPort);
	void   DisconnectInput(int n);
	void   DisconnectOutput(int n);
	std::string GetInputName(int ID)           { return m_InputPortMap[ID]->Name; }
	std::string GetOutputName(int ID)          { return m_OutputPortMap[ID]->Name; }
	void   SetInputBuf(int ID, float* s);
	void   SetOutputBuf(int ID, float* s);
	int    GetJackInstanceID()           { return m_JackInstanceID; }
	void   SetJackInstanceID(int JackInstanceID)          { m_JackInstanceID=JackInstanceID; }
	int    GetBufferSize()           { return m_BufferSize; }
	void   SetBufferSize(jack_nframes_t BufferSize)          { m_BufferSize=BufferSize; }
	int    GetSampleRate()           { return m_BufferSize; }
	void   SetSampleRate(jack_nframes_t SampleRate)          { m_SampleRate=SampleRate; }
	int    GetJackInputCount()           { return m_JackInputCount; }
	void   SetJackInputCount(int JackInputCount)          { m_JackInputCount=JackInputCount; }
	int    GetJackOutputCount()           { return m_JackOutputCount; }
	void   SetJackOutputCount(int JackOutputCount)          { m_JackOutputCount=JackOutputCount; }

	class JackPort
	{		
		public:
		JackPort::JackPort() :
			Connected(false),Buf(NULL),Port(NULL) {}
		
		int            PortNo;
		std::string    Name;
		bool           Connected;
		float*         Buf;
		jack_port_t*   Port;
		std::string    ConnectedTo;
	};

	jack_client_t*     m_Client;
	std::map<int,JackPort*> m_InputPortMap;
	std::map<int,JackPort*> m_OutputPortMap;

	//// Kludge for GUI ////
	bool CheckingPortChanges;
	std::vector<JackPort*> m_OutputPortsChanged;
	std::vector<JackPort*> m_InputPortsChanged;
	
	//// inline Callbacks ////
        inline void JackProcess_i(jack_nframes_t nframes);	
	inline void SampleRateChange_i(jack_nframes_t nframes);
	inline void JackShutdown_i();	

	//// static Callbacks ////
        static int JackProcess(jack_nframes_t nframes, void *jack_client) { ((JackClient *)jack_client)->JackProcess_i(nframes); return 0;}
	static int SampleRateChange(jack_nframes_t nframes, void *jack_client) { ((JackClient *)jack_client)->SampleRateChange_i(nframes); return 0;}
	static void JackShutdown(void *jack_client) { ((JackClient *)jack_client)->JackShutdown_i();}
private:
	jack_nframes_t  m_BufferSize;
	jack_nframes_t  m_SampleRate;	
	bool            m_Attached;
        int             m_JackInputCount;
        int             m_JackOutputCount;
	int		m_JackInstanceID;

	static int JackProcessInstanceID;

	void(*RunCallback)(void*, bool m);
	void *RunContext;
};

///////////////////////////////////////////////////

class JackPlugin : public SpiralPlugin
{
public:
 	JackPlugin();
	virtual ~JackPlugin();

	virtual PluginInfo& Initialise(const HostInfo *Host);
	virtual SpiralGUIType*  CreateGUI();
	
	virtual void 		Execute();
	virtual void 		ExecuteCommands();

	virtual bool         	Kill();
	
	virtual void	    StreamOut(std::ostream &s);
	virtual void	    StreamIn(std::istream &s);
	
	JackClient *GetJackClient()           { return m_JackClient; }
	
        void SetNumberPorts (int nInputs, int nOutputs);

	enum GUICommands{NONE,UPDATE_NAMES,SET_PORT_COUNT,CHECK_PORT_CHANGES};
	struct GUIArgs
	{
		int NumInputs;
		int NumOutputs;
		char Port[256];
	};

	void Attach() { m_JackClient->Attach(); }
	void Detach() { m_JackClient->Detach(); }			
private:
	const HostInfo* host;

	GUIArgs m_GUIArgs;	
	
	int m_Version;
	
	// slightly clumsy, but we have to share this data with the gui
	int  m_NumInputPortNames;
	char m_InputPortNames[MAX_PORTS][256];
	int  m_NumOutputPortNames;
	char m_OutputPortNames[MAX_PORTS][256];

	void GetPortNames(std::vector<std::string> &InputNames,std::vector<std::string> &OutputNames) { m_JackClient->GetPortNames(InputNames,OutputNames); }
	void ConnectInput(int n, const std::string &JackPort)  { m_JackClient->ConnectInput(n,JackPort); }
	void ConnectOutput(int n, const std::string &JackPort) { m_JackClient->ConnectOutput(n,JackPort); }

        void CreatePorts (int nInputs, int nOutputs, bool );

	bool		m_UpdateNames;
	bool		m_Connected;	
	JackClient 	*m_JackClient;
	int		m_JackInstanceID;

	//clunky work-around for unique ID
	static int JackInstanceCount;
};

#endif
