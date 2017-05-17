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

#include "PoshSamplerPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <FL/fl_file_chooser.h>

using namespace std;

////////////////////////////////////////////

Fl_WaveDisplay::Fl_WaveDisplay(int x,int y,int w,int h, char *Name) :
Fl_Widget(x,y,w,h,Name),
m_Sample(NULL),
m_StartPos(1),
m_EndPos(10),
m_ViewStart(0),
m_ViewEnd(INT_MAX),
m_PlayPos(0),
m_PlayStart(0),
m_LoopStart(0),
m_LoopEnd(INT_MAX),
m_PosMarker(true)
{
}

Fl_WaveDisplay::~Fl_WaveDisplay()
{
}

void Fl_WaveDisplay::SetSample(const float* s, long len)
{
	if (m_Sample) delete m_Sample;
	m_Sample = new Sample(s,len);
}

void Fl_WaveDisplay::draw()
{
	int ho=h()/2;
	fl_color (m_BGColour);
	fl_rectf(x(), y(), w(), h());
	if (!m_Sample || m_Sample->GetLength()==0) return;

	if (m_ViewStart<0) m_ViewStart=0;
	if (m_ViewEnd>m_Sample->GetLength()-1) m_ViewEnd=m_Sample->GetLength()-1;

	if (m_PlayStart<0) m_PlayStart=0;
	if (m_PlayStart>m_Sample->GetLength()-1) m_PlayStart=m_Sample->GetLength()-1;
	if (m_LoopStart<0) m_LoopStart=0;
	if (m_LoopStart>m_Sample->GetLength()-1) m_LoopStart=m_Sample->GetLength()-1;
	if (m_LoopEnd<0) m_LoopEnd=0;
	if (m_LoopEnd>m_Sample->GetLength()-1) m_LoopEnd=m_Sample->GetLength()-1;

	float Value=0,NextValue=0;
	int pos=0;
	int Jump=(m_ViewEnd-m_ViewStart)/w();
	if (Jump==0) Jump=1;

	for(int n=m_ViewStart; n<m_ViewEnd-Jump; n+=Jump)
	{
		fl_font(fl_font(),10);

		if (m_PlayPos>=n && m_PlayPos<n+Jump)
		{
			fl_color (m_IndColour);
			fl_line(x()+pos, y(),
				    x()+pos, y()+h());
		}

		if (m_PlayStart>=n && m_PlayStart<n+Jump)
		{
			fl_color (m_MrkColour);
			fl_draw("S",x()+pos+2,y()+h());
			fl_line(x()+pos, y(),
				    x()+pos, y()+h());
		}

		if (m_LoopStart>=n && m_LoopStart<n+Jump)
		{
			fl_color (m_MrkColour);
			fl_draw("LS",x()+pos+2,y()+h());
			fl_line(x()+pos, y(),
				    x()+pos, y()+h());
		}

		if (m_LoopEnd>=n && m_LoopEnd<n+Jump)
		{
			fl_color (m_MrkColour);
			fl_draw("LE",x()+pos+2,y()+h());
			fl_line(x()+pos, y(),
				    x()+pos, y()+h());
		}

		if (n>m_StartPos && n<m_EndPos) fl_color (m_SelColour);
		else fl_color (m_FGColour);

		Value = NextValue;

		// get max
		float max=(*m_Sample)[n];
		float min=(*m_Sample)[n];
		for (int m=n; m<n+Jump; m++)
		{
			if (max<(*m_Sample)[m]) max=(*m_Sample)[m];
			if (min>(*m_Sample)[m]) min=(*m_Sample)[m];
		}

		min*=ho; max*=ho;

		fl_line(x()+pos-1, y()+ho-(int)min,
				x()+pos-1, y()+ho-(int)max);
		pos++;
	}
}

