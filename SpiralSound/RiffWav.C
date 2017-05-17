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

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "RiffWav.h"
//#include "SpiralInfo.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define TRACE_OUT

using namespace std;

#ifndef USE_LIBSNDFILE
const int HEADERLEN = (4+24+8);

#if __BYTE_ORDER == BIG_ENDIAN
#define SWAPSHORT(a) (a)=(((a)<<8)|(((a)>>8)&0xff))
#define SWAPINT(a) (a)=(((a)&0x000000ff)<<24)|(((a)&0x0000ff00)<<8)|(((a)&0x00ff0000)>>8)|(((a)&0xff000000)>>24)
#define SWAPFLOAT(a) swap_be_to_le_float(&a)
#else
#define SWAPSHORT(a)
#define SWAPINT(a)
#define SWAPFLOAT(a)
#endif

static void swap_be_to_le_float(float *a)
{
	float tmp;
	char *src, *dst;
	
 	src = (char*)a;
	dst = (char*)&tmp;
	dst[0] = src[3];
	dst[1] = src[2];
	dst[2] = src[1];
	dst[3] = src[0];
	*a = tmp;
}

static void write_float_as_16bit_short(float v, FILE *file)
{
	short value=(short)(v*SHRT_MAX);
	SWAPSHORT(value);
	fwrite(((char*) &value),2,1,file);
}

static void write_float_as_24bit_int(float v, FILE *file)
{
	char data[3];

	#if __BYTE_ORDER == BIG_ENDIAN
		data[0] = (int)(v*0x7FFFFF) >> 16;
		data[1] = ((int)(v*0x7FFFFF) >> 8);
		data[2] = ((int)(v*0x7FFFFF));
	#else
		data[0] = (int)(v*0x7FFFFF);
		data[1] = ((int)(v*0x7FFFFF) >> 8);
		data[2] = ((int)(v*0x7FFFFF) >> 16);
	#endif

	fwrite(data,3,1,file);
}

static void write_float_as_32bit_float(float v, FILE *file)
{
	float value=v;
	SWAPFLOAT(v);
	fwrite(((char*) &v),4,1,file);
}
#endif

