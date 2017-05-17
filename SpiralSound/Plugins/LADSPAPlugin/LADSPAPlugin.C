/*  LADSPAPlugin.C
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
 *  LADSPA Plugin by Nicolas Noble <nicolas@nobis-crew.org>
 *  Modified by Mike Rawes <myk@waxfrenzy.org>
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
#include <config.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include "SpiralIcon.xpm"
#include "LADSPAPlugin.h"
#include "LADSPAPluginGUI.h"
#include "LADSPAInfo.h"

using namespace std;

LADSPAInfo * LADSPAPlugin::m_LADSPAInfo= NULL;
int LADSPAPlugin::InstanceCount=0;
////////////////////////////////////////////////////

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new LADSPAPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0016;
}

string SpiralPlugin_GetGroupName()
{
	return "Filters/FX";
}
}

///////////////////////////////////////////////////////

LADSPAPlugin::LADSPAPlugin()
{
	InstanceCount++;
	if (!m_LADSPAInfo)
	{
		m_LADSPAInfo = new LADSPAInfo(false, "");
	}	

	m_PlugDesc = NULL;

	ClearPlugin();

	m_Version=9;

	m_PluginInfo.Name="LADSPA";
	m_PluginInfo.Width=500;
	m_PluginInfo.Height=320;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Nuffink yet");

	m_MaxInputPortCount = m_LADSPAInfo->GetMaxInputPortCount();

// For receiving from GUI
	m_AudioCH->RegisterData("SetUniqueID", ChannelHandler::INPUT,&(m_InData.UniqueID), sizeof(m_InData.UniqueID));
	m_AudioCH->RegisterData("SetPage", ChannelHandler::INPUT,&(m_InData.Page), sizeof(m_InData.Page));
	m_AudioCH->RegisterData("SetUpdateInputs", ChannelHandler::INPUT,&(m_InData.UpdateInputs),sizeof(m_InData.UpdateInputs));
	m_AudioCH->RegisterData("SetInputPortIndex", ChannelHandler::INPUT, &(m_InData.InputPortIndex), sizeof(m_InData.InputPortIndex));
	m_AudioCH->RegisterData("SetInputPortDefault", ChannelHandler::INPUT, &(m_InData.InputPortDefault), sizeof(m_InData.InputPortDefault));
	m_AudioCH->RegisterData("SetInputPortMin", ChannelHandler::INPUT, &(m_InData.InputPortMin), sizeof(m_InData.InputPortMin));
	m_AudioCH->RegisterData("SetInputPortMax", ChannelHandler::INPUT, &(m_InData.InputPortMax), sizeof(m_InData.InputPortMax));
	m_AudioCH->RegisterData("SetInputPortClamp", ChannelHandler::INPUT, &(m_InData.InputPortClamp), sizeof(m_InData.InputPortClamp));

// For sending to GUI
	m_AudioCH->RegisterData("GetName",ChannelHandler::OUTPUT,m_Name,256);
	m_AudioCH->RegisterData("GetMaker",ChannelHandler::OUTPUT,m_Maker,256);
	m_AudioCH->RegisterData("GetMaxInputPortCount",ChannelHandler::OUTPUT,&(m_MaxInputPortCount),sizeof(m_MaxInputPortCount));
	m_AudioCH->RegisterData("GetInputPortCount",ChannelHandler::OUTPUT,&(m_InputPortCount),sizeof(m_InputPortCount));

	m_OutData.InputPortNames = (char *)malloc(256 * m_MaxInputPortCount);
	m_OutData.InputPortSettings = (PortSetting *)malloc(sizeof(PortSetting) * m_MaxInputPortCount);
	m_OutData.InputPortValues = (PortValue *)calloc(m_MaxInputPortCount, sizeof(PortValue));
	m_OutData.InputPortDefaults = (float *)calloc(m_MaxInputPortCount, sizeof(float));

	if (m_OutData.InputPortNames &&
	    m_OutData.InputPortSettings &&
	    m_OutData.InputPortValues &&
	    m_OutData.InputPortDefaults)
	{
		m_AudioCH->RegisterData("GetInputPortNames", ChannelHandler::OUTPUT, m_OutData.InputPortNames, 256 * m_MaxInputPortCount);
		m_AudioCH->RegisterData("GetInputPortSettings", ChannelHandler::OUTPUT, m_OutData.InputPortSettings, sizeof(PortSetting) * m_MaxInputPortCount);
		m_AudioCH->RegisterData("GetInputPortValues", ChannelHandler::OUTPUT, m_OutData.InputPortValues, sizeof(PortValue) * m_MaxInputPortCount);
		m_AudioCH->RegisterData("GetInputPortDefaults", ChannelHandler::OUTPUT, m_OutData.InputPortDefaults, sizeof(float) * m_MaxInputPortCount);
	} else {
		cerr<<"LADSPA Plugin: Memory allocation error"<<endl;
	}
}

LADSPAPlugin::~LADSPAPlugin()
{
// Clear plugin
	ClearPlugin();

// Free allocated buffers
	if (m_OutData.InputPortNames)  free(m_OutData.InputPortNames);
	if (m_OutData.InputPortSettings) free(m_OutData.InputPortSettings);
	if (m_OutData.InputPortValues) free(m_OutData.InputPortValues);
	if (m_OutData.InputPortDefaults) free(m_OutData.InputPortDefaults);

	InstanceCount--;
	if (m_LADSPAInfo && InstanceCount<=0)
	{
		delete m_LADSPAInfo;
		m_LADSPAInfo = NULL;
	}	
}

PluginInfo &LADSPAPlugin::Initialise(const HostInfo *Host)
{
	PluginInfo& Info = SpiralPlugin::Initialise(Host);
	LADSPA_Data *NewPort = new LADSPA_Data[m_HostInfo->BUFSIZE];
	m_LADSPABufVec.push_back(NewPort);
	return Info;
}

SpiralGUIType *LADSPAPlugin::CreateGUI()
{
	return new LADSPAPluginGUI(m_PluginInfo.Width, m_PluginInfo.Height,
	                           this, m_AudioCH, m_HostInfo, m_LADSPAInfo->GetMenuList());
}

void LADSPAPlugin::Execute()
{
	if (m_PlugDesc)
	{
		// convert inputs if exist (use default if not)
		for (int n=0; n<m_PluginInfo.NumInputs; n++)
		{
			if (GetInput(n))
			{
				if (m_InputPortClamp[n]) {
					// scale input to match hinted range
					float Offset=m_InputPortMin[n];
					float Scale=m_InputPortMax[n]-m_InputPortMin[n];

					//cerr<<n<<" ["<<Scale<<"] ["<<Offset<<"]"<<endl;

					for (int i=0; i<m_HostInfo->BUFSIZE; i++)
					{
						m_LADSPABufVec[n][i]=Offset+(GetInput(n,i)*0.5f+0.5f)*Scale;
						//cerr<<Scale<<" "<<Offset<<" "<<m_LADSPABufVec[n][i]<<endl;
					}
				} else {
					// pass input as is
					for (int i=0; i<m_HostInfo->BUFSIZE; i++)
					{
						m_LADSPABufVec[n][i]=GetInput(n,i);
					}
				}
				m_OutData.InputPortValues[n].Connected = true;
				m_InputPortDefault[n] = m_LADSPABufVec[n][0];
			}
			else // Use default
			{
				for (int i=0; i<m_HostInfo->BUFSIZE; i++) {
					m_LADSPABufVec[n][i]=m_InputPortDefault[n];
				}
				if (m_OutData.InputPortValues[n].Connected) {
					m_OutData.InputPortValues[n].Connected = false;
					m_InputPortDefault[n] = m_OutData.InputPortValues[n].Value;
				}
			}
			// Copy values into OutData value buffer for display in GUI
			m_OutData.InputPortValues[n].Value = m_LADSPABufVec[n][0];
			// Ditto for default, which may have been set to value
			m_OutData.InputPortDefaults[n] = m_InputPortDefault[n];
		}

		// run plugin
		m_PlugDesc->run(m_PlugInstHandle,m_HostInfo->BUFSIZE);

		// convert outputs
		for (int n=0; n<m_PluginInfo.NumOutputs; n++)
		{
			for (int i=0; i<m_HostInfo->BUFSIZE; i++)
			{
				SetOutput(n,i,m_LADSPABufVec[n+m_PluginInfo.NumInputs][i]);
			}
		}
	}
}

void LADSPAPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch(m_AudioCH->GetCommand())
		{
			case (SETPAGE):
			{
				m_Page = m_InData.Page;
			}
			break;
			case (SELECTPLUGIN):
			{
				UpdatePlugin(m_InData.UniqueID);
			}
			break;
			case (CLEARPLUGIN):
			{
				ClearPlugin();
				m_PluginInfo.NumOutputs=1;
				m_PluginInfo.PortTips.push_back("Nuffink yet");
				UpdatePluginInfoWithHost();
			}
			break;
			case (SETUPDATEINPUTS):
			{
				m_UpdateInputs = m_InData.UpdateInputs;
			}
			break;
			case (SETDEFAULT):
			{
				m_InputPortDefault[m_InData.InputPortIndex] = m_InData.InputPortDefault;
				m_OutData.InputPortDefaults[m_InData.InputPortIndex] = m_InData.InputPortDefault;
			}
			break;
			case (SETMIN):
			{
				m_InputPortMin[m_InData.InputPortIndex] = m_InData.InputPortMin;
				m_OutData.InputPortSettings[m_InData.InputPortIndex].Min = m_InData.InputPortMin;
			}
			break;
			case (SETMAX):
			{
				m_InputPortMax[m_InData.InputPortIndex] = m_InData.InputPortMax;
				m_OutData.InputPortSettings[m_InData.InputPortIndex].Max = m_InData.InputPortMax;
			}
			break;
			case (SETCLAMP):
			{
				m_InputPortClamp[m_InData.InputPortIndex] = m_InData.InputPortClamp;
				m_OutData.InputPortSettings[m_InData.InputPortIndex].Clamp = m_InData.InputPortClamp;
			}
		}
	}

// If there are no connections, Execute() will not be called.
// If the last connection is removed, it will not be reflected in
// the GUI data (m_OutData.InputPortValues.Connected)
	bool has_connection = false;
	for (int p = 0; p < m_PluginInfo.NumInputs && !has_connection; p++)
	{
		if (GetInput(p)) has_connection = true;
	}
	if (!has_connection)
	{
	// Only change stuff if there are no connections
	// (i.e. if Execute has not already taken care of this)
		for (int p = 0; p < m_PluginInfo.NumInputs; p++)
		{
			m_OutData.InputPortValues[p].Connected = false;
		}
	}
}

void LADSPAPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";

	switch (m_Version)
	{
		case 9:
		{
		// Get number of unconnected inputs
			m_UnconnectedInputs = m_PluginInfo.NumInputs;
			for (int p = 0; p < m_PluginInfo.NumInputs; p++) {
				if (m_OutData.InputPortValues[p].Connected) m_UnconnectedInputs--;
			}

			s<<m_Page<<" ";
			s<<m_UpdateInputs<<" ";
			s<<m_UniqueID<<" ";
			s<<m_InputPortMin.size()<<" ";
			s<<m_UnconnectedInputs<<" ";
			assert(m_InputPortMin.size()==m_InputPortMax.size());
			assert(m_InputPortMin.size()==m_InputPortClamp.size());
			assert(m_InputPortMin.size()==m_InputPortDefault.size());
			for (vector<float>::iterator i=m_InputPortMin.begin();
			     i!=m_InputPortMin.end(); i++)
			{
				float f = finite(*i)?(*i):0.0f;
				s<< f <<" ";
			}
			for (vector<float>::iterator i=m_InputPortMax.begin();
			     i!=m_InputPortMax.end(); i++)
			{
				float f = finite(*i)?(*i):0.0f;
				s<< f <<" ";
			}
			for (vector<bool>::iterator i=m_InputPortClamp.begin();
			     i!=m_InputPortClamp.end(); i++)
			{
				float f = finite(*i)?(*i):0.0f;
				s<< f <<" ";
			}
			for (vector<float>::iterator i=m_InputPortDefault.begin();
			     i!=m_InputPortDefault.end(); i++)
			{
				float f = finite(*i)?(*i):0.0f;
				s<< f <<" ";
			}
		}
		break;
		case 8:
		{
//			s<<m_Page<<" ";
//			s<<m_UpdateInputs<<" ";
//			s<<m_UniqueID<<" ";
//			s<<m_InputPortMin.size()<<" ";
//			assert(m_InputPortMin.size()==m_InputPortMax.size());
//			assert(m_InputPortMin.size()==m_InputPortClamp.size());
//			assert(m_InputPortMin.size()==m_InputPortDefault.size());
//			for (vector<float>::iterator i=m_InputPortMin.begin();
//			     i!=m_InputPortMin.end(); i++)
//			{
//				float f = finite(*i)?(*i):0.0f;
//				s<< f <<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortMax.begin();
//			     i!=m_InputPortMax.end(); i++)
//			{
//				float f = finite(*i)?(*i):0.0f;
//				s<< f <<" ";
//			}
//			for (vector<bool>::iterator i=m_InputPortClamp.begin();
//			     i!=m_InputPortClamp.end(); i++)
//			{
//				float f = finite(*i)?(*i):0.0f;
//				s<< f <<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortDefault.begin();
//			     i!=m_InputPortDefault.end(); i++)
//			{
//				float f = finite(*i)?(*i):0.0f;
//				s<< f <<" ";
//			}
		}
		break;
		case 7:
		{
//			s<<m_Page<<" ";
//			s<<m_UniqueID<<" ";
//			s<<m_InputPortMin.size()<<" ";
//			assert(m_InputPortMin.size()==m_InputPortMax.size());
//			assert(m_InputPortMin.size()==m_InputPortClamp.size());
//			assert(m_InputPortMin.size()==m_InputPortDefault.size());
//			for (vector<float>::iterator i=m_InputPortMin.begin();
//			     i!=m_InputPortMin.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortMax.begin();
//			     i!=m_InputPortMax.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<bool>::iterator i=m_InputPortClamp.begin();
//			     i!=m_InputPortClamp.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortDefault.begin();
//			     i!=m_InputPortDefault.end(); i++)
//			{
//				s<<*i<<" ";
//			}
		}
		break;
		case 6:
		{
//			s<<m_UniqueID<<" ";
//			s<<m_InputPortMin.size()<<" ";
//			assert(m_InputPortMin.size()==m_InputPortMax.size());
//			assert(m_InputPortMin.size()==m_InputPortClamp.size());
//			assert(m_InputPortMin.size()==m_InputPortDefault.size());
//			for (vector<float>::iterator i=m_InputPortMin.begin();
//			     i!=m_InputPortMin.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortMax.begin();
//			     i!=m_InputPortMax.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<bool>::iterator i=m_InputPortClamp.begin();
//			     i!=m_InputPortClamp.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortDefault.begin();
//			     i!=m_InputPortDefault.end(); i++)
//			{
//				s<<*i<<" ";
//			}
		}
		break;
		case 5:
		{
//			s<<m_Gain<<" ";
//			s<<m_UniqueID<<" ";
//			s<<m_InputPortMin.size()<<" ";
//			assert(m_InputPortMin.size()==m_InputPortMax.size());
//			assert(m_InputPortMin.size()==m_InputPortClamp.size());
//			assert(m_InputPortMin.size()==m_InputPortDefault.size());
//			for (vector<float>::iterator i=m_InputPortMin.begin();
//			     i!=m_InputPortMin.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortMax.begin();
//			     i!=m_InputPortMax.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<bool>::iterator i=m_InputPortClamp.begin();
//			     i!=m_InputPortClamp.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortDefault.begin();
//			     i!=m_InputPortDefault.end(); i++)
//			{
//				s<<*i<<" ";
//			}
		}
		break;
		case 4:
		{
//			s<<m_Gain<<" ";
//			s<<m_UniqueID<<" ";
//			s<<m_InputPortMin.size()<<" ";
//			assert(m_InputPortMin.size()==m_InputPortMax.size());
//			assert(m_InputPortMin.size()==m_InputPortClamp.size());
//			assert(m_InputPortMin.size()==m_InputPortDefault.size());
//			for (vector<float>::iterator i=m_InputPortMin.begin();
//			     i!=m_InputPortMin.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortMax.begin();
//			     i!=m_InputPortMax.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<bool>::iterator i=m_InputPortClamp.begin();
//			     i!=m_InputPortClamp.end(); i++)
//			{
//				s<<*i<<" ";
//			}
		}
		break;
		case 3:
		{
//			s<<m_Gain<<" ";
//			s<<m_Filename<<" ";
//			s<<m_Label<<" ";
//			s<<m_InputPortMin.size()<<" ";
//			assert(m_InputPortMin.size()==m_InputPortMax.size());
//			assert(m_InputPortMin.size()==m_InputPortClamp.size());
//			for (vector<float>::iterator i=m_InputPortMin.begin();
//				 i!=m_InputPortMin.end(); i++)
//			{
//			s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortMax.begin();
//				 i!=m_InputPortMax.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<bool>::iterator i=m_InputPortClamp.begin();
//			 i!=m_InputPortClamp.end(); i++)
//			{
//				s<<*i<<" ";
//			}
		}
		break;
		case 2:
		{
//			s<<m_Gain<<" ";
//			s<<m_Filename<<" ";
//			s<<m_Label<<" ";
//			s<<m_InputPortMin.size()<<" ";
//			assert(m_InputPortMin.size()==m_InputPortMax.size());
//			for (vector<float>::iterator i=m_InputPortMin.begin();
//				 i!=m_InputPortMin.end(); i++)
//			{
//				s<<*i<<" ";
//			}
//			for (vector<float>::iterator i=m_InputPortMax.begin();
//				 i!=m_InputPortMax.end(); i++)
//			{
//				s<<*i<<" ";
//			}
		}
		break;
		case 1:
		{
//			s<<m_Gain<<" ";
//			s<<m_Filename<<" ";
//			s<<m_Label<<" ";
		}
		break;
	}
}

void LADSPAPlugin::StreamIn(istream &s)
{
	int Version;
	float Gain;
	unsigned long UniqueID;
	int PortCount;
	float Min, Max;
	bool Clamp;
	float Default;

	ClearPlugin();

	s >> Version;

	switch (Version)
	{
		case 9:
		{
			s >> m_Page;
			s >> m_UpdateInputs;
			s >> UniqueID;
			s >> PortCount;
			s >> m_UnconnectedInputs;

			for (int n=0; n<PortCount; n++)
			{
				s >> Min;
				m_InputPortMin.push_back(Min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s >> Max;
				m_InputPortMax.push_back(Max);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Clamp;
				m_InputPortClamp.push_back(Clamp);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Default;
				m_InputPortDefault.push_back(Default);
			}
		}
		break;
		case 8:
		{
			s >> m_Page;
			s >> m_UpdateInputs;
			s >> UniqueID;
			s >> PortCount;

			for (int n=0; n<PortCount; n++)
			{
				s >> Min;
				m_InputPortMin.push_back(Min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s >> Max;
				m_InputPortMax.push_back(Max);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Clamp;
				m_InputPortClamp.push_back(Clamp);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Default;
				m_InputPortDefault.push_back(Default);
			}
		}
		break;
		case 7:
		{
			s >> m_Page;
			s >> UniqueID;
			s >> PortCount;

			for (int n=0; n<PortCount; n++)
			{
				s >> Min;
				m_InputPortMin.push_back(Min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s >> Max;
				m_InputPortMax.push_back(Max);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Clamp;
				m_InputPortClamp.push_back(Clamp);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Default;
				m_InputPortDefault.push_back(Default);
			}
		}
		break;
		case 6:
		{
			s >> UniqueID;
			s >> PortCount;

			for (int n=0; n<PortCount; n++)
			{
				s >> Min;
				m_InputPortMin.push_back(Min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s >> Max;
				m_InputPortMax.push_back(Max);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Clamp;
				m_InputPortClamp.push_back(Clamp);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Default;
				m_InputPortDefault.push_back(Default);
			}
		}
		break;
		case 5:
		{
			s >> Gain;
			s >> UniqueID;
			s >> PortCount;

			for (int n=0; n<PortCount; n++)
			{
				s >> Min;
				m_InputPortMin.push_back(Min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s >> Max;
				m_InputPortMax.push_back(Max);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Clamp;
				m_InputPortClamp.push_back(Clamp);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Default;
				m_InputPortDefault.push_back(Default);
			}
		}
		break;
		case 4:
		{
			s >> Gain;
			s >> UniqueID;
			s >> PortCount;

			for (int n=0; n<PortCount; n++)
			{
				s >> Min;
				m_InputPortMin.push_back(Min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s >> Max;
				m_InputPortMax.push_back(Max);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Clamp;
				m_InputPortClamp.push_back(Clamp);
			}
			for (int n=0; n<PortCount; n++)
			{
			// Set defaults to zero
				m_InputPortDefault.push_back(0.0f);
			}
		}
		break;

		case 3:
		{
			string Filename,Label;

			s >> Gain;
			s >> Filename;
			s >> Label;
			s >> PortCount;

			for (int n=0; n<PortCount; n++)
			{
				s >> Min;
				m_InputPortMin.push_back(Min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s >> Max;
				m_InputPortMax.push_back(Max);
			}
			for (int n=0; n<PortCount; n++)
			{
				s >> Clamp;
				m_InputPortClamp.push_back(Clamp);
			}

			for (int n=0; n<PortCount; n++)
			{
			// Set defaults to zero
				m_InputPortDefault.push_back(0.0f);
			}

			if (Filename!="None")
			{
			// Get Unique ID from filename and label
				UniqueID = m_LADSPAInfo->GetIDFromFilenameAndLabel(Filename, Label);
			}
		}
		break;

		case 2:
		{
			string Filename, Label;

			s >> Gain;
			s >> Filename;
			s >> Label;
			s >> PortCount;

			for (int n=0; n<PortCount; n++)
			{
				s >> Min;
				m_InputPortMin.push_back(Min);
			}

			for (int n=0; n<PortCount; n++)
			{
				s >> Max;
				m_InputPortMax.push_back(Max);
			}

			for (int n=0; n<PortCount; n++)
			{
			// Set InputPortClamp to true as default
				m_InputPortClamp.push_back(true);
			}

			for (int n=0; n<PortCount; n++)
			{
			// Set defaults to zero
				m_InputPortDefault.push_back(0.0f);
			}

			if (Filename!="None")
			{
			// Get Unique ID from filename and label
				UniqueID = m_LADSPAInfo->GetIDFromFilenameAndLabel(Filename, Label);
			}
		}
		break;

		case 1:
		{
			string Filename, Label;

			s >> Gain;
			s >> Filename;
			s >> Label;

			if (Filename!="None")
			{
			// Get Unique ID from filename and label
				UniqueID = m_LADSPAInfo->GetIDFromFilenameAndLabel(Filename, Label);
			}
		}
		break;
	}

	if (Version == 1) {
	// Need to reset ports - none will have been saved
		UpdatePlugin(UniqueID);
	} else {
	// Versions prior to 9 have 'Setup' page as page 1
	// This is now page 2, as the sliders are page 1
		if (Version < 9 && m_Page == 1) m_Page = 2;

		if (SelectPlugin(UniqueID)) {
		// Versions prior to 9 will not have the unconnected inupts saved
		// Default to the number of input ports.
			if (Version < 9) m_UnconnectedInputs = m_InputPortCount;

			SetGUIExports();
		} else {
			ClearPlugin();
		}
	}
}

bool LADSPAPlugin::UpdatePlugin(unsigned long UniqueID)
{
	ClearPlugin();
	if (SelectPlugin(UniqueID)) {
		ResetPortSettings();
		SetGUIExports();
		return true;
	}

// Oops. Clean up.
	ClearPlugin();
	cerr << "Error loading LADSPA Plugin.\n";

	return false;
}

bool LADSPAPlugin::SelectPlugin(unsigned long UniqueID)
{
// Reject trivial case
	if (UniqueID == 0) return false;

	m_PlugDesc = m_LADSPAInfo->GetDescriptorByID(UniqueID);

	if (m_PlugDesc) {
	// Create instance
		if (!(m_PlugInstHandle = m_PlugDesc->instantiate(m_PlugDesc, m_HostInfo->SAMPLERATE))) {
			cerr << "WARNING: Could not instantiate plugin " << UniqueID << endl;
			m_LADSPAInfo->DiscardDescriptorByID(UniqueID);
			m_PlugDesc = 0;
			return false;
		}

		// Find number of input and output ports
		for (unsigned long i = 0; i < m_PlugDesc->PortCount; i++) {
			if (LADSPA_IS_PORT_INPUT(m_PlugDesc->PortDescriptors[i])) {
				m_PluginInfo.NumInputs++;
			} else if (LADSPA_IS_PORT_OUTPUT(m_PlugDesc->PortDescriptors[i])) {
				m_PluginInfo.NumOutputs++;
			}
		}

/////////////////////////////////
// LADSPA Buffers

		unsigned long c=0;
		for (unsigned int n=0; n<m_PlugDesc->PortCount; n++)
		{
			if (LADSPA_IS_PORT_INPUT(m_PlugDesc->PortDescriptors[n]))
			{
				LADSPA_Data *NewPort = new LADSPA_Data[m_HostInfo->BUFSIZE];
				m_LADSPABufVec.push_back(NewPort);
				m_PlugDesc->connect_port(m_PlugInstHandle, n, m_LADSPABufVec[c]);
				m_PortID.push_back(n);
				c++;
			}
		}

		for (unsigned int n=0; n<m_PlugDesc->PortCount; n++)
		{
			if (LADSPA_IS_PORT_OUTPUT(m_PlugDesc->PortDescriptors[n]))
			{
				LADSPA_Data *NewPort = new LADSPA_Data[m_HostInfo->BUFSIZE];
				m_LADSPABufVec.push_back(NewPort);
				m_PlugDesc->connect_port(m_PlugInstHandle, n, m_LADSPABufVec[c]);
				m_PortID.push_back(n);
				c++;
			}
		}

		// activate the plugin now
		if (m_PlugDesc->activate)
			m_PlugDesc->activate(m_PlugInstHandle);

/////////////////////////////////
// SSM Buffers

		// Allocate the i/o buffers required
		for (int n=0; n<m_PluginInfo.NumInputs; n++) AddInput();
		for (int n=0; n<m_PluginInfo.NumOutputs; n++) AddOutput();

//////////////////////////////
// Update the GUI stuff

		string desc;
		c=0;
		for (unsigned int i = 0; i < m_PlugDesc->PortCount; i++)
		{
			if (LADSPA_IS_PORT_INPUT(m_PlugDesc->PortDescriptors[i]))
			{
				desc = string(m_PlugDesc->PortNames[i]) +
					(LADSPA_IS_PORT_CONTROL(m_PlugDesc->PortDescriptors[i]) ? " (CV)" : " (AU)");
				m_PluginInfo.PortTips.push_back(desc.c_str());

				c++;
			}
		}

		for (unsigned int i = 0; i < m_PlugDesc->PortCount; i++)
		{
			if (LADSPA_IS_PORT_OUTPUT(m_PlugDesc->PortDescriptors[i])) {

				desc = string(m_PlugDesc->PortNames[i]) +
					(LADSPA_IS_PORT_CONTROL(m_PlugDesc->PortDescriptors[i]) ? " (CV)" : " (AU)");

				m_PluginInfo.PortTips.push_back(desc.c_str());
			}
		}

		UpdatePluginInfoWithHost();

		m_UniqueID = m_PlugDesc->UniqueID;
		m_InputPortCount = m_PluginInfo.NumInputs;

		int lbl_length;

		lbl_length = strlen(m_PlugDesc->Name);
		lbl_length = lbl_length > 255 ? 255 : lbl_length;
		strncpy(m_Name, m_PlugDesc->Name, lbl_length);
		m_Name[lbl_length] = '\0';

		lbl_length = strlen(m_PlugDesc->Maker);
		lbl_length = lbl_length > 255 ? 255 : lbl_length;
		strncpy(m_Maker, m_PlugDesc->Maker, lbl_length);
		m_Maker[lbl_length] = '\0';

		return true;
	}
	return false;
}

void LADSPAPlugin::ClearPlugin(void)
{
// Clear selected plugin
	if (m_PlugDesc) {
		if (m_PlugDesc->deactivate) m_PlugDesc->deactivate(m_PlugInstHandle);
		m_PlugDesc->cleanup(m_PlugInstHandle);
		m_PlugDesc = NULL;

		m_LADSPAInfo->DiscardDescriptorByID(m_UniqueID);
	}

	m_Page = 1;
	m_UpdateInputs = true;
	m_UniqueID = 0;
	m_InputPortCount = 0;
	m_UnconnectedInputs = 0;
	strncpy(m_Name, "None\0", 5);
	strncpy(m_Maker, "None\0", 5);

	for(vector<LADSPA_Data*>::iterator i=m_LADSPABufVec.begin();
		i!=m_LADSPABufVec.end(); i++)
	{
		if (*i) delete[] (*i);
	}
	m_LADSPABufVec.clear();

	RemoveAllInputs();
	RemoveAllOutputs();

	m_PluginInfo.NumInputs = 0;
	m_PluginInfo.NumOutputs = 0;
	m_PluginInfo.PortTips.clear();

	m_PortID.clear();
	m_InputPortMin.clear();
	m_InputPortMax.clear();
	m_InputPortClamp.clear();
	m_InputPortDefault.clear();
}

void LADSPAPlugin::ResetPortSettings(void)
{
	for (int n=0; n<m_PluginInfo.NumInputs; n++)
	{
		float Max=1.0f, Min=-1.0f, Default=0.0f;
		int Port=m_PortID[n];

		// Get the bounding hints for the port
		LADSPA_PortRangeHintDescriptor HintDesc=m_PlugDesc->PortRangeHints[Port].HintDescriptor;
		if (LADSPA_IS_HINT_BOUNDED_BELOW(HintDesc))
		{
			Min=m_PlugDesc->PortRangeHints[Port].LowerBound;
			if (LADSPA_IS_HINT_SAMPLE_RATE(HintDesc))
			{
				Min*=m_HostInfo->SAMPLERATE;
			}
		}
		if (LADSPA_IS_HINT_BOUNDED_ABOVE(HintDesc))
		{
			Max=m_PlugDesc->PortRangeHints[Port].UpperBound;
			if (LADSPA_IS_HINT_SAMPLE_RATE(HintDesc))
			{
				Max*=m_HostInfo->SAMPLERATE;
			}
		}

#ifdef LADSPA_VERSION
// We've got a version of the header that supports port defaults
		if (LADSPA_IS_HINT_HAS_DEFAULT(HintDesc)) {
		// LADSPA_HINT_DEFAULT_0 is assumed anyway, so we don't check for it
			if (LADSPA_IS_HINT_DEFAULT_1(HintDesc)) {
				Default = 1.0f;
			} else if (LADSPA_IS_HINT_DEFAULT_100(HintDesc)) {
				Default = 100.0f;
			} else if (LADSPA_IS_HINT_DEFAULT_440(HintDesc)) {
				Default = 440.0f;
			} else {
			// These hints may be affected by SAMPLERATE, LOGARITHMIC and INTEGER
				if (LADSPA_IS_HINT_DEFAULT_MINIMUM(HintDesc) &&
				    LADSPA_IS_HINT_BOUNDED_BELOW(HintDesc)) {
					Default=m_PlugDesc->PortRangeHints[Port].LowerBound;
				} else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(HintDesc) &&
				           LADSPA_IS_HINT_BOUNDED_ABOVE(HintDesc)) {
					Default=m_PlugDesc->PortRangeHints[Port].UpperBound;
				} else if (LADSPA_IS_HINT_BOUNDED_BELOW(HintDesc) &&
				           LADSPA_IS_HINT_BOUNDED_ABOVE(HintDesc)) {
				// These hints require both upper and lower bounds
					float lp = 0.0f, up = 0.0f;
					float min = m_PlugDesc->PortRangeHints[Port].LowerBound;
					float max = m_PlugDesc->PortRangeHints[Port].UpperBound;
					if (LADSPA_IS_HINT_DEFAULT_LOW(HintDesc)) {
						lp = 0.75f;
						up = 0.25f;
					} else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(HintDesc)) {
						lp = 0.5f;
						up = 0.5f;
					} else if (LADSPA_IS_HINT_DEFAULT_HIGH(HintDesc)) {
						lp = 0.25f;
						up = 0.75f;
					}

					if (LADSPA_IS_HINT_LOGARITHMIC(HintDesc)) {
						if (min==0.0f || max==0.0f) {
						// Zero at either end means zero no matter
						// where hint is at, since:
						//  log(n->0) -> Infinity
							Default = 0.0f;
						} else {
						// Catch negatives
							bool neg_min = min < 0.0f ? true : false;
							bool neg_max = max < 0.0f ? true : false;

							if (!neg_min && !neg_max) {
								Default = exp(log(min) * lp + log(max) * up);
							} else if (neg_min && neg_max) {
								Default = -exp(log(-min) * lp + log(-max) * up);
							} else {
							// Logarithmic range has asymptote
							// so just use linear scale
								Default = min * lp + max * up;
							}
						}
					} else {
						Default = min * lp + max * up;
					}
				}
				if (LADSPA_IS_HINT_SAMPLE_RATE(HintDesc)) {
					Default *= m_HostInfo->SAMPLERATE;
				}
				if (LADSPA_IS_HINT_INTEGER(HintDesc)) {
					Default = floorf(Default);
				}
			}
		}
#else
// No LADSPA_VERSION - implies no defaults
#warning ************************************
#warning Your LADSPA header is out of date!
#warning Please get the latest sdk from
#warning     www.ladspa.org
#warning Defaults will not be used.
#warning ************************************
		Default = 0.0f;
#endif

		m_InputPortMin.push_back(Min);
		m_InputPortMax.push_back(Max);
		m_InputPortClamp.push_back(true);
		m_InputPortDefault.push_back(Default);
	}
}

void LADSPAPlugin::SetGUIExports(void)
{
	int lbl_length;
	char *lbl_start;

	lbl_start = m_OutData.InputPortNames;
	for (unsigned long p = 0; p < m_InputPortCount; p++) {
		int Port = m_PortID[p];
		LADSPA_PortRangeHintDescriptor HintDesc=m_PlugDesc->PortRangeHints[Port].HintDescriptor;

	// Port Labels
		lbl_length = m_PluginInfo.PortTips[p].size();
		lbl_length = lbl_length > 255 ? 255 : lbl_length;
		strncpy(lbl_start, m_PluginInfo.PortTips[p].c_str(), lbl_length);
		lbl_start[lbl_length] = '\0';
		lbl_start += 256;

		m_OutData.InputPortSettings[p].Integer = LADSPA_IS_HINT_INTEGER(HintDesc);
		if (LADSPA_IS_HINT_LOGARITHMIC(HintDesc)) {
			if (LADSPA_IS_HINT_SAMPLE_RATE(HintDesc)) {
				m_OutData.InputPortSettings[p].LogBase = 2.0f;
			} else {
				m_OutData.InputPortSettings[p].LogBase = 10.0f;
			}
		} else {
			m_OutData.InputPortSettings[p].LogBase = 0.0f;
		}

		m_OutData.InputPortSettings[p].Min = m_InputPortMin[p];
		m_OutData.InputPortSettings[p].Max = m_InputPortMax[p];
		m_OutData.InputPortSettings[p].Clamp = m_InputPortClamp[p];
		m_OutData.InputPortDefaults[p] = m_InputPortDefault[p];
	}
}
