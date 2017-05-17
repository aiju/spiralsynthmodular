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

#include "ChannelHandler.h"
#include <unistd.h>

using namespace std;

//#define CHANNEL_DEBUG

ChannelHandler::ChannelHandler() :
m_UpdateIndicator(false)
{
	m_Mutex = new pthread_mutex_t;
	m_Command[0]=0;
	m_Command[1]=0;
	m_BulkSrc=NULL;
	m_BulkSize=0;
	m_BulkPos=-1;
	pthread_mutex_init(m_Mutex,NULL);
}

ChannelHandler::~ChannelHandler()
{
    for(map<string,Channel*>::iterator i=m_ChannelMap.begin();
        i!=m_ChannelMap.end(); i++)
    {
		free(i->second->data_buf);
        delete i->second;
    }

    pthread_mutex_destroy(m_Mutex);
	delete m_Mutex;
}

///////////////////////////////////////////////////////////////

void ChannelHandler::UpdateDataNow()
{
	#ifdef CHANNEL_DEBUG
	cerr<<"Started update"<<endl;
	#endif
	// make sure the command is cleared even if 
	// we can't get a lock on the data
	m_Command[0]=0;

    if (pthread_mutex_trylock(m_Mutex))
    {
		#ifdef CHANNEL_DEBUG
		cerr<<"Got lock"<<endl;
		#endif
		
		m_UpdateIndicator=!m_UpdateIndicator;

        for(map<string, Channel*>::iterator i=m_ChannelMap.begin();
            i!=m_ChannelMap.end(); i++)
        {
			Channel *ch = i->second;

            switch (ch->type)
			{
				case INPUT :
            	{
					#ifdef CHANNEL_DEBUG
					cerr<<"memcpy input channel: ["<<i->first<<"]"<<endl;
					#endif
	               	memcpy(ch->data,ch->data_buf,ch->size);
            	} break;
				
				case OUTPUT :
			    {
					#ifdef CHANNEL_DEBUG
					cerr<<"memcpy output channel: ["<<i->first<<"]"<<endl;
					#endif
            	    memcpy(ch->data_buf,ch->data,ch->size);
            	} break;
				
				// one off request type
				case OUTPUT_REQUEST :
			    {
					if (m_BulkID==i->first && ch->requested)
					{
						if (m_BulkPos!=-1)
						{
							// doing a bulk transfer
							if (m_BulkPos+ch->size>m_BulkSize)
							{
								// last transfer
								#ifdef CHANNEL_DEBUG
								cerr<<"memcpy (last) bulk output channel: ["<<i->first<<"]"<<endl;
								cerr<<"pos:"<<m_BulkPos<<" size:"<<m_BulkSize<<" chsize:"<<ch->size<<endl;
								#endif
								memcpy(ch->data_buf,((char*)m_BulkSrc)+m_BulkPos,m_BulkSize-m_BulkPos);
								m_BulkPos=-1;
							}
							else
							{
								#ifdef CHANNEL_DEBUG
								cerr<<"memcpy bulk output channel: ["<<i->first<<"]"<<endl;
								cerr<<"pos:"<<m_BulkPos<<" size:"<<m_BulkSize<<endl;
								#endif
								memcpy(ch->data_buf,((char*)m_BulkSrc)+m_BulkPos,ch->size);
								m_BulkPos+=ch->size;
							}							
							ch->updated=true;
							ch->requested=false;
						}					
					}
					else
					{
						// normal request transfer
		           	    if (ch->requested)
						{					
							#ifdef CHANNEL_DEBUG
							cerr<<"memcpy output channel: ["<<i->first<<"]"<<endl;
							#endif
							memcpy(ch->data_buf,ch->data,ch->size);
							ch->updated=true;
							ch->requested=false;
						}
					}
            	} break;
			}
        }
		
		m_Command[0]=m_Command[1];
		// make sure the command only lasts one update
		m_Command[1]=0;
		
        pthread_mutex_unlock(m_Mutex);
			//cerr<<"audio out mutex"<<endl;
	
		//cerr<<"Update succeeded"<<endl;
    }
    else
    {
        //cerr<<"Couldn't get lock"<<endl;
    }	
	
	#ifdef CHANNEL_DEBUG
	cerr<<"Ended update"<<endl;
	#endif
}

void ChannelHandler::RegisterData(const string &ID, Type t,void* pData, int size)
{
    // probably don't need to lock here, as if get/set are called before
    // the channels have been set up they won't work anyway, but...
    //pthread_mutex_lock(m_Mutex);

	#ifdef CHANNEL_DEBUG
	cerr<<"Registering ["<<ID<<"] "<<hex<<pData<<dec<<" as "<<size<<" bytes big"<<endl;
	#endif

    map<string,Channel*>::iterator i=m_ChannelMap.find(ID);
    if (i!=m_ChannelMap.end())
    {
        cerr<<"Channel with ID ["<<ID<<"] already exists"<<endl;
    }

    Channel *NewCh=new Channel(t);
    NewCh->data_buf  = malloc(size);
    NewCh->size      = size;
    NewCh->data      = pData;
	NewCh->requested = false;
	NewCh->updated   = false;
    memcpy(NewCh->data_buf,NewCh->data,size);
    m_ChannelMap[ID]=NewCh;

    //pthread_mutex_unlock(m_Mutex);
}

