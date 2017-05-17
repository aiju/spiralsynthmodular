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

#include "SpiralPlugin.h"

using namespace std;

SpiralPlugin::SpiralPlugin()
{
	m_Version=1;
	m_PluginInfo.Name="BasePlugin";
	m_PluginInfo.Width=100;
	m_PluginInfo.Height=100;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=0;
	UpdateInfo=NULL;
	cb_Update=NULL;
	m_Parent=NULL;
	m_HostID=-1;
	m_IsTerminal=false;
	m_IsDead=false;
	m_AudioCH = new ChannelHandler;
}

SpiralPlugin::~SpiralPlugin()
{
	RemoveAllOutputs();	
	RemoveAllInputs();	
	delete m_AudioCH;
}

bool SpiralPlugin::Kill() 
{
	m_IsDead = true; 
	return true;
}

PluginInfo &SpiralPlugin::Initialise(const HostInfo *Host)
{	
	m_HostInfo=Host;

        for (int n=0; n<m_PluginInfo.NumInputs; n++)
	{
		m_Input.push_back(NULL);
	}

	for (int n=0; n<m_PluginInfo.NumOutputs; n++)
	{
		m_Output.push_back(new Sample(Host->BUFSIZE));
	}

	for (int n=0; n<m_PluginInfo.NumInputs+m_PluginInfo.NumOutputs; n++)
 	{
 		m_PluginInfo.PortTypes.push_back(0);
 	}

	return m_PluginInfo;
}

bool SpiralPlugin::GetOutput(unsigned int n, Sample **s)
{
	if (n>=m_Output.size()) return false;
	*s = m_Output[n];

	return true;
}

bool SpiralPlugin::SetInput(unsigned int n, const Sample *s)
{
	if (n>=m_Input.size()) return false;
	m_Input[n]=s;
	return true;
}

void SpiralPlugin::UpdateChannelHandler()
{
    m_AudioCH->UpdateDataNow();
}

void SpiralPlugin::AddOutput()
{
	Sample* NewSample = new Sample(m_HostInfo->BUFSIZE);
	m_Output.push_back(NewSample);
}

void SpiralPlugin::RemoveOutput()
{
	vector<Sample*>::iterator si=m_Output.end();
	si--;
	delete *si;
	m_Output.erase(si);
}

void SpiralPlugin::RemoveAllOutputs()
{
	for (vector<Sample*>::iterator i=m_Output.begin();
		 i!=m_Output.end(); i++)
	{
		delete *i;
	}

	m_Output.clear();
}

void SpiralPlugin::AddInput()
{
	m_Input.push_back(NULL);
}

void SpiralPlugin::RemoveInput()
{
	vector<const Sample*>::iterator si=m_Input.end();
	si--;
	m_Input.erase(si);
}

void SpiralPlugin::RemoveAllInputs()
{
	m_Input.clear();
}

void SpiralPlugin::UpdatePluginInfoWithHost()
{
	if (UpdateInfo!=NULL)
	{
		UpdateInfo(m_HostID,(void*)&m_PluginInfo);
	}
}
void SpiralPlugin::SetUpdateInfoCallback(int ID, void(*s)(int, void *))
{
	m_HostID=ID;
	UpdateInfo=s;
}

void SpiralPlugin::SetInPortType(PluginInfo &pinfo, int port, Sample::SampleType type)
{
    pinfo.PortTypes[port] = type;
}

void SpiralPlugin::SetOutPortType(PluginInfo &pinfo, int port, Sample::SampleType type)
{
    pinfo.PortTypes[port+m_PluginInfo.NumInputs] = type;
    GetOutputBuf(port)->setSampleType(type);
}