int WavFile::Open(string FileName, Mode mode, Channels channels)
{
#ifdef USE_LIBSNDFILE
	if (m_FileHandle!=NULL)
	{
		cerr<<"WavFile: File already open ["<<FileName<<"]"<<endl;
		return 0;
	}
	
	if (mode==WRITE) {
		if (channels==STEREO)
			m_FileInfo.channels = 2;
		else
  			m_FileInfo.channels = 1;
  
		switch (m_BitsPerSample) {
			case 8 :
				m_FileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_S8; 
				break;

			case 16 :
				m_FileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; 
				break;

			case 24 :
				m_FileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24; 
				break;

			case 32 :
				m_FileInfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT; 
				break;
  
			default :
				m_FileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		}		
	} else
		m_FileInfo.format = 0;
	
	m_FileHandle = sf_open (FileName.c_str(), (mode==WRITE)?SFM_WRITE:SFM_READ, &m_FileInfo);

	if (mode==WRITE)//auto-update header on write so everything written before is valid even in case of unexpected crash/close
		sf_command (m_FileHandle, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_TRUE) ;

	if (m_FileHandle == NULL)
	{
		cerr<<"WavFile: File ["<<FileName<<"] does not exist"<<endl;
		return 0;
	}

	return 1;
#else
	if (m_Stream!=NULL)
	{
		cerr<<"WavFile: File already open ["<<FileName<<"]"<<endl;
		return 0;
	}

	if (mode==WRITE) m_Stream = fopen (FileName.c_str(), "wb");
	else m_Stream = fopen (FileName.c_str(), "rb");

	if (m_Stream == (FILE*)0)
	{
		cerr<<"WavFile: File ["<<FileName<<"] does not exist"<<endl;
		return 0;
	}

	//Write the header

	if (mode==WRITE)
	{
		m_Header.RiffName[0]='R';
		m_Header.RiffName[1]='I';
		m_Header.RiffName[2]='F';
		m_Header.RiffName[3]='F';

		m_Header.RiffFileLength= 4 + 24 + (8 + m_Header.FmtChannels*m_Header.FmtBitsPerSample/8 * 1 + 1); // bzzt - wrong "1" should be total number of samples

		m_Header.RiffTypeName[0]='W';
		m_Header.RiffTypeName[1]='A';
		m_Header.RiffTypeName[2]='V';
		m_Header.RiffTypeName[3]='E';

		m_Header.FmtName[0]='f';
		m_Header.FmtName[1]='m';
		m_Header.FmtName[2]='t';
		m_Header.FmtName[3]=' ';

		m_Header.FmtLength=0x00000010; // length of fmt data (16 bytes)
		
		if (WavFile::m_BitsPerSample==32)
			m_Header.FmtTag=0x0003;        // Format tag: 3 = IEEE FLOAT32
		else
			m_Header.FmtTag=0x0001;        // Format tag: 1 = PCM
			
		if (channels==STEREO) 
			m_Header.FmtChannels=2;
		else 
			m_Header.FmtChannels=1;
			
		m_Header.FmtSamplerate=WavFile::m_Samplerate;

		m_Header.FmtBitsPerSample=WavFile::m_BitsPerSample;//(WavFile::m_BitsPerSample==24)?32:WavFile::m_BitsPerSample;
		m_Header.FmtBlockAlign=m_Header.FmtChannels*m_Header.FmtBitsPerSample/8;
		m_Header.FmtBytesPerSec=m_Header.FmtSamplerate*m_Header.FmtBlockAlign;

		m_DataHeader.DataName[0]='d';
		m_DataHeader.DataName[1]='a';
		m_DataHeader.DataName[2]='t';
		m_DataHeader.DataName[3]='a';

		m_DataHeader.DataLengthBytes=0;

		SWAPINT(m_Header.RiffFileLength);
		SWAPINT(m_Header.FmtLength);
		SWAPSHORT(m_Header.FmtTag);
		SWAPSHORT(m_Header.FmtChannels);
		SWAPINT(m_Header.FmtSamplerate);
		SWAPINT(m_Header.FmtBytesPerSec);
		SWAPSHORT(m_Header.FmtBlockAlign);
		SWAPSHORT(m_Header.FmtBitsPerSample);
		SWAPINT(m_DataHeader.DataLengthBytes);

		fwrite(&m_Header,1,sizeof(CanonicalWavHeader),m_Stream);

		fwrite(&m_DataHeader,1,sizeof(DataHeader),m_Stream);

		return 1;
	}
	else
	if (mode==READ)
	{
		fread(&m_Header,sizeof(CanonicalWavHeader),1,m_Stream);

		SWAPINT(m_Header.RiffFileLength);
		SWAPINT(m_Header.FmtLength);
		SWAPSHORT(m_Header.FmtTag);
		SWAPSHORT(m_Header.FmtChannels);
		SWAPINT(m_Header.FmtSamplerate);
		SWAPINT(m_Header.FmtBytesPerSec);
		SWAPSHORT(m_Header.FmtBlockAlign);
		SWAPSHORT(m_Header.FmtBitsPerSample);

		WavFile::m_BitsPerSample=m_Header.FmtBitsPerSample;
		WavFile::m_Samplerate=m_Header.FmtSamplerate;

		#ifdef TRACE_OUT
		cerr<<FileName<<endl;
		cerr<<"RiffFileLength "<<m_Header.RiffFileLength<<endl;
		cerr<<"FmtLength "<<m_Header.FmtLength<<endl;
		cerr<<"FmtTag "<<m_Header.FmtTag<<endl;
		cerr<<"FmtChannels "<<m_Header.FmtChannels<<endl;
		cerr<<"FmtSamplerate "<<m_Header.FmtSamplerate<<endl;
		cerr<<"FmtBytesPerSec "<<m_Header.FmtBytesPerSec<<endl;
		cerr<<"FmtBlockAlign "<<m_Header.FmtBlockAlign<<endl;
		cerr<<"FmtBitsPerSample "<<m_Header.FmtBitsPerSample<<endl;
		#endif

		// skip the rest of the fmt header if necissary
		if (m_Header.FmtLength>16)
		{
			fseek(m_Stream,m_Header.FmtLength-16,SEEK_CUR);
		}

		fread(&m_DataHeader,sizeof(DataHeader),1,m_Stream);

		SWAPINT(m_DataHeader.DataLengthBytes);

		while (m_DataHeader.DataName[0]!='d' ||
			   m_DataHeader.DataName[1]!='a' ||
			   m_DataHeader.DataName[2]!='t' ||
			   m_DataHeader.DataName[3]!='a')
		{
			// crawl through the rest of the propriatory headers
			// if we need to to try and get to the data header
			if (feof(m_Stream) || fseek(m_Stream,-(sizeof(DataHeader)-1),SEEK_CUR)==-1)
			{
				cerr<<"WavFile: File open error, wrong format ["<<FileName<<"]"<<endl;
				return 0;
			}

			fread(&m_DataHeader,sizeof(DataHeader),1,m_Stream);
		}

		fgetpos(m_Stream,(fpos_t*)&m_DataStart);
		m_CurSeekPos=m_DataStart;

		#ifdef TRACE_OUT
		cerr<<m_DataHeader.DataName[0]<<m_DataHeader.DataName[1]<<
		      m_DataHeader.DataName[2]<<m_DataHeader.DataName[3]<<endl;
		cerr<<"DataLengthBytes "<<m_DataHeader.DataLengthBytes<<endl;
		#endif

		// check we have a wav file here
		if (m_Header.RiffName[0]=='R' &&
			m_Header.RiffName[1]=='I' &&
			m_Header.RiffName[2]=='F' &&
			m_Header.RiffName[3]=='F')
		{
			return 1;
		}

		fclose(m_Stream);
		cerr<<"WavFile: File open error, wrong format ["<<FileName<<"]"<<endl;
		return 0;
	}
	return 0;
#endif
}

