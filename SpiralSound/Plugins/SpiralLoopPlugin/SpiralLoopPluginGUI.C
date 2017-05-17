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

#include "SpiralLoopPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <FL/fl_file_chooser.h>

using namespace std;

////////////////////////////////////////////

SpiralLoopPluginGUI::SpiralLoopPluginGUI(int w, int h,SpiralLoopPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	m_LoopGUI = new Fl_Loop(0,15,w,h-15);
        m_LoopGUI->SetBGColour (Info->SCOPE_BG_COLOUR);
        m_LoopGUI->SetWaveColour (Info->SCOPE_FG_COLOUR);
        m_LoopGUI->SetSelColour (Info->SCOPE_SEL_COLOUR);
        m_LoopGUI->SetIndColour (Info->SCOPE_IND_COLOUR);
        m_LoopGUI->SetMrkColour (Info->SCOPE_MRK_COLOUR);

	m_Play = new Fl_LED_Button(255, h-35, 40, 25, "Play");
	//m_Play->color(GUIBG2_COLOUR);
    m_Play->type(1);
	m_Play->value(1);
    m_Play->down_box(FL_DOWN_BOX);
    m_Play->labelsize(10);
    m_Play->callback((Fl_Callback*)cb_Play);
	m_LoopGUI->add(m_Play);

	m_Volume = new Fl_Knob(95, 100, 45, 45, "Volume");
    m_Volume->color(Info->GUI_COLOUR);
	m_Volume->type(Fl_Knob::DOTLIN);
    m_Volume->labelsize(8);
    m_Volume->maximum(2);
    m_Volume->step(0.0001);
    m_Volume->value(1);
	m_Volume->callback((Fl_Callback*)cb_Volume);
	m_LoopGUI->add(m_Volume);

	m_Speed = new Fl_Knob(60, 150, 45, 45, "Speed");
    m_Speed->color(Info->GUI_COLOUR);
	m_Speed->type(Fl_Knob::DOTLIN);
    m_Speed->labelsize(8);
    m_Speed->maximum(2);
    m_Speed->step(0.0001);
    m_Speed->value(1);
	m_Speed->callback((Fl_Callback*)cb_Speed);
	m_LoopGUI->add(m_Speed);

	m_Length = new Fl_Knob(120, 160, 45, 45, "Length");
    m_Length->color(Info->GUI_COLOUR);
	m_Length->type(Fl_Knob::DOTLIN);
    m_Length->labelsize(8);
    m_Length->maximum(1);
    m_Length->step(0.0001);
    m_Length->value(1);
	m_Length->callback((Fl_Callback*)cb_Length);
	m_LoopGUI->add(m_Length);

	m_WavSize = new Fl_Knob(w-45, 15, 30, 30, "WaveSize");
    m_WavSize->color(Info->GUI_COLOUR);
	m_WavSize->type(Fl_Knob::DOTLIN);
    m_WavSize->labelsize(8);
    m_WavSize->maximum(3);
    m_WavSize->step(0.0001);
    m_WavSize->value(1);
	m_WavSize->callback((Fl_Callback*)cb_WavSize);
	m_LoopGUI->add(m_WavSize);

	m_Ticks = new Fl_Counter(5,h-30,30,14,"Ticks/Loop");
	m_Ticks->labelsize(8);
	m_Ticks->textsize(8);
	m_Ticks->step(1);
	m_Ticks->type(FL_SIMPLE_COUNTER);
	m_Ticks->value(64);
	m_Ticks->callback((Fl_Callback*)cb_Ticks);
	m_LoopGUI->add(m_Ticks);

	int X=w/2-30,Y=60,WIDTH=60,HEIGHT=12;
	int ycount=0;

	float phase=0.19f;
	float scale=70.0f;
	Fl_Boxtype   boxtype=FL_THIN_UP_BOX;

	m_Rec = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Record");
    m_Rec->type(1);
    m_Rec->box(boxtype);
    m_Rec->labelsize(8);
    m_Rec->labelcolor(FL_RED);
    m_Rec->callback((Fl_Callback*)cb_Rec);
	m_LoopGUI->add(m_Rec);
	ycount++;

	m_OverDub = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "OverDub");
    m_OverDub->type(1);
    m_OverDub->box(boxtype);
    m_OverDub->labelsize(8);
    m_OverDub->labelcolor(FL_RED);
    m_OverDub->callback((Fl_Callback*)cb_OverDub);
	m_LoopGUI->add(m_OverDub);
	ycount++;

	m_Hold = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Keep Dub");
    m_Hold->box(boxtype);
    m_Hold->labelsize(8);
    m_Hold->callback((Fl_Callback*)cb_Hold);
	m_LoopGUI->add(m_Hold);
	ycount++;

	m_Undo = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Undo Dub");
    m_Undo->box(boxtype);
    m_Undo->labelsize(8);
    m_Undo->callback((Fl_Callback*)cb_Undo);
	m_LoopGUI->add(m_Undo);
	ycount++;

	m_Load = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Load");
    m_Load->box(boxtype);
    m_Load->labelsize(8);
    m_Load->callback((Fl_Callback*)cb_Load);
	m_LoopGUI->add(m_Load);
	ycount++;

	m_Save = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Save");
    m_Save->box(boxtype);
    m_Save->labelsize(8);
    m_Save->callback((Fl_Callback*)cb_Save);
	m_LoopGUI->add(m_Save);
	ycount++;

	m_Trig = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "New trigger");
    m_Trig->box(boxtype);
    m_Trig->labelsize(8);
    m_Trig->callback((Fl_Callback*)cb_Trig);
	m_LoopGUI->add(m_Trig);
	ycount++;

	m_Cut = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Cut");
    m_Cut->box(boxtype);
    m_Cut->labelsize(8);
    m_Cut->callback((Fl_Callback*)cb_Cut);
	m_LoopGUI->add(m_Trig);
	ycount++;

	m_Copy = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Copy");
    m_Copy->box(boxtype);
    m_Copy->labelsize(8);
    m_Copy->callback((Fl_Callback*)cb_Copy);
	m_LoopGUI->add(m_Trig);
	ycount++;

	m_Paste = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Paste");
    m_Paste->box(boxtype);
    m_Paste->labelsize(8);
    m_Paste->callback((Fl_Callback*)cb_Paste);
	m_LoopGUI->add(m_Paste); 
	ycount++;

	m_PasteMix = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "PasteMix");
    m_PasteMix->box(boxtype);
    m_PasteMix->labelsize(8);
    m_PasteMix->callback((Fl_Callback*)cb_PasteMix);
	m_LoopGUI->add(m_PasteMix); 
	ycount++;

	m_ZeroR = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Zero Range");
    m_ZeroR->box(boxtype);
    m_ZeroR->labelsize(8);
    m_ZeroR->callback((Fl_Callback*)cb_ZeroR);
	m_LoopGUI->add(m_ZeroR); 
	ycount++;
	
	m_ReverseR = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Rev Range");
    m_ReverseR->box(boxtype);
    m_ReverseR->labelsize(8);
    m_ReverseR->callback((Fl_Callback*)cb_ReverseR);
	m_LoopGUI->add(m_ReverseR); 
	ycount++;
	
	m_SelectAll = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Select all");
    m_SelectAll->box(boxtype);
    m_SelectAll->labelsize(8);
    m_SelectAll->callback((Fl_Callback*)cb_SelectAll);
	m_LoopGUI->add(m_SelectAll); 
	ycount++;
	
	m_Double = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Double");
    m_Double->box(boxtype);
    m_Double->labelsize(8);
    m_Double->callback((Fl_Callback*)cb_Double);
	m_LoopGUI->add(m_Double); 
	ycount++;
	
	m_Half = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Half");
    m_Half->box(boxtype);
    m_Half->labelsize(8);
    m_Half->callback((Fl_Callback*)cb_Half);
	m_LoopGUI->add(m_Half); 
	ycount++;
	
	m_Crop = new Fl_Button(X+(int)(sin(ycount*phase)*scale), Y+ycount*HEIGHT, WIDTH, HEIGHT, "Crop");
    m_Crop->box(boxtype);
    m_Crop->labelsize(8);
    m_Crop->callback((Fl_Callback*)cb_Crop);
	m_LoopGUI->add(m_Crop); 
	ycount++;
}