int Fl_WaveDisplay::handle(int event)
{
	int xx=Fl::event_x();
	int yy=Fl::event_y();
	static int DragX,DragY;
	static int Mousebutton=0;
	static int Holding=0;
	static int GrabDist=10;

	if (!m_Sample || m_Sample->GetLength()==0) return 1;

	if (event==FL_PUSH)
	{
		GrabDist=(int)((m_ViewEnd-m_ViewStart)*0.03f);
		Mousebutton=Fl::event_button();
		DragX=xx;
		DragY=yy;

		if (Mousebutton==1)
		{
			int MousePos=(xx-x())*((m_ViewEnd-m_ViewStart)/w())+m_ViewStart;
			Holding=0;
			if (abs(MousePos-m_StartPos)<GrabDist)    Holding=1;
			else if (abs(MousePos-m_EndPos)<GrabDist) Holding=2;
			else if (abs(MousePos-m_PlayStart)<GrabDist) Holding=3;
			else if (abs(MousePos-m_LoopStart)<GrabDist) Holding=4;
			else if (abs(MousePos-m_LoopEnd)<GrabDist) Holding=5;
			else
			{
				m_StartPos=MousePos;
				m_EndPos=MousePos;
			}
		}
	}
	
	if (event==FL_DRAG)
	{
		if (Mousebutton==1)
		{	
			int MousePos=(xx-x())*((m_ViewEnd-m_ViewStart)/w())+m_ViewStart;				

			switch (Holding)
			{
				case 0:
				{
					if (MousePos>m_EndPos) m_EndPos=MousePos;
					else m_StartPos=MousePos;					
				} break;
					
				case 1:
				{
					m_StartPos=MousePos;
					if (m_StartPos>m_EndPos) Holding=2; // swap
				} break;
				
			
				case 2:
				{
					m_EndPos=MousePos;
					if (m_StartPos>m_EndPos) Holding=1; // swap
				} break;
			
				case 3: m_PlayStart=MousePos; break;
				case 4: m_LoopStart=MousePos; break;
				case 5: m_LoopEnd=MousePos; break;
			}
		}
		
		if (Mousebutton==2)
		{
			int Dist=(DragX-xx)*((m_ViewEnd-m_ViewStart)/w());
			if (m_ViewStart>0 && m_ViewEnd<m_Sample->GetLength()-1)
			{			
				m_ViewStart+=Dist; 				
				m_ViewEnd+=Dist;
			}
			else // stop it sticking when at end/beginning
			{
				if ((Dist>0 && m_ViewStart<=0) ||
				    (Dist<0 && m_ViewEnd>=m_Sample->GetLength()-1)) 
				{
					m_ViewStart+=Dist; 				
					m_ViewEnd+=Dist;
				}
			}
			DragX=xx;
			DragY=yy;
		}	
		
		if (Mousebutton==3)
		{
			// only draw wave at 1 pixel = 1 sample
			if ((m_ViewEnd-m_ViewStart)/w()==1)
			{
				int MousePos=(xx-x())*((m_ViewEnd-m_ViewStart)/w())+m_ViewStart;				
				float Value=-(yy-y())/((float)h()/2.0f)+1.0f;			
				m_Sample->Set(MousePos,Value);
				redraw();
			}
		}
		
		do_callback();
		redraw();		
	}
	
	if (m_EndPos>=m_Sample->GetLength()) m_EndPos=m_Sample->GetLength()-1;
	
	return 1;
}

void Fl_WaveDisplay::ZoomIn()
{
	int Zoom=(int)((m_ViewEnd-m_ViewStart)*0.03f);
	if ((m_ViewEnd-m_ViewStart)/w()>1)
	{
		m_ViewStart+=Zoom; 
		m_ViewEnd-=Zoom;
	}
	
	redraw();
}

void Fl_WaveDisplay::ZoomOut()
{
	int Zoom=(int)((m_ViewEnd-m_ViewStart)*0.03f);
	m_ViewStart-=Zoom; 
	m_ViewEnd+=Zoom;
	redraw();
}

////////////////////////////////////////////

