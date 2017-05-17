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
#include "PoshSamplerPlugin.h"
#include "PoshSamplerPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"
#include <stdio.h>

#include "../../RiffWav.h"

using namespace std;

static const int   NOTETRIG    = NUM_SAMPLES*2+1;
static const int   REC_INPUT   = 16;

static const int   S1_INPUT   = 18;
static const int   S2_INPUT   = 19;
static const int   S3_INPUT   = 20;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new PoshSamplerPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 32;
}

string SpiralPlugin_GetGroupName()
{
	return "Delay/Sampling";
}
}

///////////////////////////////////////////////////////

static void 
InitializeSampleDescription(SampleDesc* NewDesc, const string &Pathname, int Note)
{
	  if (NewDesc) {
		NewDesc->Pathname   = Pathname; 
		NewDesc->Volume     = 1.0f;
		NewDesc->Velocity   = 1.0f;
		NewDesc->Pitch      = 1.0f;
		NewDesc->PitchMod   = 1.0f;
		NewDesc->SamplePos  = -1;
		NewDesc->Loop       = false;
		NewDesc->PingPong   = false;
		NewDesc->Note       = Note;	
		NewDesc->Octave     = 0;
		NewDesc->TriggerUp  = true;
		NewDesc->SamplePos  = -1;
		NewDesc->SampleRate = 44100;
		NewDesc->Stereo     = false;
		NewDesc->PlayStart  = 0;
		NewDesc->LoopStart  = 0;
		NewDesc->LoopEnd    = INT_MAX;
	}
}

PoshSamplerPlugin::PoshSamplerPlugin() :
m_Recording(false)
{
	m_PluginInfo.Name="PoshSampler";
	m_PluginInfo.Width=400;
	m_PluginInfo.Height=215;
	m_PluginInfo.NumInputs=21;
	m_PluginInfo.NumOutputs=9;
	m_PluginInfo.PortTips.push_back("Sample 1 Pitch");
	m_PluginInfo.PortTips.push_back("Sample 1 Trigger");
	m_PluginInfo.PortTips.push_back("Sample 2 Pitch");
	m_PluginInfo.PortTips.push_back("Sample 2 Trigger");
	m_PluginInfo.PortTips.push_back("Sample 3 Pitch");
	m_PluginInfo.PortTips.push_back("Sample 3 Trigger");
	m_PluginInfo.PortTips.push_back("Sample 4 Pitch");
	m_PluginInfo.PortTips.push_back("Sample 4 Trigger");
	m_PluginInfo.PortTips.push_back("Sample 5 Pitch");
	m_PluginInfo.PortTips.push_back("Sample 5 Trigger");
	m_PluginInfo.PortTips.push_back("Sample 6 Pitch");
	m_PluginInfo.PortTips.push_back("Sample 6 Trigger");
	m_PluginInfo.PortTips.push_back("Sample 7 Pitch");
	m_PluginInfo.PortTips.push_back("Sample 7 Trigger");
	m_PluginInfo.PortTips.push_back("Sample 8 Pitch");
	m_PluginInfo.PortTips.push_back("Sample 8 Trigger");
	m_PluginInfo.PortTips.push_back("Input");
	m_PluginInfo.PortTips.push_back("Sample trigger pitch");
	m_PluginInfo.PortTips.push_back("Sample 1 Start Pos");
	m_PluginInfo.PortTips.push_back("Sample 2 Start Pos");
	m_PluginInfo.PortTips.push_back("Sample 3 Start Pos");
	m_PluginInfo.PortTips.push_back("Mixed Output");
	m_PluginInfo.PortTips.push_back("Sample 1 Output");
	m_PluginInfo.PortTips.push_back("Sample 2 Output");
	m_PluginInfo.PortTips.push_back("Sample 3 Output");
	m_PluginInfo.PortTips.push_back("Sample 4 Output");
	m_PluginInfo.PortTips.push_back("Sample 5 Output");
	m_PluginInfo.PortTips.push_back("Sample 6 Output");
	m_PluginInfo.PortTips.push_back("Sample 7 Output");
	m_PluginInfo.PortTips.push_back("Sample 8 Output");

	for (int n=0; n<NUM_SAMPLES; n++)
	{
		Sample* NewSample = new Sample;
		m_SampleVec.push_back(NewSample);

		SampleDesc* NewDesc = new SampleDesc;
		char temp[256];
		sprintf (temp, "PoshSampler%d_%d", GetID(), n);
 		InitializeSampleDescription(NewDesc, temp, n);
		m_SampleDescVec.push_back(NewDesc);
	}

	m_Version=3;
	m_Current = 0;

	m_AudioCH->Register("Num",&m_GUIArgs.Num);
	m_AudioCH->Register("Value",&m_GUIArgs.Value);
	m_AudioCH->Register("Bool",&m_GUIArgs.Boole);
	m_AudioCH->Register("Int",&m_GUIArgs.Int);
	m_AudioCH->Register("Start",&m_GUIArgs.Start);
	m_AudioCH->Register("End",&m_GUIArgs.End);
	m_AudioCH->Register("LoopStart",&m_GUIArgs.LoopStart);
	m_AudioCH->RegisterData("Name",ChannelHandler::INPUT,&m_GUIArgs.Name,sizeof(m_GUIArgs.Name));
	m_AudioCH->Register("PlayPos",&m_CurrentPlayPos,ChannelHandler::OUTPUT);
	m_AudioCH->RegisterData("SampleBuffer",ChannelHandler::OUTPUT_REQUEST,&m_SampleBuffer,TRANSBUF_SIZE);
	m_AudioCH->Register("SampleSize",&m_SampleSize,ChannelHandler::OUTPUT_REQUEST);

}

