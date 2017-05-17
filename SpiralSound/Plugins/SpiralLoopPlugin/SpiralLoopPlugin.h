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

#ifndef SpiralLoopPLUGIN
#define SpiralLoopPLUGIN

static const int TRANSBUF_SIZE = 0x10000;
static const int MAX_TRIGGERS = 8;

struct SampleDesc
{
	std::string Pathname;
	float  Volume;
	float  Pitch;
	float  PitchMod;
	bool   Loop;
	int    Note;
	bool   TriggerUp;	
	float  SamplePos;
	int    SampleRate;
	bool   Stereo;
};

class SpiralLoopPlugin : public SpiralPlugin
{
public:
 	SpiralLoopPlugin();
	virtual ~SpiralLoopPlugin();
	
	struct TriggerInfo
	{
		int Channel;
		float Time;
		bool Triggered;
	};
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void ExecuteCommands();
	virtual void StreamOut(std::ostream &s);
	virtual void StreamIn(std::istream &s);
	virtual bool SaveExternalFiles(const std::string &Dir);
	virtual void LoadExternalFiles(const std::string &Dir);
	
	enum GUICommands{NONE,START,STOP,RECORD,OVERDUB,ENDRECORD,LOAD,SAVE,CUT,
					 COPY,PASTE,PASTEMIX,ZERO_RANGE,REVERSE_RANGE,SELECT_ALL,DOUBLE,HALF,
					 MOVE,CROP,KEEPDUB,UNDODUB,CHANGE_LENGTH,NEW_TRIGGER,UPDATE_TRIGGER,
					 GETSAMPLE};	
	
	void LoadWav(const char *Filename);
	void SaveWav(const char *Filename);
	bool GetOutput(Sample &data);
	void AllocateMem(int Length);
	void Clear();
	void MixDub()   { m_StoreBuffer.Mix(m_DubBuffer,0); ClearDub(); }
	void ClearDub() { m_DubBuffer.Zero(); }
	void Double();
	void MatchLength(int Len);
	void Crop();
	
	void SetId(int Id)       				{ m_Id=Id; }
	void SetPos(int Pos)     				{ m_Pos=Pos; }
	void SetLength(int Len)  				{ m_LoopPoint=Len; }
	void SetSpeed(float Speed)				{ m_Speed=Speed; }
	void SetVolume(float Vol)  				{ m_Volume=Vol; }
	void SetPlaying(bool Playing) 			{ m_Playing=Playing; }
	void SetRecordingSource(const float *s) { m_RecordingSource=s; }
	void Record(bool r) 					{ m_Recording=r; if (!r) EndRecordBuf(); }
	void DelMe()              				{ m_DelMe=true; }
	void Trigger()							{ m_Pos=0; SetPlaying(true); }
	
	const bool   IsPlaying() 				{return m_Playing;}
	const float *GetLoopPtr() 				{return m_StoreBuffer.GetBuffer();}
	const int    GetId() 					{return m_Id;}
	const int    GetLoopLength() 			{return m_LoopPoint;}
	float 		*GetPosPtr() 				{return &m_Pos;}
	const int    GetTotalLength() 			{assert(m_StoreBuffer.GetLength()==m_DubBuffer.GetLength()); return m_StoreBuffer.GetLength();}
	const bool   Delete()   				{return m_DelMe; }
	const float  GetSpeed() 				{return m_Speed;}
	const float  GetVolume()     			{ return m_Volume; }
	const float  GetCurrentAngle() 			{ return m_LoopPoint?(m_Pos/m_LoopPoint)*360.0f:0; }
	const std::string& GetSampleName()  			{ return m_Sample; }
	std::vector<TriggerInfo> *GetTriggerVec()    { return &m_TriggerVec; }
	
	void Cut(int Start, int End);
	void Copy(int Start, int End);
	void Paste(int Start);
	void PasteMix(int Start);
	
	void ZeroRange(int Start, int End);
	void ReverseRange(int Start, int End);
	void Halve();
	void SelectAll();
	void Move(int Start);

	Sample *GetStoreBuffer() { return &m_StoreBuffer; }
		
private:

	struct GUIArgs
	{
		long  Start;
		long  End;
		float Length;
		char  Name[256];
	};
	
	GUIArgs m_GUIArgs;
	
	void RecordBuf(float Pos, int Length);
	void EndRecordBuf();

	int    m_Id;
	float  m_Pos;
	int    m_IntPos;
	int    m_PlayBufPos;
	bool   m_Playing;
	bool   m_Recording;
	bool   m_DelMe;
	long int m_LoopPoint;
	float  m_Speed;
	float  m_Volume;
	const float *m_RecordingSource;
	Sample m_StoreBuffer;
	Sample m_DubBuffer;
	Sample m_RecBuffer;
	Sample m_CopyBuffer;
	int    m_RecPos;
	
	bool   m_FirstRecord;
	bool   m_FixedRecord;
	int    m_RecLength;
		
	int    m_TickTime;
	int    m_TickCurrent;
	int    m_TicksPerLoop;
	float  m_TickOutput;
	
	bool m_Triggered;
	
	char  m_SampleBuffer[TRANSBUF_SIZE];
	long  m_SampleSize;
	
	std::vector<TriggerInfo> m_TriggerVec;

	std::string m_Sample;
};

#endif