PoshSamplerPluginGUI::PoshSamplerPluginGUI(int w, int h,PoshSamplerPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
m_UpdateMe(false)
{	
	int n=0; 

	m_Load = new Fl_Button(5, 20, 70, 20, "Load");
        m_Load->labelsize(10);
        m_Load->box (FL_PLASTIC_UP_BOX);
	m_Load->color (Info->GUI_COLOUR);
	m_Load->selection_color (Info->GUI_COLOUR);
	m_Load->callback((Fl_Callback*)cb_Load);
	add(m_Load);

	m_Save = new Fl_Button(5, 40, 70, 20, "Save");
        m_Save->labelsize(10);
        m_Save->box (FL_PLASTIC_UP_BOX);
	m_Save->color (Info->GUI_COLOUR);
	m_Save->selection_color (Info->GUI_COLOUR);
	m_Save->callback((Fl_Callback*)cb_Save);
	add(m_Save);

	m_Record = new Fl_Button(5, 60, 70, 20, "Record");
        m_Record->type (FL_TOGGLE_BUTTON);
        m_Record->box (FL_PLASTIC_UP_BOX);
	m_Record->color (FL_RED);
	m_Record->selection_color (FL_RED);
        m_Record->labelsize (10);
        //m_Record->labelcolor (FL_RED);
	m_Record->callback((Fl_Callback*)cb_Record);
	add(m_Record);

	m_Loop = new Fl_Button(80, 20, 70, 20, "Loop");
        m_Loop->type (FL_TOGGLE_BUTTON);
        m_Loop->labelsize(10);
        m_Loop->box (FL_PLASTIC_UP_BOX);
	m_Loop->color (Info->GUI_COLOUR);
	m_Loop->selection_color (Info->GUI_COLOUR);
	m_Loop->callback((Fl_Callback*)cb_Loop);
	add(m_Loop);

	m_PingPong = new Fl_Button(80, 40, 70, 20, "PingPong");
        m_PingPong->labelsize(10);
	m_PingPong->type (FL_TOGGLE_BUTTON);
        m_PingPong->labelsize(10);
        m_PingPong->box (FL_PLASTIC_UP_BOX);
	m_PingPong->color (Info->GUI_COLOUR);
	m_PingPong->selection_color (Info->GUI_COLOUR);
	m_PingPong->callback((Fl_Callback*)cb_PingPong);
	add(m_PingPong);

	m_PosMarker = new Fl_Button(80, 60, 70, 20, "PosMarker");
        m_PosMarker->labelsize(10);
	m_PosMarker->type (FL_TOGGLE_BUTTON);
        m_PosMarker->labelsize(10);
        m_PosMarker->box (FL_PLASTIC_UP_BOX);
	m_PosMarker->color (Info->GUI_COLOUR);
	m_PosMarker->selection_color (Info->GUI_COLOUR);
	m_PosMarker->value(1);
	m_PosMarker->callback((Fl_Callback*)cb_PosMarker);
	add(m_PosMarker);

	m_Volume = new Fl_Knob(160, 20, 50, 50, "Volume");
        m_Volume->color(Info->GUI_COLOUR);
	m_Volume->type(Fl_Knob::LINELIN);
        m_Volume->labelsize(10);
        m_Volume->maximum(2);
        m_Volume->step(0.001);
        m_Volume->value(1);
	m_Volume->callback((Fl_Callback*)cb_Volume);
	add(m_Volume);

	m_Pitch = new Fl_Knob(220, 20, 50, 50, "Pitch");
        m_Pitch->color(Info->GUI_COLOUR);
	m_Pitch->type(Fl_Knob::LINELIN);
        m_Pitch->labelsize(10);
        m_Pitch->maximum(10);
        m_Pitch->step(0.001);
        m_Pitch->value(1);
	m_Pitch->callback((Fl_Callback*)cb_Pitch);
	add(m_Pitch);

	m_Octave = new Fl_Knob(280, 20, 50, 50, "Octave");
        m_Octave->color(Info->GUI_COLOUR);
	m_Octave->type(Fl_Knob::LINELIN);
        m_Octave->labelsize(10);
        m_Octave->maximum(12);
        m_Octave->step(1);
        m_Octave->value(6);
	m_Octave->callback((Fl_Callback*)cb_Octave);
	add(m_Octave);

	m_SampleNum = new Fl_Counter (w-60, 15, 45, 20, "Sample");
        m_SampleNum->labelsize(10);
	m_SampleNum->type(FL_SIMPLE_COUNTER);
        m_SampleNum->box (FL_PLASTIC_UP_BOX);
	m_SampleNum->color (Info->GUI_COLOUR);
	m_SampleNum->selection_color (Info->GUI_COLOUR);
	m_SampleNum->step(1);
	m_SampleNum->value(n);
	m_SampleNum->callback((Fl_Callback*)cb_SampleNum);
	add(m_SampleNum);

	m_Note = new Fl_Counter (w-60, 50, 45, 20, "Trig Note");
        m_Note->labelsize(10);
	m_Note->type(FL_SIMPLE_COUNTER);
        m_Note->box (FL_PLASTIC_UP_BOX);
	m_Note->color (Info->GUI_COLOUR);
	m_Note->selection_color (Info->GUI_COLOUR);
	m_Note->step(1);
	m_Note->value(n);
	m_Note->callback((Fl_Callback*)cb_Note);
	add(m_Note);

	m_Display = new Fl_WaveDisplay(5,85,w-10,100,"");
        m_Display->SetColours (Info->SCOPE_BG_COLOUR, Info->SCOPE_FG_COLOUR,
                               Info->SCOPE_SEL_COLOUR, Info->SCOPE_IND_COLOUR, Info->SCOPE_MRK_COLOUR);
	m_Display->callback((Fl_Callback*)cb_WaveDisplay);

	int bx=5,by=190,bw=w/9-2,bh=20,bs=w/9-2;
	n=0;

	m_Cut = new Fl_Button(bx+(n++*bs),by,bw,bh,"Cut");
	m_Cut->labelsize(10);
        m_Cut->box (FL_PLASTIC_UP_BOX);
	m_Cut->color (Info->GUI_COLOUR);
	m_Cut->selection_color (Info->GUI_COLOUR);
	m_Cut->callback((Fl_Callback*)cb_Cut);

	m_Copy = new Fl_Button(bx+(n++*bs),by,bw,bh,"Copy");
	m_Copy->labelsize(10);
        m_Copy->box (FL_PLASTIC_UP_BOX);
	m_Copy->color (Info->GUI_COLOUR);
	m_Copy->selection_color (Info->GUI_COLOUR);
	m_Copy->callback((Fl_Callback*)cb_Copy);

	m_Paste = new Fl_Button(bx+(n++*bs),by,bw,bh,"Paste");
	m_Paste->labelsize(10);
        m_Paste->box (FL_PLASTIC_UP_BOX);
	m_Paste->color (Info->GUI_COLOUR);
	m_Paste->selection_color (Info->GUI_COLOUR);
	m_Paste->callback((Fl_Callback*)cb_Paste);

	m_Mix = new Fl_Button(bx+(n++*bs),by,bw,bh,"Mix");
	m_Mix->labelsize(10);
        m_Mix->box (FL_PLASTIC_UP_BOX);
	m_Mix->color (Info->GUI_COLOUR);
	m_Mix->selection_color (Info->GUI_COLOUR);
	m_Mix->callback((Fl_Callback*)cb_Mix);

	m_Crop = new Fl_Button(bx+(n++*bs),by,bw,bh,"Crop");
	m_Crop->labelsize(10);
        m_Crop->box (FL_PLASTIC_UP_BOX);
	m_Crop->color (Info->GUI_COLOUR);
	m_Crop->selection_color (Info->GUI_COLOUR);
	m_Crop->callback((Fl_Callback*)cb_Crop);

	m_Reverse = new Fl_Button(bx+(n++*bs),by,bw,bh,"Reverse");
	m_Reverse->labelsize(10);
        m_Reverse->box (FL_PLASTIC_UP_BOX);
	m_Reverse->color (Info->GUI_COLOUR);
	m_Reverse->selection_color (Info->GUI_COLOUR);
	m_Reverse->callback((Fl_Callback*)cb_Reverse);

	m_Amp = new Fl_Button(bx+(n++*bs),by,bw,bh,"Amp");
	m_Amp->labelsize(10);
        m_Amp->box (FL_PLASTIC_UP_BOX);
	m_Amp->color (Info->GUI_COLOUR);
	m_Amp->selection_color (Info->GUI_COLOUR);
	m_Amp->callback((Fl_Callback*)cb_Amp);

	m_ZoomIn = new Fl_Button(bx+(n++*bs),by,bw,bh,"Zoom +");
	m_ZoomIn->labelsize(10);
        m_ZoomIn->box (FL_PLASTIC_UP_BOX);
	m_ZoomIn->color (Info->GUI_COLOUR);
	m_ZoomIn->selection_color (Info->GUI_COLOUR);
	//m_ZoomIn->callback((Fl_Callback*)cb_ZoomIn);

	m_ZoomOut = new Fl_Button(bx+(n++*bs),by,bw,bh,"Zoom -");
	m_ZoomOut->labelsize(10);
        m_ZoomOut->box (FL_PLASTIC_UP_BOX);
	m_ZoomOut->color (Info->GUI_COLOUR);
	m_ZoomOut->selection_color (Info->GUI_COLOUR);
	//m_ZoomOut->callback((Fl_Callback*)cb_ZoomOut);

	end();

	redraw();
}

void PoshSamplerPluginGUI::UpdateSampleDisplay(int num)
{
	m_GUICH->SetCommand(PoshSamplerPlugin::GETSAMPLE);
	m_GUICH->Wait();
	m_GUICH->RequestChannelAndWait("SampleSize");
	long SampleSize=m_GUICH->GetLong("SampleSize");

	if (SampleSize)
	{
		char *TempBuf = new char[SampleSize];
		m_GUICH->BulkTransfer("SampleBuffer",(void*)TempBuf,SampleSize);		
		m_Display->SetSample((float*)TempBuf,SampleSize/sizeof(float));
		delete[] TempBuf;
	}
}

void PoshSamplerPluginGUI::Update()
{
	SetPlayPos(m_GUICH->GetLong("PlayPos"));
	
	if (m_ZoomIn->value()) m_Display->ZoomIn();	
	if (m_ZoomOut->value()) m_Display->ZoomOut();
	
	if (m_UpdateMe)
	{
		UpdateSampleDisplay((int)m_SampleNum->value());
		m_Display->redraw();
		m_UpdateMe=false;
	}
	//redraw();
}

void PoshSamplerPluginGUI::UpdateValues(SpiralPlugin *o)
{
	PoshSamplerPlugin *Plugin = (PoshSamplerPlugin*)o;

	m_Volume->value(Plugin->GetVolume((int)m_SampleNum->value()));
	m_Pitch->value(Plugin->GetPitch((int)m_SampleNum->value()));
	m_Note->value(Plugin->GetNote((int)m_SampleNum->value()));	
	m_Loop->value(Plugin->GetLoop((int)m_SampleNum->value()));	
	m_UpdateMe=true;
	m_Display->SetPlayStart(Plugin->GetPlayStart((int)m_SampleNum->value()));	
	m_Display->SetLoopStart(Plugin->GetLoopStart((int)m_SampleNum->value()));	
	m_Display->SetLoopEnd(Plugin->GetLoopEnd((int)m_SampleNum->value()));	
	m_Display->redraw();
}
	
inline void PoshSamplerPluginGUI::cb_Load_i(Fl_Button* o, void* v)
{ 
	char *fn=fl_file_chooser("Load a sample", "{*.wav,*.WAV}", NULL);
		
	if (fn && fn!='\0')
	{
		strcpy(m_TextBuf,fn);
		m_GUICH->SetData("Name",m_TextBuf);
		m_GUICH->Set("Num",(int)m_SampleNum->value());
		m_GUICH->SetCommand(PoshSamplerPlugin::LOAD);
		
		m_GUICH->Wait(); // wait for the sample to load
				
		UpdateSampleDisplay((int)m_SampleNum->value());
		m_Display->redraw();
		redraw();
	}
}
void PoshSamplerPluginGUI::cb_Load(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Load_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Save_i(Fl_Button* o, void* v)
{
	char *fn=fl_file_chooser("Save sample", "{*.wav,*.WAV}", NULL);
		
	if (fn && fn!='\0')
	{		
		strcpy(m_TextBuf,fn);
		m_GUICH->Set("Name",m_TextBuf);
		m_GUICH->Set("Num",(int)m_SampleNum->value());
		m_GUICH->SetCommand(PoshSamplerPlugin::SAVE);
	}
}
void PoshSamplerPluginGUI::cb_Save(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Save_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Volume_i(Fl_Knob* o, void* v)
{ 		
	m_GUICH->Set("Value",(float)o->value());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETVOL);
}
void PoshSamplerPluginGUI::cb_Volume(Fl_Knob* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Volume_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Pitch_i(Fl_Knob* o, void* v)
{ 	
	m_GUICH->Set("Value",(float)o->value());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETPITCH);
}
void PoshSamplerPluginGUI::cb_Pitch(Fl_Knob* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Pitch_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Octave_i(Fl_Knob* o, void* v)
{ 
	m_GUICH->Set("Int",(int)o->value());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETOCT);
}
void PoshSamplerPluginGUI::cb_Octave(Fl_Knob* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Octave_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Loop_i(Fl_Button* o, void* v)
{ 	
	m_GUICH->Set("Bool",(bool)o->value());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETLOOP);
}
void PoshSamplerPluginGUI::cb_Loop(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Loop_i(o,v);}

inline void PoshSamplerPluginGUI::cb_PingPong_i(Fl_Button* o, void* v)
{ 	
	m_GUICH->Set("Bool",(bool)o->value());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETPING);
}
void PoshSamplerPluginGUI::cb_PingPong(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_PingPong_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Record_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Bool",(bool)o->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETREC);
	redraw();
}
void PoshSamplerPluginGUI::cb_Record(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Record_i(o,v);}

inline void PoshSamplerPluginGUI::cb_PosMarker_i(Fl_Button* o, void* v)
{ m_Display->SetPosMarker(o->value()); }
void PoshSamplerPluginGUI::cb_PosMarker(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_PosMarker_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Note_i(Fl_Counter* o, void* v)
{ 
	m_GUICH->Set("Int",(int)o->value());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETNOTE);
}
void PoshSamplerPluginGUI::cb_Note(Fl_Counter* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Note_i(o,v);}

inline void PoshSamplerPluginGUI::cb_SampleNum_i(Fl_Counter* o, void* v)
{ 
	if (m_SampleNum->value()<0) m_SampleNum->value(0);
	if (m_SampleNum->value()>7) m_SampleNum->value(7);
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETCURRENT);
	m_GUICH->Wait(); 
	UpdateSampleDisplay((int)m_SampleNum->value());
}
void PoshSamplerPluginGUI::cb_SampleNum(Fl_Counter* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_SampleNum_i(o,v);}


