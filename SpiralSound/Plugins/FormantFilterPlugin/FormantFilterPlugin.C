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
#include "FormantFilterPlugin.h"
#include "FormantFilterPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

using namespace std;

/*
Public source code by alex@smartelectronix.com
Simple example of implementation of formant filter
Vowelnum can be 0,1,2,3,4 <=> A,E,I,O,U
Good for spectral rich input like saw or square
*/

//-------------------------------------------------------------VOWEL COEFFICIENTS
const double coeff[5][11]= {
{ 8.11044e-06,
8.943665402, -36.83889529, 92.01697887, -154.337906, 181.6233289,
-151.8651235,   89.09614114, -35.10298511, 8.388101016, -0.923313471  ///A
},
{4.36215e-06,
8.90438318, -36.55179099, 91.05750846, -152.422234, 179.1170248,  ///E
-149.6496211,87.78352223, -34.60687431, 8.282228154, -0.914150747
},
{ 3.33819e-06,
8.893102966, -36.49532826, 90.96543286, -152.4545478, 179.4835618,
-150.315433, 88.43409371, -34.98612086, 8.407803364, -0.932568035  ///I
},
{1.13572e-06,
8.994734087, -37.2084849, 93.22900521, -156.6929844, 184.596544,   ///O
-154.3755513, 90.49663749, -35.58964535, 8.478996281, -0.929252233
},
{4.09431e-07,
8.997322763, -37.20218544, 93.11385476, -156.2530937, 183.7080141,  ///U
-153.2631681, 89.59539726, -35.12454591, 8.338655623, -0.910251753
}
};
//---------------------------------------------------------------------------------

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new FormantFilterPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 42;
}

string SpiralPlugin_GetGroupName()
{
	return "Filters/FX";
}
}

///////////////////////////////////////////////////////

FormantFilterPlugin::FormantFilterPlugin() :
m_Vowel(0)
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			memory[i][j] = 0;
		}
	}

	m_PluginInfo.Name="FormantFilter";
	m_PluginInfo.Width=90;
	m_PluginInfo.Height=110;
	m_PluginInfo.NumInputs=2;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");
	m_PluginInfo.PortTips.push_back("Vowel CV");	
	m_PluginInfo.PortTips.push_back("Output");	
	
	m_AudioCH->Register("Vowel",&m_Vowel);
}

FormantFilterPlugin::~FormantFilterPlugin()
{
}

PluginInfo &FormantFilterPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);		
}

SpiralGUIType *FormantFilterPlugin::CreateGUI()
{
	return new FormantFilterPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void FormantFilterPlugin::Execute()
{
	float res,o[5],out=0, in=0;
	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{		
		//reset memory if disconnected, and skip out (prevents CPU spike)
		if (! InputExists(0)) 
		{	
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					memory[i][j] = 0;
				}
			}
			out = 0;	 
		}
		else
		{
			in = GetInput(0,n);
			
			for (int v=0; v<5; v++)
			{
				res= (float) (coeff[v][0]*(in*0.1f) +
						  coeff[v][1]*memory[v][0] +  
						  coeff[v][2]*memory[v][1] +
						  coeff[v][3]*memory[v][2] +
						  coeff[v][4]*memory[v][3] +
						  coeff[v][5]*memory[v][4] +
						  coeff[v][6]*memory[v][5] +
						  coeff[v][7]*memory[v][6] +
						  coeff[v][8]*memory[v][7] +
						  coeff[v][9]*memory[v][8] +
						  coeff[v][10]*memory[v][9] );

				memory[v][9]=memory[v][8];
				memory[v][8]=memory[v][7];
				memory[v][7]=memory[v][6];
				memory[v][6]=memory[v][5];
				memory[v][5]=memory[v][4];
				memory[v][4]=memory[v][3];
				memory[v][3]=memory[v][2];
				memory[v][2]=memory[v][1];
				memory[v][1]=memory[v][0];
				memory[v][0]=(double) res;
			
				o[v]=res;
			}
			
			if (InputExists(1)) 
			{	
				m_Vowel=fabs(GetInput(1,n))*4.0f;
			}
		
			// mix between vowel sounds
			if (m_Vowel<1) out=Linear(0,1,m_Vowel,o[1],o[0]); 
			else if (m_Vowel>1 && m_Vowel<2) out=Linear(0,1,m_Vowel-1.0f,o[2],o[1]);
			else if (m_Vowel>2 && m_Vowel<3) out=Linear(0,1,m_Vowel-2.0f,o[3],o[2]);
			else if (m_Vowel>3 && m_Vowel<4) out=Linear(0,1,m_Vowel-3.0f,o[4],o[3]);
			else if (m_Vowel==4) out=o[4];
		}		
		SetOutput(0,n,out);	 
	}
		
}
	
void FormantFilterPlugin::Randomise()
{
}
	
void FormantFilterPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" "<<m_Vowel<<" ";
}

void FormantFilterPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	s>>m_Vowel;
}
