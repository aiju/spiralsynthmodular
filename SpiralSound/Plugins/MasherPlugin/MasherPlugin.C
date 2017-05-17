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
#include <math.h>
#include "MasherPlugin.h"
#include "MasherPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

#define PI 3.141592654

using namespace std;

float RandRange(float L, float H)
{
	return ((rand()%10000/10000.0f)*(H-L))+L;
}

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new MasherPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 54;
}

string SpiralPlugin_GetGroupName()
{
	return "Filters/FX";
}
}

///////////////////////////////////////////////////////

MasherPlugin::MasherPlugin() :
m_GrainStoreSize(MAX_GRAINSTORE_SIZE),
m_Density(10),
m_Randomness(0),
m_GrainPitch(1),
m_ReadGrain(0),
m_WriteGrain(0)
{
	m_PluginInfo.Name="Masher";
	m_PluginInfo.Width=120;
	m_PluginInfo.Height=140;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");
	m_PluginInfo.PortTips.push_back("GrainPitch");
	m_PluginInfo.PortTips.push_back("Density");
	m_PluginInfo.PortTips.push_back("Output");
	
	m_AudioCH->Register("GrainPitch",&m_GrainPitch);	
	m_AudioCH->Register("GrainStoreSize",&m_GrainStoreSize);	
	m_AudioCH->Register("Density",&m_Density);	
	m_AudioCH->Register("Randomness",&m_Randomness);	
}

MasherPlugin::~MasherPlugin()
{
}

PluginInfo &MasherPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *MasherPlugin::CreateGUI()
{
	return new MasherPluginGUI(m_PluginInfo.Width,
							 m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void MixPitch(Sample &src, Sample &dst, int Pos, float Pitch)
{
	float n=0; 
	int p=Pos;
	while (n<src.GetLength() && p<dst.GetLength())
	{
		if (p>=0) dst.Set(p,dst[p]+src[n]);
		n+=Pitch;
		p++;
	}
}

void MasherPlugin::Execute()
{
	GetOutputBuf(0)->Zero();
	if (!InputExists(0)) return;
	
	int n=0;
	float s=GetInput(0,0);
	int last=0;
	bool First=true;
	
	// paste any overlapping grains to the start of the buffer.
	for (vector<GrainDesc>::iterator i = m_OverlapVec.begin();
		 i!=m_OverlapVec.end(); i++)
	{
		MixPitch(m_GrainStore[i->Grain],*GetOutputBuf(0),i->Pos-m_HostInfo->BUFSIZE,m_GrainPitch);
	}
	
	m_OverlapVec.clear();
	
	// chop up the buffer and put the grains in the grainstore	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		if ((s<0 && GetInput(0,n)>0) || (s>0 && GetInput(0,n)<0))
		{
			// chop the bits between zero crossings
			if (!First) 
			{
				GetInput(0)->GetRegion(m_GrainStore[m_WriteGrain%m_GrainStoreSize],last,n);
				m_WriteGrain++;
			}
			else First=false;
			
			last=n;
			s=GetInput(0,n);
		}
	}
		
	int NextGrain=0;
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		int Density = m_Density;
		if (InputExists(2)) Density=(int)(Density*GetInput(2,n));
		
		if(n>=NextGrain || rand()%1000<Density)
		{
			int GrainNum = m_ReadGrain%m_GrainStoreSize;
			float Pitch=m_GrainPitch;
			if (InputExists(1)) Pitch*=fabs(GetInput(1,n));
			MixPitch(m_GrainStore[GrainNum],*GetOutputBuf(0),n,Pitch);
			int GrainLength = (int)(m_GrainStore[GrainNum].GetLength()*Pitch);
			
			NextGrain=n+m_GrainStore[GrainNum].GetLength();
			
			// if this grain overlaps the buffer
			if (n+GrainLength>m_HostInfo->BUFSIZE)
			{
				GrainDesc new_grain;
				new_grain.Pos=n;
				new_grain.Grain=GrainNum;
				m_OverlapVec.push_back(new_grain);
			}
			
			if (m_Randomness) m_ReadGrain+=1+rand()%m_Randomness;
			else m_ReadGrain++;
		}
	}		
}
	
void MasherPlugin::Randomise()
{
}
	
void MasherPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	s<<m_GrainStoreSize<<" "<<m_Density<<" "<<m_Randomness<<" "<<m_GrainPitch<<" ";
}

void MasherPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	s>>m_GrainStoreSize>>m_Density>>m_Randomness>>m_GrainPitch;
}