inline void PoshSamplerPluginGUI::cb_Cut_i(Fl_Button* o, void* v)
{	
	m_GUICH->Set("Start",(long)m_Display->GetRangeStart());
	m_GUICH->Set("End",(long)m_Display->GetRangeEnd());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::CUT);
	m_GUICH->Wait(); 
	UpdateSampleDisplay((int)m_SampleNum->value());
	m_Display->redraw();
}
void PoshSamplerPluginGUI::cb_Cut(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Cut_i(o,v);}
	
inline void PoshSamplerPluginGUI::cb_Copy_i(Fl_Button* o, void* v)
{	
	m_GUICH->Set("Start",(long)m_Display->GetRangeStart());
	m_GUICH->Set("End",(long)m_Display->GetRangeEnd());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::COPY);
}
void PoshSamplerPluginGUI::cb_Copy(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Copy_i(o,v);}
	
inline void PoshSamplerPluginGUI::cb_Paste_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",(long)m_Display->GetRangeStart());
	m_GUICH->Set("End",(long)m_Display->GetRangeEnd());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::PASTE);
	m_GUICH->Wait(); 
	UpdateSampleDisplay((int)m_SampleNum->value());
	m_Display->redraw();
}
void PoshSamplerPluginGUI::cb_Paste(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Paste_i(o,v);}
	