int WavFile::Close()
{
#ifdef USE_LIBSNDFILE
	if (m_FileHandle==NULL)
	{
		return 0;
	}

	sf_close(m_FileHandle);

	m_FileHandle=NULL;
	return 1;
#else
	if (m_Stream==NULL)
	{
		return 0;
	}

	// write the total length in
	fseek(m_Stream, 40, SEEK_SET);
	fwrite(&m_DataHeader.DataLengthBytes,4,1,m_Stream);
	fclose(m_Stream);

	m_Stream=NULL;
	return 1;
#endif
}

int WavFile::Save(Sample &data)
{
#ifdef USE_LIBSNDFILE
	if (m_FileHandle==NULL || data.GetLength()==0)
	{
		return 0;
	}
	
	if (sf_writef_float(m_FileHandle, data.GetNonConstBuffer(), data.GetLength()) != data.GetLength())
	{
		cerr<<"WavFile: an error occured writing to the file"<<endl;
		return 0;
	}	

	m_FileInfo.frames += data.GetLength();
	return 1;
#else
	if (m_Stream==NULL || data.GetLength()==0)
	{
		return 0;
	}
	
	for (int n=0; n<data.GetLength(); n++)
	{
		float m=(float)data[n];
		if (m<-1) m=-1; if (m>1) m=1;

		switch (WavFile::m_BitsPerSample)
		{
			case 16:
				write_float_as_16bit_short(m, m_Stream);
			break;	

			case 24:
				write_float_as_24bit_int(m, m_Stream);
			break;	

			case 32:
				write_float_as_32bit_float(m, m_Stream);
			break;	

			default: return 0;
		}
	}
	
	m_DataHeader.DataLengthBytes+=data.GetLength()*(WavFile::m_BitsPerSample/8);

	return 1;
#endif
}

int WavFile::Save(short *data, int Bytes)
{
#ifdef USE_LIBSNDFILE
	if (m_FileHandle==NULL || data==NULL)
	{
		return 0;
	}
	
	sf_write_short(m_FileHandle, data, Bytes*m_FileInfo.channels/2);

	return 1;
#else
	if (m_Stream==NULL || data==NULL)
	{
		return 0;
	}
	
	m_DataHeader.DataLengthBytes+=Bytes;
	fwrite(data,sizeof(data),Bytes/4,m_Stream);
	return 1;
#endif
}