void SpiralLoopPluginGUI::UpdateValues(SpiralPlugin *o)
{
	SpiralLoopPlugin *Plugin=(SpiralLoopPlugin *)o;
	m_SampleSize=Plugin->GetStoreBuffer()->GetLength();
	m_LoopGUI->SetData((float*)Plugin->GetStoreBuffer()->GetBuffer(),m_SampleSize);
	
	m_Volume->value(Plugin->GetVolume());
	m_Speed->value(Plugin->GetSpeed());
	if(m_SampleSize!=0) 
	{
		m_Length->value(Plugin->GetLoopLength()/m_SampleSize);
		m_LoopGUI->SetLength(Plugin->GetLoopLength());
	}
	else
	{
		m_LoopGUI->SetLength(0);
	}
	
	vector<SpiralLoopPlugin::TriggerInfo> *TrigVec=Plugin->GetTriggerVec();
	int ID=0;
	
	for (vector<SpiralLoopPlugin::TriggerInfo>::iterator i=TrigVec->begin();
		 i!=TrigVec->end(); i++)
	{
		Fl_Trigger *NewTrigger = new Fl_Trigger(parent()->x(), parent()->y(), 20, 20);
		NewTrigger->SetCentreX(150);
		NewTrigger->SetCentreY(150);
		NewTrigger->SetCentreRadius(125);
		if(m_SampleSize!=0) NewTrigger->SetAngle(i->Time*360.0f);
		NewTrigger->SetID(ID);
		NewTrigger->SetChannel(i->Channel);
		NewTrigger->callback((Fl_Callback*)cb_Trigger);
		m_LoopGUI->add(NewTrigger);
		m_TriggerVec.push_back(NewTrigger);
		NewTrigger->redraw();
		m_LoopGUI->redraw();
		ID++;
	}
}