PoshSamplerPlugin::~PoshSamplerPlugin()
{
	for (vector<Sample*>::iterator i=m_SampleVec.begin();
		 i!=m_SampleVec.end(); i++)
	{
		delete(*i);
	}

	for (vector<SampleDesc*>::iterator i=m_SampleDescVec.begin();
		 i!=m_SampleDescVec.end(); i++)
	{
		delete(*i);
	}
}

PluginInfo &PoshSamplerPlugin::Initialise(const HostInfo *Host)
{
	return SpiralPlugin::Initialise(Host);;
}

SpiralGUIType *PoshSamplerPlugin::CreateGUI()
{
	return new PoshSamplerPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void PoshSamplerPlugin::Execute()
{
	static bool Pong=false;

	for (int s=0; s<NUM_SAMPLES+1; s++)
	{
		GetOutputBuf(s)->Zero();
	}

	float Freq=0;

	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		Freq=GetInputPitch(NOTETRIG,n);

		for (int s=0; s<NUM_SAMPLES; s++)
		{
			SampleDesc* S=m_SampleDescVec[s];

			// if we have a sample here
			if (m_SampleVec[s]->GetLength())
			{
				// Convert the CV input into a useable trigger

				if (GetInput(s*2+1,n)>0 || feq(Freq,NoteTable[S->Note],0.01f))
				{
					if (S->TriggerUp)
					{
						if (s==0 && InputExists(S1_INPUT))
							S->PlayStart=(long int)((GetInput(S1_INPUT,n)*0.5+0.5f)*(S->LoopEnd-S->LoopStart))+S->LoopStart;
						if (s==1 && InputExists(S2_INPUT))
							S->PlayStart=(long int)((GetInput(S2_INPUT,n)*0.5+0.5f)*(S->LoopEnd-S->LoopStart))+S->LoopStart;
						if (s==2 && InputExists(S3_INPUT))
							S->PlayStart=(long int)((GetInput(S3_INPUT,n)*0.5+0.5f)*(S->LoopEnd-S->LoopStart))+S->LoopStart;

						if (S->PlayStart<0) S->PlayStart=0;

						S->SamplePos=S->PlayStart;
						S->TriggerUp=false;
						S->Velocity=GetInput(s*2+1,n);
					}
				}
				else
				{
					S->TriggerUp=true;

					// end it if it's looping
					if (S->Loop)
					{
						S->SamplePos=-1;
					}
				}

				// if the sample has ended
				if (S->SamplePos>=S->LoopEnd || S->SamplePos>=m_SampleVec[s]->GetLength())
				{
					if (S->Loop)
					{
						if (S->PingPong) Pong=true;
						else S->SamplePos=S->LoopStart;
					}
					else
					{
						S->SamplePos=-1;
					}
				}

				// if the sample has ended ponging
				if (Pong && S->SamplePos<=S->LoopStart)
				{
					Pong=false;
				}

				if (S->SamplePos!=-1)
				{
					if (InputExists(s*2))
					{
						// Get the pitch from the CV
						float PlayFreq=GetInputPitch(s*2,n);

						// assumtion: base frequency = 440 (middle A)
						S->Pitch = PlayFreq/440;
						S->Pitch *= S->SampleRate/(float)m_HostInfo->SAMPLERATE;
					}

					// mix the sample to the output.
					MixOutput(0,n,(*m_SampleVec[s])[S->SamplePos]*S->Volume*S->Velocity);
					// copy the sample to it's individual output.
					SetOutput(s+1,n,((*m_SampleVec[s])[S->SamplePos]*S->Volume));

					float Freq=S->Pitch;
					if (S->Octave>0) Freq*=1<<(S->Octave);
					if (S->Octave<0) Freq/=1<<(-S->Octave);

					if (Pong) S->SamplePos-=Freq*S->PitchMod;
					else S->SamplePos+=Freq*S->PitchMod;
				}
			}
		}
	}

	// record
	static int LastRecording=false;
	if(m_Recording && InputExists(REC_INPUT))
	{
		int s=0;//GUI->GetCurrentSample();

		if (!LastRecording) m_SampleVec[s]->Clear();

		// new sample
		if (m_SampleVec[s]->GetLength()==0)
		{
			*m_SampleVec[s]=*GetInput(REC_INPUT);

			m_SampleDescVec[s]->SampleRate=m_HostInfo->SAMPLERATE;
			m_SampleDescVec[s]->Stereo=false;
			m_SampleDescVec[s]->Pitch *= 1.0f;
			m_SampleDescVec[s]->LoopEnd=m_SampleVec[s]->GetLength();

		}
		else
		{
			m_SampleVec[s]->Add(*GetInput(REC_INPUT));
			m_SampleDescVec[s]->LoopEnd=m_SampleVec[s]->GetLength();
		}
	}
	LastRecording=m_Recording;

	if (m_SampleDescVec[m_Current]->SamplePos>0)
	{
		m_CurrentPlayPos=(long)m_SampleDescVec[m_Current]->SamplePos;
	}
}

void PoshSamplerPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch(m_AudioCH->GetCommand())
		{
			case (LOAD)         : LoadSample(m_GUIArgs.Num,m_GUIArgs.Name); break;
			case (SAVE)         : SaveSample(m_GUIArgs.Num,m_GUIArgs.Name); break;
			case (SETVOL)       : SetVolume(m_GUIArgs.Num,m_GUIArgs.Value); break;
			case (SETPITCH)     : SetPitch(m_GUIArgs.Num,m_GUIArgs.Value); break;
			case (SETLOOP)      : SetLoop(m_GUIArgs.Num,m_GUIArgs.Boole); break;
			case (SETPING)      : SetPingPong(m_GUIArgs.Num,m_GUIArgs.Boole); break;
			case (SETNOTE)      : SetNote(m_GUIArgs.Num,m_GUIArgs.Int); break;
			case (SETOCT)       : SetOctave(m_GUIArgs.Num,m_GUIArgs.Int); break;
			case (SETPLAYPOINTS):
			{
				SetPlayStart(m_GUIArgs.Num,m_GUIArgs.Start);
				SetLoopStart(m_GUIArgs.Num,m_GUIArgs.LoopStart);
				SetLoopEnd(m_GUIArgs.Num,m_GUIArgs.End);
			} break;
			case (SETREC)       : SetRecord(m_GUIArgs.Boole); break;
			case (CUT)          : Cut(m_GUIArgs.Num,m_GUIArgs.Start,m_GUIArgs.End); break;
			case (COPY)         : Copy(m_GUIArgs.Num,m_GUIArgs.Start,m_GUIArgs.End); break;
			case (PASTE)        : Paste(m_GUIArgs.Num,m_GUIArgs.Start,m_GUIArgs.End); break;
			case (MIX)          : Mix(m_GUIArgs.Num,m_GUIArgs.Start,m_GUIArgs.End); break;
			case (CROP)         : Crop(m_GUIArgs.Num,m_GUIArgs.Start,m_GUIArgs.End); break;
			case (REV)          : Reverse(m_GUIArgs.Num,m_GUIArgs.Start,m_GUIArgs.End); break;
			case (AMP)          : Amp(m_GUIArgs.Num,m_GUIArgs.Start,m_GUIArgs.End); break;
			case (SETCURRENT)   : m_Current = m_GUIArgs.Num; break;
			case (GETSAMPLE)    :
			{
				m_AudioCH->SetupBulkTransfer((void*)m_SampleVec[m_Current]->GetBuffer());
				m_SampleSize=m_SampleVec[m_Current]->GetLengthInBytes();
			} break;
		};
	}
}

void PoshSamplerPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	for (int n=0; n<NUM_SAMPLES; n++)
	{
		s<<m_SampleDescVec[n]->Volume<<" "<<
		   m_SampleDescVec[n]->PitchMod<<" "<<
		   m_SampleDescVec[n]->Loop<<" "<<
		   m_SampleDescVec[n]->PingPong<<" "<<
		   m_SampleDescVec[n]->Note<<" "<<
		   m_SampleDescVec[n]->Octave<<" "<<
		   m_SampleDescVec[n]->SamplePos<<" "<<
		   m_SampleDescVec[n]->PlayStart<<" "<<
		   m_SampleDescVec[n]->LoopStart<<" "<<
		   m_SampleDescVec[n]->LoopEnd<<" "<<
		   m_SampleDescVec[n]->Note<<" ";
	}
}

void PoshSamplerPlugin::StreamIn(istream &s)
{
	int version;
	s>>version;

	for (int n=0; n<NUM_SAMPLES; n++)
	{
		s>>m_SampleDescVec[n]->Volume>>
		   m_SampleDescVec[n]->PitchMod>>
		   m_SampleDescVec[n]->Loop>>
		   m_SampleDescVec[n]->PingPong>>
		   m_SampleDescVec[n]->Note>>
		   m_SampleDescVec[n]->Octave>>
		   m_SampleDescVec[n]->SamplePos>>
		   m_SampleDescVec[n]->PlayStart>>
		   m_SampleDescVec[n]->LoopStart>>
		   m_SampleDescVec[n]->LoopEnd>>
		   m_SampleDescVec[n]->Note;

		if (version<3)
		{
			int size;
			s>>size;
			s.ignore(1);
			char Buf[4096];
			s.get(Buf,size+1);
		}
	}
}

