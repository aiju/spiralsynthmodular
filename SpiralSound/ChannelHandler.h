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

#ifndef CHANNEL_HANDLER
#define CHANNEL_HANDLER

#include <pthread.h>
#include <string>
#include <map>
#include <iostream>

class ChannelHandler
{
public:
    enum Type{INPUT,OUTPUT,OUTPUT_REQUEST};

    ChannelHandler();
    ~ChannelHandler();

    // only call these from the audio thread

	// don't call Register*() when the audio thread is running
    void     RegisterData(const std::string &ID, Type t, void *pData, int size);
    void     Register(const std::string &ID, bool* pData,  Type t=ChannelHandler::INPUT)    
		{ RegisterData(ID, t,(bool*)pData,sizeof(bool)); }
    void     Register(const std::string &ID, char* pData,  Type t=ChannelHandler::INPUT)    
		{ RegisterData(ID, t,(char*)pData,sizeof(char)); }
    void     Register(const std::string &ID, int* pData,   Type t=ChannelHandler::INPUT)    
		{ RegisterData(ID, t,(int*)pData,sizeof(int)); }
    void     Register(const std::string &ID, long* pData,   Type t=ChannelHandler::INPUT)    
		{ RegisterData(ID, t,(long*)pData,sizeof(long)); }
    void     Register(const std::string &ID, short* pData, Type t=ChannelHandler::INPUT)    
		{ RegisterData(ID, t,(short*)pData,sizeof(short)); }
    void     Register(const std::string &ID, float* pData, Type t=ChannelHandler::INPUT)    
		{ RegisterData(ID, t,(float*)pData,sizeof(float)); }
    void     Register(const std::string &ID, double* pData, Type t=ChannelHandler::INPUT)    
		{ RegisterData(ID, t,(double*)pData,sizeof(double)); }
    
	void     UpdateDataNow();
		
	bool     IsCommandWaiting() { return m_Command[0]; }
	char     GetCommand() { return m_Command[0]; }
	
	// use bulk transfers to copy variable and large sized data to the gui
	void	 SetupBulkTransfer(void *source) { m_BulkSrc=source; }
	
    // only call these from the gui thread
    // the ID comes from the order the channel is registered in
    void          GetData(const std::string &ID, void *data);
    const bool    GetBool(const std::string &ID)              { bool t; GetData(ID,&t); return t; }
    const char    GetChar(const std::string &ID)              { char t; GetData(ID,&t); return t; }
    const int     GetInt(const std::string &ID)               { int t; GetData(ID,&t); return t; }
    const long    GetLong(const std::string &ID)              { long t; GetData(ID,&t); return t; }
    const short   GetShort(const std::string &ID)             { short t; GetData(ID,&t); return t; }
    const float   GetFloat(const std::string &ID)             { float t; GetData(ID,&t); return t; }
    const double  GetDouble(const std::string &ID)            { double t; GetData(ID,&t); return t; }

    void          SetData(const std::string &ID, void *s);
    void          Set(const std::string &ID, const bool& s)     { SetData(ID,(void*)&s); }
    void          Set(const std::string &ID, const char& s)     { SetData(ID,(void*)&s); }
    void          Set(const std::string &ID, const int& s)      { SetData(ID,(void*)&s); }
    void          Set(const std::string &ID, const long& s)     { SetData(ID,(void*)&s); }
    void          Set(const std::string &ID, const short& s)    { SetData(ID,(void*)&s); }
    void          Set(const std::string &ID, const float& s)    { SetData(ID,(void*)&s); }
    void          Set(const std::string &ID, const double& s)   { SetData(ID,(void*)&s); }

	void          SetCommand(char command);

	// initialises the data from the audio side to the internal buffers
	void          FlushChannels();
	
	////////////////////////////////////////////
	// This is useful for getting data from the audio side immediately,
	// as it will block until complete, and for larger amounts of data, as
	// it won't be doing the copy every update.
		
	void 		  RequestChannelAndWait(const std::string &ID);
			
	///////////////////////////////////////////		
	// Use for copying large variable sized data very infrequently.
	// uses an OUTPUT_REQUEST channel as a transfer buffer.
	// Send the size needed to the gui down another channel.
	// This can be a slowww function, as it can take many audio updates
	// to complete, depending on the size of the buffer channel and the 
	// total size of the transfer.
	
	void 		  BulkTransfer(const std::string &ID, void *dest, int size);
	
	// delays for one or more updates. use for syncing commands
	void          Wait();
	
private:

    class Channel
    {
    public:
        Channel(Type t) { type=t; }

        Type type;
        void *data_buf;
        int   size;
        void *data;
		bool requested;
		bool updated;
    };

    std::map<std::string,Channel*> m_ChannelMap;
	char m_Command[2];
	
	bool m_UpdateIndicator;
	
	void *m_BulkSrc;
	
	int   m_BulkSize;
	int   m_BulkPos;
	std::string m_BulkID;
	
    pthread_mutex_t* m_Mutex;	
};

#endif