int WavFile::Save(float *left, float *right, int Length)
{
#ifdef USE_LIBSNDFILE
	if (m_FileHandle==NULL || left==NULL || right==NULL)
	{
		return 0;
	}

	if (IsStereo())// Interleave the channels into buffer and write
	{
		float *TempBuf = new float[Length*2];
		for (int n=0; n<Length; n++)
		{
			TempBuf[(n*2)] = left[n];
			TempBuf[(n*2)+1] = right[n];
		}
 
		sf_write_float(m_FileHandle, TempBuf, Length*2);
		delete[] TempBuf;
	}
	else // mix the channels into a mono buffer and then write
	{
		float *TempBuf = new float[Length];
		for (int n=0; n<Length; n++)
		{
			TempBuf[n] = left[n];
			TempBuf[n] += right[n];

			TempBuf[n]/=2;
		}
 
		sf_write_float(m_FileHandle, TempBuf, Length);
		delete[] TempBuf;
	}

	m_FileInfo.frames += Length;
	return 1;
#else
	if (m_Stream==NULL || left==NULL || right==NULL)
	{
		return 0;
	}

	for (int n=0; n<Length; n++)
	{
		float l=(float)left[n],r=(float)right[n];
		if (l<-1) l=-1; if (l>1) l=1;
		if (r<-1) r=-1; if (r>1) r=1;

		switch (WavFile::m_BitsPerSample)
		{
			case 16:
				write_float_as_16bit_short(l, m_Stream);
				write_float_as_16bit_short(r, m_Stream);
			break;	

			case 24:
				write_float_as_24bit_int(l, m_Stream);
				write_float_as_24bit_int(r, m_Stream);
			break;	

			case 32:
				write_float_as_32bit_float(l, m_Stream);
				write_float_as_32bit_float(r, m_Stream);
			break;	

			default: return 0;
		}
	}
	
	m_DataHeader.DataLengthBytes+=Length*2*(WavFile::m_BitsPerSample/8);

	return 1;
#endif
}

#ifndef USE_LIBSNDFILE
int WavFile::GetSize()
{	
        int Bps = m_Header.FmtBitsPerSample/8;
	if ((Bps<1) || (Bps>4))
	{
		cerr<<"WavFile Warning: FmtBitsPerSample="<<m_Header.FmtBitsPerSample<<" (can't cope, treating as 16)"<<endl;
		m_Header.FmtBitsPerSample=16;
		Bps=2;
	}
	
	int ret=0;
	
	if (IsStereo()) 
          ret=m_DataHeader.DataLengthBytes/(Bps*2);
	else 
          ret=m_DataHeader.DataLengthBytes/Bps;
	
	return ret;
}
#endif