void PoshSamplerPlugin::LoadSample(int n, const string &Name)
{
	WavFile Wav;
	if (Wav.Open(Name,WavFile::READ))
	{
		m_SampleVec[n]->Allocate(Wav.GetSize());
		Wav.Load(*m_SampleVec[n]);
		InitializeSampleDescription(m_SampleDescVec[n], Name, n);
		m_SampleDescVec[n]->SampleRate=Wav.GetSamplerate();
		m_SampleDescVec[n]->Stereo=Wav.IsStereo();
		m_SampleDescVec[n]->Pitch *= m_SampleDescVec[n]->SampleRate/(float)m_HostInfo->SAMPLERATE;
		m_SampleDescVec[n]->LoopEnd=m_SampleVec[n]->GetLength()-1;
	}
}

void PoshSamplerPlugin::SaveSample(int n, const string &Name)
{
	if (m_SampleVec[n]->GetLength()==0) return;

	WavFile Wav;
	Wav.Open(Name,WavFile::WRITE,WavFile::MONO);
	Wav.Save(*m_SampleVec[n]);
}

void PoshSamplerPlugin::Cut(int n, long s, long e)
{
	if (m_SampleVec[n]->GetLength()==0) return;
	m_SampleVec[n]->GetRegion(m_CopyBuffer, s, e);
	m_SampleVec[n]->Remove(s, e);
}

void PoshSamplerPlugin::Copy(int n, long s, long e)
{
	if (m_SampleVec[n]->GetLength()==0) return;
	m_SampleVec[n]->GetRegion(m_CopyBuffer, s, e);
}

void PoshSamplerPlugin::Paste(int n, long s, long e)
{
	if (m_SampleVec[n]->GetLength()==0) return;
	m_SampleVec[n]->Insert(m_CopyBuffer, s);
}

void PoshSamplerPlugin::Mix(int n, long s, long e)
{
	if (m_SampleVec[n]->GetLength()==0) return;
	m_SampleVec[n]->Mix(m_CopyBuffer, s);
}

void PoshSamplerPlugin::Crop(int n, long s, long e)
{
	if (m_SampleVec[n]->GetLength()==0) return;
	m_SampleVec[n]->Remove(0, s);
	m_SampleVec[n]->Remove(e, m_SampleVec[n]->GetLength()-1);
}

void PoshSamplerPlugin::Reverse(int n, long s, long e)
{
	if (m_SampleVec[n]->GetLength()==0) return;
	m_SampleVec[n]->Reverse(s, e);
}

void PoshSamplerPlugin::Amp(int n, long s, long e)
{
	if (m_SampleVec[n]->GetLength()==0) return;
	for (int m=0; m<m_SampleVec[n]->GetLength(); m++)
	{
		m_SampleVec[n]->Set(m,(*m_SampleVec[n])[m]*m_SampleDescVec[n]->Volume);
	}
}


bool PoshSamplerPlugin::SaveExternalFiles(const string &Dir)
{
	for (int n=0; n<NUM_SAMPLES; n++)
	{
		char temp[256];
                // Andy's fix for bug 766594
                // sprintf (temp, "PoshSampler%d_%d.wav", SpiralPlugin_GetID(), n);
		sprintf (temp, "PoshSampler%d_%d.wav", GetID(), n);
		m_SampleDescVec[n]->Pathname   = temp;
	}

	for (int n=0; n<NUM_SAMPLES; n++)
	{
		// if it's not empty
		if (m_SampleVec[n]->GetLength()!=0)
		{
			SaveSample(n,Dir+m_SampleDescVec[n]->Pathname);
		}
	}
	return true;
}

void PoshSamplerPlugin::LoadExternalFiles(const string &Dir)
{
	for (int n=0; n<NUM_SAMPLES; n++)
	{
		char temp[256];
                // Andy's fix for bug 766594
		// sprintf (temp, "PoshSampler%d_%d.wav", SpiralPlugin_GetID(), n);
		sprintf (temp, "PoshSampler%d_%d.wav", GetID(), n);
		m_SampleDescVec[n]->Pathname   = temp;
	}

	for (int n=0; n<NUM_SAMPLES; n++)
	{
		LoadSample(n,Dir+m_SampleDescVec[n]->Pathname);
	}
}