void SpiralLoopPluginGUI::Update()
{
	m_LoopGUI->SetPos(m_GUICH->GetFloat("Pos"));
	m_LoopGUI->DrawPosMarker();
}

void SpiralLoopPluginGUI::UpdateSampleDisplay()
{
	m_GUICH->Wait();
	m_GUICH->SetCommand(SpiralLoopPlugin::GETSAMPLE);
	m_GUICH->Wait();
	m_GUICH->RequestChannelAndWait("SampleSize");
	m_SampleSize=m_GUICH->GetLong("SampleSize");

	if (m_SampleSize)
	{
		char *TempBuf = new char[m_SampleSize*sizeof(float)];
		m_GUICH->BulkTransfer("SampleBuffer",(void*)TempBuf,m_SampleSize*sizeof(float));
		m_LoopGUI->SetData((float*)TempBuf,m_SampleSize);
		delete[] TempBuf;
	}
	
	redraw();
}
	
inline void SpiralLoopPluginGUI::cb_Play_i(Fl_LED_Button* o, void* v)
{ 
	if (o->value()) m_GUICH->SetCommand(SpiralLoopPlugin::START);
	else m_GUICH->SetCommand(SpiralLoopPlugin::STOP);
}
void SpiralLoopPluginGUI::cb_Play(Fl_LED_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Play_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Volume_i(Fl_Knob* o, void* v)
{
	m_GUICH->Set("Volume",(float)o->value());
}
void SpiralLoopPluginGUI::cb_Volume(Fl_Knob* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Volume_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Speed_i(Fl_Knob* o, void* v)
{
	m_GUICH->Set("Speed",(float)o->value());
}
void SpiralLoopPluginGUI::cb_Speed(Fl_Knob* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Speed_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Length_i(Fl_Knob* o, void* v)
{
	m_GUICH->SetCommand(SpiralLoopPlugin::CHANGE_LENGTH);
	m_GUICH->Set("Length",(float)o->value());
	m_LoopGUI->SetLength((int)(m_SampleSize*o->value()));
	m_LoopGUI->redraw();
}
void SpiralLoopPluginGUI::cb_Length(Fl_Knob* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Length_i(o,v); }

inline void SpiralLoopPluginGUI::cb_WavSize_i(Fl_Knob* o, void* v)
{
	m_LoopGUI->SetWaveSize(o->value());
	redraw();
}
void SpiralLoopPluginGUI::cb_WavSize(Fl_Knob* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_WavSize_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Rec_i(Fl_Button* o, void* v)
{
	if (o->value()) m_GUICH->SetCommand(SpiralLoopPlugin::RECORD);
	else m_GUICH->SetCommand(SpiralLoopPlugin::ENDRECORD);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_Rec(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Rec_i(o,v); }

inline void SpiralLoopPluginGUI::cb_OverDub_i(Fl_Button* o, void* v)
{
	if (o->value()) m_GUICH->SetCommand(SpiralLoopPlugin::OVERDUB);
	else m_GUICH->SetCommand(SpiralLoopPlugin::ENDRECORD);
}
void SpiralLoopPluginGUI::cb_OverDub(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_OverDub_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Load_i(Fl_Button* o, void* v)
{
	char *fn=fl_file_chooser("Load a sample", "{*.wav,*.WAV}", NULL);
	
	if (fn && fn!='\0')
	{
		strcpy(m_TextBuf,fn);
		m_GUICH->SetData("Name",m_TextBuf);
		m_GUICH->SetCommand(SpiralLoopPlugin::LOAD);
		
		m_GUICH->Wait(); // wait for the sample to load
				
		UpdateSampleDisplay();
	} 
}
void SpiralLoopPluginGUI::cb_Load(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Load_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Save_i(Fl_Button* o, void* v)
{	
	char *fn=fl_file_chooser("Load a sample", "{*.wav,*.WAV}", NULL);
	
	if (fn && fn!='\0')
	{
		strcpy(m_TextBuf,fn);
		m_GUICH->SetData("Name",m_TextBuf);
		m_GUICH->SetCommand(SpiralLoopPlugin::SAVE);
	} 
}
void SpiralLoopPluginGUI::cb_Save(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Save_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Trig_i(Fl_Button* o, void* v)
{
	Fl_Trigger *NewTrigger = new Fl_Trigger(parent()->x(), parent()->y(), 20, 20);
	NewTrigger->SetCentreX(150);
	NewTrigger->SetCentreY(150);
	NewTrigger->SetCentreRadius(125);
	NewTrigger->SetID(m_TriggerVec.size());
	if (m_TriggerVec.size()<8) NewTrigger->SetChannel(m_TriggerVec.size());
	else NewTrigger->SetChannel(7);
	NewTrigger->callback((Fl_Callback*)cb_Trigger);
	m_LoopGUI->add(NewTrigger);
	m_TriggerVec.push_back(NewTrigger);
	NewTrigger->redraw();
	m_LoopGUI->redraw();
	
	m_GUICH->Set("Start",NewTrigger->GetID());
	m_GUICH->Set("End",NewTrigger->GetChannel());
	m_GUICH->Set("Length",0);
	m_GUICH->SetCommand(SpiralLoopPlugin::NEW_TRIGGER);
}
void SpiralLoopPluginGUI::cb_Trig(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Trig_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Cut_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",m_LoopGUI->GetRangeStart());
	m_GUICH->Set("End",m_LoopGUI->GetRangeEnd());
	m_GUICH->SetCommand(SpiralLoopPlugin::CUT);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_Cut(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Cut_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Copy_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",m_LoopGUI->GetRangeStart());
	m_GUICH->Set("End",m_LoopGUI->GetRangeEnd());
	m_GUICH->SetCommand(SpiralLoopPlugin::COPY);
}
void SpiralLoopPluginGUI::cb_Copy(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Copy_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Paste_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",m_LoopGUI->GetRangeStart());
	m_GUICH->SetCommand(SpiralLoopPlugin::PASTE);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_Paste(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Paste_i(o,v); }

inline void SpiralLoopPluginGUI::cb_PasteMix_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",m_LoopGUI->GetRangeStart());
	m_GUICH->SetCommand(SpiralLoopPlugin::PASTEMIX);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_PasteMix(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_PasteMix_i(o,v); }

inline void SpiralLoopPluginGUI::cb_ZeroR_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",m_LoopGUI->GetRangeStart());
	m_GUICH->Set("End",m_LoopGUI->GetRangeEnd());
	m_GUICH->SetCommand(SpiralLoopPlugin::ZERO_RANGE);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_ZeroR(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_ZeroR_i(o,v); }

inline void SpiralLoopPluginGUI::cb_ReverseR_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",m_LoopGUI->GetRangeStart());
	m_GUICH->Set("End",m_LoopGUI->GetRangeEnd());	
	m_GUICH->SetCommand(SpiralLoopPlugin::REVERSE_RANGE);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_ReverseR(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_ReverseR_i(o,v); }

inline void SpiralLoopPluginGUI::cb_SelectAll_i(Fl_Button* o, void* v)
{
	m_LoopGUI->SelectAll();
	//m_GUICH->SetCommand(SpiralLoopPlugin::SELECT_ALL);
}
void SpiralLoopPluginGUI::cb_SelectAll(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_SelectAll_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Double_i(Fl_Button* o, void* v)
{
	m_GUICH->SetCommand(SpiralLoopPlugin::DOUBLE);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_Double(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Double_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Half_i(Fl_Button* o, void* v)
{
	m_GUICH->SetCommand(SpiralLoopPlugin::HALF);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_Half(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Half_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Crop_i(Fl_Button* o, void* v)
{
	m_GUICH->SetCommand(SpiralLoopPlugin::CROP);
}
void SpiralLoopPluginGUI::cb_Crop(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Crop_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Hold_i(Fl_Button* o, void* v)
{
	m_GUICH->SetCommand(SpiralLoopPlugin::KEEPDUB);
	UpdateSampleDisplay();
}
void SpiralLoopPluginGUI::cb_Hold(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Hold_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Undo_i(Fl_Button* o, void* v)
{
	m_GUICH->SetCommand(SpiralLoopPlugin::UNDODUB);
}
void SpiralLoopPluginGUI::cb_Undo(Fl_Button* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Undo_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Trigger_i(Fl_Trigger* o, void* v)
{
	m_GUICH->Set("Start",o->GetID());
	m_GUICH->Set("End",o->GetChannel());
	m_GUICH->Set("Length",o->GetAngle()/360.0f);
	m_GUICH->SetCommand(SpiralLoopPlugin::UPDATE_TRIGGER);	
}
void SpiralLoopPluginGUI::cb_Trigger(Fl_Trigger* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Trigger_i(o,v); }

inline void SpiralLoopPluginGUI::cb_Ticks_i(Fl_Counter* o, void* v)
{
	if (o->value()<1) o->value(1); 
	m_GUICH->Set("TicksPerLoop",(int)o->value());	
}
void SpiralLoopPluginGUI::cb_Ticks(Fl_Counter* o, void* v)
{ ((SpiralLoopPluginGUI*)(o->parent()->parent()))->cb_Ticks_i(o,v); }

const string SpiralLoopPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "The SpiralLoop is designed for live jamming and the creation of freeform\n" 
	+ "  loops that you can sync to other loops, or other sequences within ssm.\n" 
	+ "This plugin is based on the old SpiralLoop design with a few improvements.\n" 
	+ "The main differece is the 'dub buffer' - a recording area that enables you to\n" 
	+ "audition recording before deciding whether to write them to the main loop\n" 
	+ "to undo them.\n" 
	+ "\n" 
	+ "Features:\n" 
	+ "Record      : Records a new loop, and deletes the old one\n" 
	+ "OverDub     : Records input into a the dub buffer\n" 
	+ "KeepDub     : Writes the dub buffer into the main loop\n" 
	+ "UndoDub     : Clears the dub buffer (erasing previous recordings)\n" 
	+ "Load        : Loads a wav into the loop\n" 
	+ "Save        : Saves the loop as a wav file\n" 
	+ "New Trigger : Create a new trigger (see below)\n" 
	+ "Cut         : Cut highlighted section of the loop\n" 
	+ "Copy        : Store highlighted section of the loop\n" 
	+ "Paste       : Insert copied/cut loop section\n" 
	+ "PasteMix    : Mix in copied/cut loop section\n" 
	+ "Zero Range  : Silence highlighted section\n" 
	+ "RevRange    : Reverse highlighted section\n" 
	+ "Select all  : Select the whole loop\n" 
	+ "Double      : Copy the loop to the end (doubling the size)\n" 
	+ "Half        : Chop the last half of the loop (halving the size)\n" 
	+ "Crop        : Lose the sampledata after the loop point (set with the length control)\n" 
	+ "\n" 
	+ "Ticks/Loop  : Sets the number of logic ticks per loop output via the clock port\n" 
	+ "              use this to sync matrix sequencers and the like.\n" 
	+ "\n" 	 
	+ "LoopTriggers\n"  
	+ "\n" 
	+ "Loop triggers are sync points that you can drag around the loop. When the play\n"
	+ "position crosses one, the corresponding output CV will go high. Multiple triggers\n"
	+ "can effect the same CV, use the right mouse button to cycle through the outputs";
}