int WavFile::Load(Sample &data)
{
#ifdef USE_LIBSNDFILE
	if (m_FileInfo.channels>1) // mix the channels into a mono buffer
	{
		#ifdef TRACE_OUT
		cerr<<"WavFile::Load - Channels = "<<m_FileInfo.channels<<
		" Mixing down to mono..."<<endl;
		#endif

		float *TempBuf = new float[GetSize()*m_FileInfo.channels];
		if (GetSize()*m_FileInfo.channels!= sf_read_float(m_FileHandle, TempBuf, GetSize()*m_FileInfo.channels))
		{
			cerr<<"WavFile: Read error"<<endl;
			return 0;
		}

		for (int n=0; n<GetSize(); n++)
		{
			float value=0;
			for (int i=0; i<m_FileInfo.channels; i++)
				value += TempBuf[(n*m_FileInfo.channels)+i];

			value/=m_FileInfo.channels;

			data.Set(n,value);
		}

		delete[] TempBuf;
	}
	else // it's mono.
	{
	    	float *TempBuf = new float[GetSize()];
		if (GetSize()!= sf_read_float(m_FileHandle, TempBuf, GetSize()))
		{
			cerr<<"WavFile: Read error"<<endl;
			return 0;
		}
		
		for (int n=0; n<GetSize(); n++)
			data.Set(n,TempBuf[n]);
		
		delete[] TempBuf;
	}
	
	return 1;
#else
	if (m_Header.FmtChannels>1) // mix the channels into a mono buffer
	{
		#ifdef TRACE_OUT
		cerr<<"WavFile::Load - Channels = "<<m_Header.FmtChannels<<
		" Mixing down to mono..."<<endl;
		#endif

		short *TempBuf = new short[m_DataHeader.DataLengthBytes];
		if (m_DataHeader.DataLengthBytes!=(int)fread(TempBuf,1,m_DataHeader.DataLengthBytes,m_Stream))
    	{
    	    cerr<<"WavFile: Read error"<<endl;
    		return 0;
    	}

		for (int n=0; n<GetSize(); n++)
		{
			long value=0;
			for (int i=0; i<m_Header.FmtChannels; i++)
			{
				short s = TempBuf[(n*m_Header.FmtChannels)+i];
				SWAPSHORT(s);
				value+=s;
			}
			value/=m_Header.FmtChannels;

			data.Set(n,value/(float)SHRT_MAX);
		}

		m_DataHeader.DataLengthBytes /= m_Header.FmtChannels;
		m_Header.FmtChannels=1;

		delete[] TempBuf;
	}
	else // it's mono.
	{
    	short *TempBuf = new short[GetSize()];
		int t=(int)fread(TempBuf,1,m_DataHeader.DataLengthBytes,m_Stream);
		if (m_DataHeader.DataLengthBytes!=t)
    	{
    	    cerr<<"WavFile: Read error read "<<t<<" expected "<<m_DataHeader.DataLengthBytes<<endl;
    		//return 0; 
    	}
		
		for (int n=0; n<GetSize(); n++)
		{
			short s = TempBuf[n];
			SWAPSHORT(s);
			data.Set(n,s/(float)SHRT_MAX);
		}
		
		delete[] TempBuf;
	}
	
	return 1;
#endif
}

int WavFile::Load(short *data)
{
#ifdef USE_LIBSNDFILE
	if (m_FileInfo.channels>1) // mix the channels into a mono buffer 
	{
		#ifdef TRACE_OUT		
		cerr<<"WavFile::Load - Channels = "<<m_FileInfo.channels<<
		" Mixing down to mono..."<<endl;
		#endif
		
		short *TempBuf = new short[GetSize()*m_FileInfo.channels];
		if (GetSize()*m_FileInfo.channels!= sf_read_short(m_FileHandle, TempBuf, GetSize()*m_FileInfo.channels))
		{
			cerr<<"WavFile: Read error"<<endl;
			return 0;
		}

		for (int n=0; n<GetSize(); n++)
		{
			float value=0;
			for (int i=0; i<m_FileInfo.channels; i++)
				value += TempBuf[(n*m_FileInfo.channels)+i];

			value/=m_FileInfo.channels;

			data[n] = (short)value/SHRT_MAX;
		}

		delete[] TempBuf;
	}
	else // we can read the data directly in, it's mono.
	{
		if (GetSize()!= sf_read_short(m_FileHandle, data, GetSize()))
		{
			cerr<<"WavFile: Read error"<<endl;
			return 0;
		}	
	}	

	return 0;
#else
	if (m_Header.FmtChannels>1) // mix the channels into a mono buffer 
	{
		#ifdef TRACE_OUT		
		cerr<<"WavFile::Load - Channels = "<<m_Header.FmtChannels<<
		" Mixing down to mono..."<<endl;
		#endif
		
		short *TempBuf = new short[m_DataHeader.DataLengthBytes];
		if (m_DataHeader.DataLengthBytes!=(int)fread(TempBuf,1,m_DataHeader.DataLengthBytes,m_Stream))
    	{
    	    cerr<<"WavFile: Read error"<<endl;
    		return 0; 
    	}
		
		for (int n=0; n<GetSize(); n++)
		{
			long value=0;
			for (int i=0; i<m_Header.FmtChannels; i++)
			{
				short s = TempBuf[(n*m_Header.FmtChannels)+i];
				SWAPSHORT(s);
				value+=s;
			}
			value/=m_Header.FmtChannels;
			
			data[n]=value;
		}
		
		m_DataHeader.DataLengthBytes /= m_Header.FmtChannels;
		m_Header.FmtChannels=1;
		
		delete[] TempBuf;
	}
	else // we can read the data directly in, it's mono.
	{
    	if (m_DataHeader.DataLengthBytes==
		(int)fread(data,1,m_DataHeader.DataLengthBytes,m_Stream))
    	{
#if __BYTE_ORDER == BIG_ENDIAN
			short *TempBuf = (short*)data;
			for (int n=0; n < m_DataHeader.DataLengthBytes / 2; n++)
				SWAPSHORT(TempBuf[n]);
#endif
    	    return 1;
    	}
	
    	cerr<<"WavFile: Read error"<<endl;
    	return 0;
	}
	
	return 0;
#endif
}