/////////////////////////////////////////////////////////////////////////

void ChannelHandler::GetData(const string &ID, void *data)
{
    map<string,Channel*>::iterator i=m_ChannelMap.find(ID);
    if (i==m_ChannelMap.end())
    {
        cerr<<"ChannelHandler: Channel ["<<ID<<"] does not exist"<<endl;
        return;
    }

    if (!data)
    {
        cerr<<"ChannelHandler: Can't copy data to uninitialised mem"<<endl;
        return;
    }

    pthread_mutex_lock(m_Mutex);
    if (i->second->type==OUTPUT || i->second->type==OUTPUT_REQUEST)
    {
        memcpy(data,i->second->data_buf,i->second->size);
    }
    else
    {
        cerr<<"ChannelHandler: Tried to Get() data registered as input"<<endl;
    }//cerr<<"unlock 1"<<endl;
    pthread_mutex_unlock(m_Mutex);
}

void ChannelHandler::SetData(const string &ID, void *s)
{
    map<string,Channel*>::iterator i=m_ChannelMap.find(ID);
    if (i==m_ChannelMap.end())
    {
        cerr<<"ChannelHandler: Channel ["<<ID<<"] does not exist"<<endl;
        return;
    }

//cerr<<"lock 2"<<endl;
    pthread_mutex_lock(m_Mutex);
//cerr<<"lock 2 ok"<<endl;
    if (i->second->type==INPUT)
    {
        memcpy(i->second->data_buf,s,i->second->size);
    }
    else
    {
        cerr<<"ChannelHandler: Tried to Set() data registered as output"<<endl;
    }//cerr<<"unlock 2"<<endl;
    pthread_mutex_unlock(m_Mutex);
}

void ChannelHandler::SetCommand(char command)
{
	pthread_mutex_lock(m_Mutex);
	m_Command[1]=command;
	pthread_mutex_unlock(m_Mutex);	
}

void ChannelHandler::FlushChannels()
{
	pthread_mutex_lock(m_Mutex);
	
	for(map<string, Channel*>::iterator i=m_ChannelMap.begin();
       i!=m_ChannelMap.end(); i++)
    {
		memcpy(i->second->data_buf,i->second->data,i->second->size);
    }
	
	pthread_mutex_unlock(m_Mutex);	
}

void ChannelHandler::RequestChannelAndWait(const string &ID)
{
	map<string,Channel*>::iterator i=m_ChannelMap.find(ID);
    if (i==m_ChannelMap.end())
    {
        cerr<<"ChannelHandler: Channel ["<<ID<<"] does not exist"<<endl;
        return;
    }
	
	if (i->second->type!=OUTPUT_REQUEST)
    {
        cerr<<"ChannelHandler: Trying to request ["<<ID<<"] which is not a requestable channel"<<endl;
        return;
    }
	
	pthread_mutex_lock(m_Mutex);
	i->second->requested=true;
	pthread_mutex_unlock(m_Mutex);	
	
	bool ready=false;
	while (!ready)
	{
		usleep(10); // random amount of time :)
		pthread_mutex_lock(m_Mutex);
		ready=i->second->updated;
		pthread_mutex_unlock(m_Mutex);	
	}
	
	pthread_mutex_lock(m_Mutex);
	i->second->requested=false;
	i->second->updated=false;
	pthread_mutex_unlock(m_Mutex);	
}

void ChannelHandler::BulkTransfer(const string &ID, void *dest, int size)
{
	map<string,Channel*>::iterator i=m_ChannelMap.find(ID);
    if (i==m_ChannelMap.end())
    {
        cerr<<"ChannelHandler: Channel ["<<ID<<"] does not exist"<<endl;
        return;
    }
		
	if (i->second->type!=OUTPUT_REQUEST)
    {
        cerr<<"ChannelHandler: Trying to bulk transfer on ["<<ID<<"] which is not a OUTPUT_REQUEST channel"<<endl;
        return;
    }
	
	m_BulkPos=0;
	m_BulkSize = size;
	m_BulkID = ID;
	
	int pos=0;
	int buffersize=i->second->size;
	
	// fill up the destination buffer
	while (m_BulkPos!=-1)
	{
		RequestChannelAndWait(ID);
		if (pos+buffersize>size)
		{
			// last copy
			char *tempbuf = (char*)malloc(buffersize);
			GetData(ID,(void*)tempbuf);
			memcpy(((char*)dest)+pos,(void*)tempbuf,size-pos);
			free(tempbuf);
		}
		else
		{
			GetData(ID,((char*)dest)+pos);
		}		
		pos+=buffersize;
	}
}

void ChannelHandler::Wait()
{	
	bool current;
	bool last;
	
	for (int n=0; n<2; n++)
	{
		pthread_mutex_lock(m_Mutex);
		current=m_UpdateIndicator;
		last=m_UpdateIndicator;
		pthread_mutex_unlock(m_Mutex);
	
		while (current==last)
		{
			usleep(10);
			pthread_mutex_lock(m_Mutex);
			current=m_UpdateIndicator;
			pthread_mutex_unlock(m_Mutex);
		}	
	}
}