inline void PoshSamplerPluginGUI::cb_Mix_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",(long)m_Display->GetRangeStart());
	m_GUICH->Set("End",(long)m_Display->GetRangeEnd());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::MIX);
	m_GUICH->Wait(); 
	UpdateSampleDisplay((int)m_SampleNum->value());
	m_Display->redraw();
}
void PoshSamplerPluginGUI::cb_Mix(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Mix_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Crop_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",(long)m_Display->GetRangeStart());
	m_GUICH->Set("End",(long)m_Display->GetRangeEnd());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::CROP);
	m_GUICH->Wait(); 
	UpdateSampleDisplay((int)m_SampleNum->value());
	m_Display->redraw();
}
void PoshSamplerPluginGUI::cb_Crop(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Crop_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Reverse_i(Fl_Button* o, void* v)
{	
	m_GUICH->Set("Start",(long)m_Display->GetRangeStart());
	m_GUICH->Set("End",(long)m_Display->GetRangeEnd());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::REV);
	m_GUICH->Wait(); 
	UpdateSampleDisplay((int)m_SampleNum->value());
	m_Display->redraw();
}
void PoshSamplerPluginGUI::cb_Reverse(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Reverse_i(o,v);}

inline void PoshSamplerPluginGUI::cb_Amp_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("Start",(long)m_Display->GetRangeStart());
	m_GUICH->Set("End",(long)m_Display->GetRangeEnd());
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::AMP);
	m_GUICH->Wait(); 
	UpdateSampleDisplay((int)m_SampleNum->value());
	m_Display->redraw();
}
void PoshSamplerPluginGUI::cb_Amp(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_Amp_i(o,v);}

