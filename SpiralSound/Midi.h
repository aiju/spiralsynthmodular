/*  SpiralSynth
 *  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
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

#ifndef MIDI
#define MIDI

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <limits.h>
#include <queue>
#include <string>
#include "../config.h"

using namespace std;

#ifdef USE_ALSA_MIDI
#include <alsa/asoundlib.h>
#endif

#if __APPLE__
#include <CoreMIDI/MIDIServices.h>
#endif

class MidiEvent
{
public:
	enum type{NONE,ON,OFF,AFTERTOUCH,PARAMETER,CHANNELPRESSURE,PITCHBEND};

	MidiEvent() {m_Type=NONE;}
	MidiEvent(type t, int note, float v)
		{m_Type=t; m_Note=note; m_Volume=v;}

	type GetType() const {return m_Type;}
	float GetVolume() const {return m_Volume;}
	int GetNote() const {return m_Note;}
private:
	float m_Volume;
	type  m_Type;
	int   m_Note;
};

class MidiDevice
{
public:
	~MidiDevice();

	enum Type{READ,WRITE};

	static void Init(const string &name, Type t);
	static void SetDeviceName(string s) {
               #ifdef USE_OSS_MIDI
               m_DeviceName=s;
               #endif
        }
        static MidiDevice *Get()      { return m_Singleton; }
	static void PackUpAndGoHome() { if (m_Singleton) delete m_Singleton; m_Singleton=NULL; }

	MidiEvent GetEvent(int Device);
	void SendEvent(int Device,const MidiEvent &Event);

	void SetPoly(int s) { m_Poly=s; }

	float GetClock() { return m_Clock; }

private:
	MidiDevice(Type t);

	int  m_Poly;
	float m_Clock;
	int   m_ClockCount;

	queue<MidiEvent> m_EventVec[16];

	static MidiDevice *m_Singleton;

	pthread_t        m_MidiReader;
	pthread_mutex_t* m_Mutex;

	static string m_AppName;
#ifdef USE_ALSA_MIDI
	static void *MidiReaderCallback (void *o) { ((MidiDevice*)o)->AlsaCollectEvents(); return NULL; }
	void AlsaCollectEvents();
	void AlsaSendEvent(int Device, const MidiEvent &Event);

        void AlsaClose ();

	//snd_seq_t *seq_handle;

	//I appears that ALsa does not support both a read and write handle
	//so we must have two handle one for each mode
	snd_seq_t *seq_rhandle;
	snd_seq_t *seq_whandle;

	void AlsaOpen();
#endif
#ifdef USE_OSS_MIDI
        static void *MidiReaderCallback (void *o) { ((MidiDevice*)o)->OssCollectEvents(); return NULL; }
	void OssCollectEvents();
	void OssAddEvent(unsigned char* midi);
	void OssReadByte(unsigned char *c);
	void OssClose();
        bool OssOpen();
        static string m_DeviceName;
	int m_MidiFd, m_MidiWrFd;
#endif
#if __APPLE__
	MIDIClientRef					mMIDIClient;
	MIDIEndpointRef					mMIDISource;
	MIDIEndpointRef					mMIDIDestination;

	#define midi_ReadSize			4096
	unsigned char					m_ReadBuffer[midi_ReadSize];
	volatile int					m_ReadFillIndex;
	volatile int					m_ReadReadIndex;

	void MidiDevice::AppleOpen();
	void MidiDevice::AppleClose();
	int MidiDevice::AppleWrite(int dummy, unsigned char *outbuffer, int maxlen);
	int MidiDevice::AppleRead(int dummy, unsigned char *outbuffer, int maxlen);

	static void MidiDevice::sMIDIRead(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon);

#endif
};

#endif