int WavFile::SeekToChunk(int Pos)
{
#ifdef USE_LIBSNDFILE
	if (m_CurSeekPos==Pos) return 0;
	m_CurSeekPos=Pos;

	if (sf_seek(m_FileHandle, m_CurSeekPos, SEEK_SET)==-1)
	{
           cerr<<"WavFile::SeekToChunk: Seek error"<<endl;
           return 0;
	}

	return 1;
#else
	Pos *= 2 * m_Header.FmtChannels;
	if (m_CurSeekPos==m_DataStart+Pos) return 1;

	m_CurSeekPos=m_DataStart+Pos;

	#ifdef TRACE_OUT		
	cerr<<"Seeking to "<<m_DataStart+Pos<<" "<<m_CurSeekPos<<endl;
	#endif
	if (fseek(m_Stream, m_DataStart+Pos, SEEK_SET)!=0)
	{
		cerr<<"WavFile::SeekToChunk: Seek error"<<endl;
	}

	return 1;
#endif
}

int WavFile::LoadChunk(int NumSamples, Sample &ldata, Sample &rdata)
{
#ifdef USE_LIBSNDFILE
	float *TempBuf = new float[NumSamples * m_FileInfo.channels];
	int ChunkSize = 0;
  	    
	ChunkSize = (int)sf_read_float(m_FileHandle, TempBuf, NumSamples*m_FileInfo.channels);
  	    
	if ((NumSamples*m_FileInfo.channels)!=ChunkSize)
	{
		cerr<<"WavFile: Only recieved "<<ChunkSize<<" of "<<NumSamples<<": Read chunk error"<<endl;
		delete[] TempBuf;
		return 0;
	} else {
		// Extract and scale samples to float range +/-1.0
		for (int n=0; n<NumSamples; n++)
		{
			ldata.Set(n,TempBuf[n*m_FileInfo.channels]);
  
			if (m_FileInfo.channels>1)
				rdata.Set(n,TempBuf[n*m_FileInfo.channels+1]);
		}
	}

	delete[] TempBuf;

	return 1;
#else
	int c = m_Header.FmtChannels;
	int SizeBytes = NumSamples * 2 * c; // 2 bytes per sample per channel
	short *TempBuf = new short[NumSamples * c];

	if (SizeBytes!=(int)fread(TempBuf,1,SizeBytes,m_Stream))
	{
		cerr<<"WavFile: Read chunk error"<<endl;
		return 0;
	} else {
		m_CurSeekPos+=SizeBytes;

	// Extract and scale samples to float range +/-1.0
		if (m_Header.FmtChannels==1) {
			for (int n=0; n<NumSamples; n++)
			{
				ldata.Set(n,TempBuf[n]/(float)SHRT_MAX);
				// rdata allocated and set to zero when stream opened
			}
		} else {
			for (int n=0; n<NumSamples; n++)
			{
			// Only copy first two channels
				ldata.Set(n,TempBuf[n*c]/(float)SHRT_MAX);
				rdata.Set(n,TempBuf[n*c+1]/(float)SHRT_MAX);
			}
		}
	}

	delete[] TempBuf;

	return 1;
#endif
}
