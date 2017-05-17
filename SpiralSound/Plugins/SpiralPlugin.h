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

#ifndef SPIRALPLUGIN
#define SPIRALPLUGIN

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <math.h>
#include <FL/Fl.h>
#include <FL/Fl_Group.h>
#include "../Sample.h"
#include "../ChannelHandler.h"
#include "../../GUI/Widgets/SpiralGUI.H"

static const float MAX_FREQ = 13000;

struct PluginInfo
{
	std::string Name;
	int Width;
	int Height;
	int NumInputs;
	int NumOutputs;
	std::vector<std::string> PortTips;
	std::vector<int> PortTypes;
	char BitMap[40][40][3];
};

struct HostInfo
{
	int    BUFSIZE;
	int    FRAGSIZE;
	int    FRAGCOUNT;
	int    SAMPLERATE;
	std::string OUTPUTFILE;
	std::string MIDIFILE;
	int    POLY;
        unsigned GUI_COLOUR;
        unsigned SCOPE_BG_COLOUR;
        unsigned SCOPE_FG_COLOUR;
        unsigned SCOPE_SEL_COLOUR;
        unsigned SCOPE_IND_COLOUR;
        unsigned SCOPE_MRK_COLOUR;
        unsigned GUICOL_Device;
        unsigned GUIDEVICE_Box;
};

/////////////////////////////////////////////////////////////////////

class ChannelHandler;

class SpiralPlugin
{
public:
	SpiralPlugin();
	virtual ~SpiralPlugin();

	virtual PluginInfo& Initialise(const HostInfo *Host);

	// execute the audio
	virtual void        Execute()=0;
	virtual bool         Kill();

	// run the commands from the GUI
	virtual void        ExecuteCommands() {}
	// create the GUI, do not store the pointer - it wont be threadsafe to use it
	virtual SpiralGUIType*  CreateGUI()=0;

	// stream the plugins state
	virtual void	    StreamOut(std::ostream &s)=0;
	virtual void	    StreamIn(std::istream &s)=0;

	// stuff here gets saved in filename_files directory
	// you must return true if this feature is used.
	virtual bool	    SaveExternalFiles(const std::string &Dir) { return false; }
	virtual void	    LoadExternalFiles(const std::string &Dir) {}

	const HostInfo*     GetHostInfo() { return m_HostInfo; }
	bool                GetOutput(unsigned int n, Sample **s);
	bool                SetInput(unsigned int n, const Sample *s);
	const Sample*       GetInput(unsigned int n) { return m_Input[n]; }
	std::string 		GetName() { return m_PluginInfo.Name; }

	void UpdatePluginInfoWithHost();
	void SetInPortType(PluginInfo &pinfo, int port, Sample::SampleType type);
    void SetOutPortType(PluginInfo &pinfo, int port, Sample::SampleType type);

	// Callbacks to main engine. Should only called by plugin hosts.
	void SetUpdateInfoCallback(int ID, void(*s)(int, void *));
	void SetUpdateCallback(void (*s)(void*,bool m)) { cb_Update=s; }
	void SetBlockingCallback(void (*s)(void*,bool m)) { cb_Blocking=s; }
	void SetParent(void *s) { m_Parent=s; }

	void UpdateChannelHandler();
	// is the plugin connected to an external device (oss/alsa/jack)
	bool IsTerminal() { return m_IsTerminal; }
	bool IsDead() { return m_IsDead; }
	 
    ChannelHandler *GetChannelHandler() { return m_AudioCH; }
		
protected:
	
    ChannelHandler *m_AudioCH;

	void  SetOutput(int n,int p, float s) 
		{ if (m_Output[n]) m_Output[n]->Set(p,s); }
		
	float GetInput(int n,int p)  
		{ if (m_Input[n]) return (*m_Input[n])[p]; else return 0.0; }
		
	void  SetOutputPitch(int n,int p, float s) 
		{ if (m_Output[n]) m_Output[n]->Set(p,(s/MAX_FREQ*2)-1.0f); }
		
	float GetInputPitch(int n,int p)
		{ if (m_Input[n]) return ((*m_Input[n])[p]+1.0f)*MAX_FREQ/2; else return 0.0; }

	void  MixOutput(int n,int p, float s)
		{ if (m_Output[n]) m_Output[n]->Set(p,s+(*m_Output[n])[p]); }
	
	bool InputExists(int n) { return m_Input[n]!=NULL; }
	bool OutputExists(int n) { return m_Output[n]!=NULL; }	

	void AddOutput();
	void RemoveOutput();
	void RemoveAllOutputs();

	void AddInput();
	void RemoveInput();
	void RemoveAllInputs();
		
	// the ID number assigned to us by ssm
	int GetID() { return m_HostID; }
	
	Sample* GetOutputBuf(int n) { return m_Output[n]; }	
	
	const HostInfo *m_HostInfo;
	PluginInfo      m_PluginInfo;
	int             m_Version;

	// needed for jack
	void (*cb_Update)(void*o ,bool m);	
	void  *m_Parent;

	// tell the engine that we are taking control of the 
	// timing for output.
	void (*cb_Blocking)(void*o ,bool m);

	bool   m_IsTerminal;
	bool   m_IsDead;
	
private:

	std::vector<const Sample*> m_Input;
	std::vector<Sample*> m_Output;

	void (*UpdateInfo)(int n,void *);	
	int    m_HostID;	
};

#endif
