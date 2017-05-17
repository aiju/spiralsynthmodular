/*  SpiralLoops
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

#include <string.h>
#include "Sample.h"
#include <iostream>

Sample::Sample(int Len) :
m_IsEmpty(true),
m_DataGranularity(1),//512),
m_Data(NULL),
m_Length(0)
{
	if (Len) 
	{
		Allocate(Len);
	}
}


Sample::Sample(const Sample &rhs):
m_IsEmpty(true),
m_DataGranularity(512),
m_Data(NULL),
m_Length(0)
{
	*this=rhs;
}


Sample::Sample(const float *S, int Len):
m_IsEmpty(false),
m_DataGranularity(512),
m_Data(NULL),
m_Length(0)
{
	assert(S);
	Allocate(Len);		
	memcpy(m_Data,S,GetLengthInBytes());
}


Sample::~Sample()
{
	Clear();
}


	
bool Sample::Allocate(int Size)
{
	Clear();
	m_Data = new float[Size];
	m_Length=Size;
	
	memset(m_Data,0,GetLengthInBytes());
	
	return (m_Data);
}

void Sample::Clear()
{
	m_IsEmpty=true;

	if (m_Data)
	{
		delete[] m_Data;
		m_Length=0;
		m_Data=NULL;
	}
}

void Sample::Zero()
{
	m_IsEmpty=true;
	memset(m_Data,0,GetLengthInBytes());
}

void Sample::Set(float Val)
{
	m_IsEmpty=false;

	for (int n=0; n<m_Length; n++)
	{
		m_Data[n]=Val;
	}
}

void Sample::Insert(const Sample &S, int Pos)
{
	// do some checking	
	assert(Pos<=GetLength());

	int NewLen = GetLength()+S.GetLength();
	float *NewBuf = new float [NewLen];
	int FromPos=0, ToPos=0, TempBufPos=0;
	
	while (FromPos<=GetLength())
	{
		if (FromPos==Pos)
		{
			for (TempBufPos=0; TempBufPos<S.GetLength(); TempBufPos++)
			{
				NewBuf[ToPos]=S[TempBufPos];
				ToPos++;
			}
		}
		else
		{
			// this test is needed so the loop can deal 
			// with samples being "inserted" on to the 
			// very end of the buffer
			if (FromPos<GetLength()) 
			{
				NewBuf[ToPos]=m_Data[FromPos];
			}
		}
		FromPos++;
		ToPos++;
	}
	
	Clear();
	m_Data=NewBuf;
	m_Length=NewLen;
}

void Sample::Add(const Sample &S)
{
	Insert(S,GetLength());
}

void Sample::Mix(const Sample &S, int Pos)
{
	// do some checking	
	assert(Pos<GetLength());
	
	int ToPos=Pos;
	
	for (int FromPos=0; FromPos<S.GetLength(); FromPos++)
	{
		m_Data[ToPos]=m_Data[ToPos]+S[FromPos];
		
		if (ToPos>GetLength()) ToPos=0;
		ToPos++;
	}
}

void Sample::Remove(int Start, int End)
{
	// do some checking
	assert(End<GetLength() && Start<GetLength());
	assert(Start<=End);
	
	// check the range
	if (End>GetLength()) End=GetLength();
	if (Start<0) Start=0;
	
	// calc lengths and allocate memory
	int CutLen = End - Start;
	// has to be granulated by the buffer size
	CutLen-=CutLen % m_DataGranularity;
	
	int NewLen = GetLength()-CutLen;
	float *TempBuf = new float[NewLen];
	
	int ToPos=0;
	
	for (int FromPos=0; FromPos<GetLength(); FromPos++)
	{
		// copy the areas outside of the cut range
		if (FromPos<Start || FromPos>End)
		{
			TempBuf[ToPos]=m_Data[FromPos];
			ToPos++;			
			// check the position is in range of the calculated length
			assert(ToPos<=NewLen);
		}
	}
	
	Clear();
	m_Data=TempBuf;
	m_Length=NewLen;
}

void Sample::Reverse(int Start, int End)
{
	// do some checking
	assert(End<GetLength() && Start<GetLength());
	assert(Start<=End);
	
	// check the range
	if (End>GetLength()) End=GetLength();
	
	int NewLen = End-Start;
	float *TempBuf = new float[NewLen];
	int ToPos=0;
	int FromPos=0;
	
	// get the reversed sample
	for (FromPos=End; FromPos>Start; FromPos--)
	{
		TempBuf[ToPos]=m_Data[FromPos];
		ToPos++;
		assert(ToPos<=NewLen);
	}
	
	FromPos=0;
	
	// overwrite back into place
	for (ToPos=Start; ToPos<End; ToPos++)
	{
		m_Data[ToPos]=TempBuf[FromPos];
		FromPos++;
	}
	
}

void Sample::Move(int Dist)
{
	int Length=GetLength();
	float *TempBuf = new float[Length];
	int ToPos=0;
	int FromPos=Dist;
	
	if (FromPos<0) FromPos+=Length;
	if (FromPos>Length) FromPos-=Length;	
	
	// get the offset sample
	for (ToPos=0; ToPos<Length; ToPos++)
	{	
		TempBuf[ToPos]=m_Data[FromPos];
		FromPos++;
		if (FromPos>=Length) FromPos=0;
	}
	
	Clear();
	m_Data=TempBuf;
	m_Length=Length;	
}

void Sample::GetRegion(Sample &S, int Start, int End) const
{
	// do some checking
	assert(End<GetLength() && Start<GetLength());
	assert(Start<=End);
	
	int Length=End-Start;
	Length-=Length % m_DataGranularity;
	S.Allocate(Length);
	
	int FromPos=Start;
	
	for (int ToPos=0; ToPos<Length; ToPos++)
	{
		S.Set(ToPos,m_Data[FromPos]);
		FromPos++;
	}
}

void Sample::CropTo(int NewLength)
{
	assert (NewLength<GetLength());
	
	float *temp = new float[NewLength];
		
	for(int n=0; n<NewLength; n++)
	{
		temp[n]=m_Data[n];
	}
	
	Clear();
	m_Data=temp;	
	m_Length=NewLength;
}

// adds length amount of blank space
void Sample::Expand(int Length)
{
	Sample Temp(Length);
	Temp.Zero();
	
	Add(Temp);
}

// shrink the samle by length amount
void Sample::Shrink(int Length)
{
	int NewLength=GetLength()-Length;
	assert(NewLength>0 && NewLength<=GetLength());
	
	float *temp = new float[NewLength];
	
	for(int n=0; n<NewLength; n++)
	{
		temp[n]=m_Data[n];
	}
		
	Clear();
	m_Data=temp;	
	m_Length=NewLength;
}