inline void PoshSamplerPluginGUI::cb_WaveDisplay_i(Fl_WaveDisplay* o, void* v)
{
	m_GUICH->Set("Start",(long)o->GetPlayStart());
	m_GUICH->Set("End",(long)o->GetLoopEnd());
	m_GUICH->Set("LoopStart",(long)o->GetLoopStart());	
	m_GUICH->Set("Num",(int)m_SampleNum->value());
	m_GUICH->SetCommand(PoshSamplerPlugin::SETPLAYPOINTS);
}
void PoshSamplerPluginGUI::cb_WaveDisplay(Fl_WaveDisplay* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_WaveDisplay_i(o,v);}

inline void PoshSamplerPluginGUI::cb_ZoomIn_i(Fl_Button* o, void* v)
{
	m_Display->ZoomIn();
}
void PoshSamplerPluginGUI::cb_ZoomIn(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_ZoomIn_i(o,v);}

inline void PoshSamplerPluginGUI::cb_ZoomOut_i(Fl_Button* o, void* v)
{
	m_Display->ZoomOut();
}
void PoshSamplerPluginGUI::cb_ZoomOut(Fl_Button* o, void* v)
{ ((PoshSamplerPluginGUI*)(o->parent()))->cb_ZoomOut_i(o,v);}

const string PoshSamplerPluginGUI::GetHelpText(const string &loc){
    return string("") 
	+ "A sampler that allows simple sample editing (cut copy paste etc),\n"
	+ "dirty time stretching (by modulating the start pos + retriggering +\n" 
	+ "modulating pitch) and loop start/end points with ping pong loop mode.\n"
	+ "Also implementations of controls, such as continuous pitch changing,\n" 
	+ "so you can add portmento to samples, trigger velocity sets sample\n"
	+ "volume.\n\n"
	+ "Can records input data too.\n\n"
	+ "Controls:\n"
	+ "lmb: Select region\n"
	+ "mmb: Move view\n"
	+ "rmb: Draws samples at full zoom.\n\n"
	+ "Left mouse also drags loop points. The Loop end marker defaults to the\n" 
	+ "end of the sample.\n\n"
	+ "Note: The loading and saving of samples is not yet realtime safe";
}
