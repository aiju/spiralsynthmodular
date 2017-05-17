/*  SpiralPlugin
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

#include "DiskWriterPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_file_chooser.H>

using namespace std;

DiskWriterPluginGUI::DiskWriterPluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
        m_16bits = new Fl_LED_Button (0, 15, 23, 23, "16bit");
        m_16bits->type (FL_RADIO_BUTTON);
        m_16bits->labelsize(10);
	m_16bits->set();
        m_16bits->callback((Fl_Callback*)cb_16bits, this);

        m_24bits = new Fl_LED_Button (0, 38, 23, 23, "24bit");
        m_24bits->type (FL_RADIO_BUTTON);
        m_24bits->labelsize(10);
        m_24bits->callback((Fl_Callback*)cb_24bits, this);

        m_32bits = new Fl_LED_Button (0, 61, 23, 23, "32bit");
        m_32bits->type (FL_RADIO_BUTTON);
        m_32bits->labelsize(10);
        m_32bits->callback((Fl_Callback*)cb_32bits, this);

        // 7 seg displays
        for (int dis=0; dis<4; dis++) {
            m_Display[dis] = new Fl_SevenSeg (50 + 27*dis, 20, 27, 38);
            m_Display[dis] -> bar_width (4);
            m_Display[dis] -> color (Info->SCOPE_FG_COLOUR);
	    m_Display[dis] -> color2 (Info->SCOPE_BG_COLOUR);
            if (dis > 0 && dis % 2 == 0) m_Display[dis] -> dp (colon);
            add (m_Display[dis]);
        }

        m_Stereo = new Fl_Check_Button (105, 63, 10, 18, "Stereo");
        m_Stereo->type (1);
        m_Stereo->value (1);
        m_Stereo->labelsize(12);
        m_Stereo->callback((Fl_Callback*)cb_Stereo, this);

        Open = new Fl_Button(0, 85, 75, 20, "Open");
        Open->type(1);
        Open->box (FL_PLASTIC_UP_BOX);
        Open->color (Info->GUI_COLOUR);
        Open->selection_color (Info->GUI_COLOUR);
        Open->labelsize(10);
        Open->callback((Fl_Callback*)cb_Open, this);

	Record = new Fl_Button(85, 85, 75, 20, "Record");
        Record->type(1);
        Record->box (FL_PLASTIC_UP_BOX);
        Record->color (Info->GUI_COLOUR);
        Record->selection_color (Info->GUI_COLOUR);
        Record->labelsize(10);
        Record->callback((Fl_Callback*)cb_Record, this);
	       
	end();
}

void DiskWriterPluginGUI::Update()
{
     float t=m_GUICH->GetFloat ("TimeRecorded");
     bool recording=m_GUICH->GetBool ("Recording");

     if (recording)
     {
	m_16bits->deactivate();
	m_24bits->deactivate();
	m_32bits->deactivate();
	m_Stereo->deactivate();
     }
     else
     {
	m_16bits->activate();
	m_24bits->activate();
	m_32bits->activate();
	m_Stereo->activate();
     }
     
     m_Display[3]->value ((int)t % 10);
     m_Display[2]->value ((int)(t/10) % 6);
     m_Display[1]->value ((int)(t/60) % 10);
     m_Display[0]->value ((int)(t/600) % 10);

     redraw();
}

void DiskWriterPluginGUI::UpdateValues (SpiralPlugin *o) 
{
	DiskWriterPlugin* Plugin = (DiskWriterPlugin*)o;

	switch (Plugin->GetBitsPerSample())
	{
		case 32 :
		{
			m_32bits->value(1);
			m_24bits->value(0);
			m_16bits->value(0);
		}	
		break;	

		case 24 :
		{
			m_32bits->value(0);
			m_24bits->value(1);
			m_16bits->value(0);
		}	
		break;
		
		case 16 :
		default:
		{
			m_32bits->value(0);
			m_24bits->value(0);
			m_16bits->value(1);
		}	
	}
	
	m_Stereo->value(Plugin->GetStereo());

	redraw();
}

//// Callbacks ////

inline void DiskWriterPluginGUI::cb_Open_i(Fl_Button* o)
{	
	if (o->value())
	{
		char *fn=fl_file_chooser("Pick a Wav file to save to", "*.wav", NULL);
		char t[256];
		sprintf(t,"%s",fn);
		
		if (fn && fn!="")
		{
			m_GUICH->SetData("Filename",(void*)t);
			m_GUICH->SetCommand(DiskWriterPlugin::OPENWAV);
		}
		else
		{
			m_GUICH->SetCommand(DiskWriterPlugin::CLOSEWAV);
			o->value(false);
		}
	}
	else
	{
		m_GUICH->SetCommand(DiskWriterPlugin::CLOSEWAV);
	}
}

inline void DiskWriterPluginGUI::cb_Record_i(Fl_Button* o)
{ 
	if (o->value()) 
	{
		m_GUICH->SetCommand(DiskWriterPlugin::RECORD);
	}	
	else 
	{
		m_GUICH->SetCommand(DiskWriterPlugin::STOP);
	}	
}

inline void DiskWriterPluginGUI::cb_16bits_i(Fl_Button* o)
{	
	m_GUICH->Set("BitsPerSample",16);
}

inline void DiskWriterPluginGUI::cb_24bits_i(Fl_Button* o)
{	
	m_GUICH->Set("BitsPerSample",24);
}

inline void DiskWriterPluginGUI::cb_32bits_i(Fl_Button* o)
{	
	m_GUICH->Set("BitsPerSample",32);
}

inline void DiskWriterPluginGUI::cb_Stereo_i(Fl_Button* o)
{	
	m_GUICH->Set("Stereo",o->value());
}

const string DiskWriterPluginGUI::GetHelpText(const string &loc)
{
	return string("")
	+ "One way of recording your creations to disk. First open a file\n"
	+ "you wish to save to, then hit record to start recording.\n"
	+ "You are able to stop and restart recording without closing the\n"
	+ "file, which should make life a little easier if you are doing\n"
	+ "things like recording lots of little samples.";
}
