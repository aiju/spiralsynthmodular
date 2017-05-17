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

#include "MatrixPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
//#include <FL/fl_file_chooser.h>

using namespace std;

static const char NoteText[12][3] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

////////////////////////////////////////////

Fl_MatrixButton::Fl_MatrixButton(int x, int y, int w, int h, char* n) :
Fl_Button(x,y,w,h,n),
m_Volume(NULL),
m_SelCol(FL_BLUE),
cb_VolChange(NULL),
cb_context(NULL)
{
	m_SliderHidden=true;
	m_VolVal=255;
}

int Fl_MatrixButton::handle(int event)
{
	if (value()==true && event==FL_PUSH && Fl::event_button()==3)
	{
		if (m_SliderHidden)
		{
			m_Volume = new Fl_Slider(x(),y()+h(),w(),50,"");
			m_Volume->type(4);
			m_Volume->selection_color(m_SelCol);
			m_Volume->maximum(255);
    		m_Volume->step(1);
    		m_Volume->value(255-m_VolVal);
			m_Volume->user_data((void*)this);
			m_Volume->callback((Fl_Callback*)cb_Vol);
			m_Volume->show();
			parent()->add(m_Volume);
			parent()->redraw();
			m_SliderHidden=false;
		}
		else
		{
			m_Volume->hide();
			m_VolVal=255-m_Volume->value();
			parent()->remove(m_Volume);
			parent()->redraw();
			m_Volume=NULL;
			m_SliderHidden=true;
		}

		return 1;
	}

	if (event==FL_PUSH && Fl::event_button()==1 && !m_SliderHidden)
	{
		m_Volume->hide();
		m_VolVal=255-m_Volume->value();
		parent()->remove(m_Volume);
		parent()->redraw();
		m_Volume=NULL;
		m_SliderHidden=true;
	}

	if (Fl::event_button()!=3) return Fl_Button::handle(event);

	return 1;
}

inline void Fl_MatrixButton::cb_Vol_i(Fl_Slider* o, void* v)
{
	m_VolVal=255-m_Volume->value();
	fl_color((char)m_VolVal,(char)m_VolVal,255);
	selection_color(fl_color());

	if (cb_VolChange) cb_VolChange(this,cb_context);
	redraw();
}
void Fl_MatrixButton::cb_Vol(Fl_Slider* o, void* v)
{ ((Fl_MatrixButton*)(o->user_data()))->cb_Vol_i(o,v);}

////////////////////////////////////////////

MatrixPluginGUI::MatrixPluginGUI(int w, int h,MatrixPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
m_LastLight(0),
m_LastPatSeqLight(0)
{
	//size_range(10,10);
	m_Pattern = new Fl_Counter (5, 20, 40, 20, "View");
        m_Pattern->labelsize (10);
	m_Pattern->type (FL_SIMPLE_COUNTER);
        m_Pattern->box (FL_PLASTIC_UP_BOX);
        m_Pattern->color (Info->GUI_COLOUR);
        m_Pattern->step (1);
        m_Pattern->minimum (0);
        m_Pattern->maximum (NUM_PATTERNS-1);
	m_Pattern->value (0);
	m_Pattern->callback ((Fl_Callback*)cb_Pattern);
	add (m_Pattern);

	m_PlayPattern = new Fl_Counter (50, 20, 40, 20, "Play");
        m_PlayPattern->labelsize (10);
	m_PlayPattern->type (FL_SIMPLE_COUNTER);
        m_PlayPattern->box (FL_PLASTIC_UP_BOX);
        m_PlayPattern->color (Info->GUI_COLOUR);
	m_PlayPattern->step (1);
        m_PlayPattern->minimum (0);
        m_PlayPattern->maximum (NUM_PATTERNS-1);
	m_PlayPattern->value (0);
	m_PlayPattern->callback ((Fl_Callback*)cb_PlayPattern);
	add (m_PlayPattern);

        m_Length = new Fl_Counter (5, 55, 40, 20, "Length");
        m_Length->labelsize (10);
	m_Length->type (FL_SIMPLE_COUNTER);
        m_Length->box (FL_PLASTIC_UP_BOX);
        m_Length->color (Info->GUI_COLOUR);
	m_Length->step (1);
	m_Length->value (64);
	m_Length->minimum (1);
	m_Length->maximum (64);
	m_Length->callback ((Fl_Callback*)cb_Length);
	add (m_Length);

	m_Octave = new Fl_Counter(5, 90, 40, 20, "Octave");
        m_Octave->labelsize(10);
	m_Octave->type (FL_SIMPLE_COUNTER);
        m_Octave->box (FL_PLASTIC_UP_BOX);
        m_Octave->color (Info->GUI_COLOUR);
        m_Octave->minimum (0);
        m_Octave->maximum (6);
	m_Octave->step (1);
	m_Octave->value (0);
	m_Octave->callback((Fl_Callback*)cb_Octave);
	add (m_Octave);

        m_Speed = new Fl_Knob (50, 60, 40, 40, "Speed");
        m_Speed->color (Info->GUI_COLOUR);
	m_Speed->type (Fl_Knob::DOTLIN);
        m_Speed->labelsize (10);
        m_Speed->minimum (0);
        m_Speed->maximum (400);
        m_Speed->step (0.01);
        m_Speed->value (8);
	m_Speed->callback ((Fl_Callback*)cb_Speed);
	add (m_Speed);

	m_SpeedVal = new Fl_Counter (5, 125, 85, 20, "");
	m_SpeedVal->labelsize (10);
	m_SpeedVal->value (10);
	m_SpeedVal->type (FL_SIMPLE_COUNTER);
        m_SpeedVal->box (FL_PLASTIC_UP_BOX);
        m_SpeedVal->color (Info->GUI_COLOUR);
	m_SpeedVal->step (1);
	m_SpeedVal->maximum (400);
	m_SpeedVal->minimum (0);
        m_SpeedVal->value (8);
	m_SpeedVal->callback ((Fl_Callback*)cb_SpeedVal);
	add (m_SpeedVal);

        m_CopyBtn = new Fl_Button (5, 150, 40, 20, "Copy");
	m_CopyBtn->labelsize (10);
        m_CopyBtn->box (FL_PLASTIC_UP_BOX);
        m_CopyBtn->color (Info->GUI_COLOUR);
        m_CopyBtn->selection_color (Info->GUI_COLOUR);
        m_CopyBtn->callback ((Fl_Callback*)cb_CopyBtn);
	add (m_CopyBtn);

	m_PasteBtn = new Fl_Button (50, 150, 40, 20, "Paste");
	m_PasteBtn->labelsize (10);
        m_PasteBtn->box (FL_PLASTIC_UP_BOX);
        m_PasteBtn->color (Info->GUI_COLOUR);
        m_PasteBtn->selection_color (Info->GUI_COLOUR);
	m_PasteBtn->deactivate();
	m_PasteBtn->callback ((Fl_Callback*)cb_PasteBtn);
	add (m_PasteBtn);

	m_ClearBtn = new Fl_Button (5, 175, 85, 20, "Clear");
	m_ClearBtn->labelsize (10);
        m_ClearBtn->box (FL_PLASTIC_UP_BOX);
        m_ClearBtn->color (Info->GUI_COLOUR);
        m_ClearBtn->selection_color (Info->GUI_COLOUR);
	m_ClearBtn->callback ((Fl_Callback*)cb_ClearBtn);
	add (m_ClearBtn);

	m_TransUpBtn = new Fl_Button (5, 200, 40, 20, "Up");
	m_TransUpBtn->labelsize (10);
        m_TransUpBtn->box (FL_PLASTIC_UP_BOX);
        m_TransUpBtn->color (Info->GUI_COLOUR);
        m_TransUpBtn->selection_color (Info->GUI_COLOUR);
	m_TransUpBtn->callback ((Fl_Callback*)cb_TransUpBtn);
	add (m_TransUpBtn);

	m_TransDnBtn = new Fl_Button (50, 200, 40, 20, "Down");
	m_TransDnBtn->labelsize (10);
        m_TransDnBtn->box (FL_PLASTIC_UP_BOX);
        m_TransDnBtn->color (Info->GUI_COLOUR);
        m_TransDnBtn->selection_color (Info->GUI_COLOUR);
	m_TransDnBtn->callback ((Fl_Callback*)cb_TransDnBtn);
	add (m_TransDnBtn);

	m_TransLbl = new Fl_Box (5, 216, 85, 20, "Transpose");
	m_TransLbl->labelsize(10);
	add (m_TransLbl);

      	m_NoteCut = new Fl_Button (5, h-30, 85, 20, "NoteCut");
	m_NoteCut->type (FL_TOGGLE_BUTTON);
        m_NoteCut->box (FL_PLASTIC_UP_BOX);
        m_NoteCut->color (Info->GUI_COLOUR);
        m_NoteCut->selection_color (Info->GUI_COLOUR);
	m_NoteCut->labelsize (10);
	m_NoteCut->value (0);
	m_NoteCut->callback ((Fl_Callback*)cb_NoteCut);
	add (m_NoteCut);

        int xoff=105;
	int yoff=40;
	int butsize=7;
	int n=0;

	fl_color(150,150,150);
	int markercol=fl_color();

	fl_color(170,170,170);
	int blcolour=fl_color();

	for(int x=0; x<MATX; x++)
	for(int y=0; y<MATY; y++)
	{
		Numbers[n]=n;
		m_Matrix[x][y] = new Fl_MatrixButton(xoff+x*butsize,yoff+((MATY-1)*butsize)-(y*butsize),butsize+1,butsize+1,"");
		m_Matrix[x][y]->type(1);
		m_Matrix[x][y]->box(FL_BORDER_BOX);
                m_Matrix[x][y]->SetSelColour (Info->GUI_COLOUR);
		if ((x%8)==0) m_Matrix[x][y]->color(markercol);
		else if ((y%12)==1 || (y%12)==3 || (y%12)==6 || (y%12)==8 || (y%12)==10) m_Matrix[x][y]->color(blcolour);
		else m_Matrix[x][y]->color(FL_GRAY);

		m_Matrix[x][y]->selection_color(FL_WHITE);
		m_Matrix[x][y]->callback((Fl_Callback*)cb_Matrix,(void*)&Numbers[n]);
		m_Matrix[x][y]->SetVolCallback((Fl_Callback*)cb_MatVol,(void*)&Numbers[n]);
		add(m_Matrix[x][y]);
		n++;
	}

	yoff=37;
	for(int y=0; y<MATY; y++)
	{
		Fl_Box *box = new Fl_Box(90,yoff+((MATY-1)*butsize)-(y*butsize),15,15,NoteText[y%12]);
		box->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
		box->labelsize(8);
	}

	xoff=103;
	for(int x=0; x<MATX; x++)
	{
		m_Flash[x] = new Fl_LED_Button(xoff+x*butsize,20,15,15,"");
		m_Flash[x]->selection_color(FL_WHITE);
		add(m_Flash[x]);
	}

	xoff=560;
	yoff=40;
	int height=12,gap=2;

	Fl_Box *patseqlabel = new Fl_Box(xoff,yoff-15,30,10,"Pat Seq");
	patseqlabel->labelsize(10);
	add(patseqlabel);

	for(int y=0; y<NUM_PATSEQ; y++)
	{
		m_PatSeq[y]= new Fl_Counter(xoff,yoff+y*(height+gap),25,height);
		m_PatSeq[y]->type(FL_SIMPLE_COUNTER);
		m_PatSeq[y]->step(1);
		m_PatSeq[y]->textsize(8);
                if (y==0) m_PatSeq[y]->minimum (0);
                else m_PatSeq[y]->minimum (-1);
                m_PatSeq[y]->value (m_PatSeq[y]->minimum ());
                m_PatSeq[y]->maximum (NUM_PATTERNS-1);
		m_PatSeq[y]->callback((Fl_Callback*)cb_PatSeq,(void*)&Numbers[y]);
		add(m_PatSeq[y]);

		m_PatSeqFlash[y] = new Fl_LED_Button(xoff+25,yoff+y*(height+gap),15,15,"");
		m_PatSeqFlash[y]->selection_color(FL_WHITE);
		add(m_PatSeqFlash[y]);
	}

	end();
}

void MatrixPluginGUI::Update()
{
	int Light=m_GUICH->GetInt("Step");
	if (Light!=m_LastLight)
	{
		m_Flash[Light]->value(1);
		m_Flash[m_LastLight]->value(0);
		m_LastLight=Light;

                m_PlayPattern->value (m_GUICH->GetInt("EchoCur"));

		int PatSeqLight=m_GUICH->GetInt("PatSeqStep");
		if (PatSeqLight!=m_LastPatSeqLight)
		{
			m_PatSeqFlash[PatSeqLight]->value(1);
			m_PatSeqFlash[m_LastPatSeqLight]->value(0);
			m_LastPatSeqLight=PatSeqLight;
		}
	}
}

void MatrixPluginGUI::UpdateValues(SpiralPlugin *o)
{
	MatrixPlugin *Plugin = (MatrixPlugin*)o;

	m_Pattern->value(Plugin->GetCurrent());
	m_Length->value(Plugin->GetPattern()->Length);
	m_Speed->value(Plugin->GetPattern()->Speed*8);
	m_SpeedVal->value((int)m_Speed->value());
	m_Octave->value(Plugin->GetPattern()->Octave);

	for(int x=0; x<MATX; x++)
	for(int y=0; y<MATY; y++)
	{
		m_Matrix[x][y]->value(Plugin->GetPattern()->Matrix[x][y]);
		m_Matrix[x][y]->SetVolume(Plugin->GetPattern()->Volume[x][y]);
	}

	for(int n=0; n<NUM_PATSEQ; n++)
	{
		m_PatSeq[n]->value(Plugin->GetPatSeq(n));
	}
}

void MatrixPluginGUI::UpdateMatrix()
{
	m_GUICH->Wait();
	m_GUICH->RequestChannelAndWait("Matrix");
	m_GUICH->GetData("Matrix",(void*)m_GUIMatrix);

	Pattern *p=&m_GUIMatrix[(int)m_Pattern->value()];

	m_Length->value(p->Length);
	m_Speed->value(p->Speed*8);
	m_SpeedVal->value((int)m_Speed->value());
	m_Octave->value(p->Octave);

	for(int x=0; x<MATX; x++)
	for(int y=0; y<MATY; y++)
	{
		m_Matrix[x][y]->value(p->Matrix[x][y]);
		m_Matrix[x][y]->SetVolume(p->Volume[x][y]);
	}

}

// Callbacks - Side Panel - Top Section

inline void MatrixPluginGUI::cb_Pattern_i (Fl_Counter* o, void* v) {
       m_GUICH->Set ("Current", (int)o->value());
       UpdateMatrix();
}

void MatrixPluginGUI::cb_Pattern (Fl_Counter* o, void* v) {
     ((MatrixPluginGUI*)(o->parent()))->cb_Pattern_i (o, v);
}

inline void MatrixPluginGUI::cb_PlayPattern_i (Fl_Counter* o, void* v) {
	m_GUICH->Set ("Num", (int)o->value());
	m_GUICH->SetCommand (MatrixPlugin::SET_CURRENT);
}

void MatrixPluginGUI::cb_PlayPattern(Fl_Counter* o, void* v) {
     ((MatrixPluginGUI*)(o->parent()))->cb_PlayPattern_i (o, v);
}

inline void MatrixPluginGUI::cb_Length_i (Fl_Counter* o, void* v)
{
	m_GUICH->Set ("Length", (int)o->value());
	m_GUICH->SetCommand (MatrixPlugin::MAT_LENGTH);
}

void MatrixPluginGUI::cb_Length (Fl_Counter* o, void* v) {
     ((MatrixPluginGUI*)(o->parent()))->cb_Length_i (o, v);
}

inline void MatrixPluginGUI::cb_Octave_i (Fl_Counter* o, void* v) {
	m_GUICH->Set ("Octave", (int)o->value());
	m_GUICH->SetCommand (MatrixPlugin::MAT_OCTAVE);
}

void MatrixPluginGUI::cb_Octave (Fl_Counter* o, void* v) {
     ((MatrixPluginGUI*)(o->parent()))->cb_Octave_i (o, v);
}

inline void MatrixPluginGUI::cb_Speed_i (Fl_Knob* o, void* v)
{
	// Round off value, but it should be a float for tweaking
	float value = o->value() + ((int)o->value() - o->value());
	m_SpeedVal->value (value);
	m_GUICH->Set ("Speed", (float)value / 8.0f);
	m_GUICH->SetCommand (MatrixPlugin::MAT_SPEED);
}

void MatrixPluginGUI::cb_Speed (Fl_Knob* o, void* v) {
     ((MatrixPluginGUI*)(o->parent()))->cb_Speed_i (o, v);
}

inline void MatrixPluginGUI::cb_SpeedVal_i (Fl_Counter* o, void* v)
{
	float value = o->value();
	m_Speed->value (value);
	m_GUICH->Set ("Speed", (float)value / 8.0f);
	m_GUICH->SetCommand (MatrixPlugin::MAT_SPEED);
}

void MatrixPluginGUI::cb_SpeedVal (Fl_Counter* o, void* v) {
     ((MatrixPluginGUI*)(o->parent()))->cb_SpeedVal_i (o, v);
}

// Callbacks - Side Panel - Editing Section

inline void MatrixPluginGUI::cb_CopyBtn_i (Fl_Button* o, void* v) {
       m_PasteBtn->activate();
       m_GUICH->SetCommand (MatrixPlugin::COPY);
       UpdateMatrix();
}

void MatrixPluginGUI::cb_CopyBtn (Fl_Button* o, void* v) {
     ((MatrixPluginGUI*)(o->parent())) -> cb_CopyBtn_i (o, v);
}

inline void MatrixPluginGUI::cb_PasteBtn_i (Fl_Button* o, void* v) {
       m_GUICH->SetCommand (MatrixPlugin::PASTE);
       UpdateMatrix();
}

void MatrixPluginGUI::cb_PasteBtn (Fl_Button* o, void* v) {
     ((MatrixPluginGUI*)(o->parent())) -> cb_PasteBtn_i (o, v);
}

inline void MatrixPluginGUI::cb_ClearBtn_i (Fl_Button* o, void* v) {
       m_GUICH->SetCommand (MatrixPlugin::CLEAR);
       UpdateMatrix();
}

void MatrixPluginGUI::cb_ClearBtn (Fl_Button* o, void* v)  {
     ((MatrixPluginGUI*)(o->parent())) -> cb_ClearBtn_i (o, v);
}

// Callbacks - Side Panel - Transpose Section

inline void MatrixPluginGUI::cb_TransUpBtn_i (Fl_Button* o, void* v) {
       m_GUICH->SetCommand(MatrixPlugin::TUP);
       UpdateMatrix();
}

void MatrixPluginGUI::cb_TransUpBtn (Fl_Button* o, void* v) {
     ((MatrixPluginGUI*)(o->parent())) -> cb_TransUpBtn_i (o, v);
}

inline void MatrixPluginGUI::cb_TransDnBtn_i (Fl_Button* o, void* v) {
       m_GUICH->SetCommand(MatrixPlugin::TDOWN);
       UpdateMatrix();
}

void MatrixPluginGUI::cb_TransDnBtn (Fl_Button* o, void* v) {
     ((MatrixPluginGUI*)(o->parent())) -> cb_TransDnBtn_i (o, v);
}

// Callbacks - Side Panel - Bottom Section

inline void MatrixPluginGUI::cb_NoteCut_i (Fl_Button* o, void* v) {
	m_GUICH->Set ("NoteCut",o->value());
}

void MatrixPluginGUI::cb_NoteCut (Fl_Button* o, void* v) {
     ((MatrixPluginGUI*)(o->parent()))->cb_NoteCut_i (o,v);
}

// Callbacks - Matrix

inline void  MatrixPluginGUI::cb_Matrix_i (Fl_Button* o, void* v) {
       m_GUICH->Set ("X", *(int*)v/MATY);
       m_GUICH->Set ("Y", *(int*)v%MATY);
       if (o->value()) m_GUICH->SetCommand (MatrixPlugin::MAT_ACTIVATE);
       else m_GUICH->SetCommand (MatrixPlugin::MAT_DEACTIVATE);
       m_GUICH->Wait();
}

void  MatrixPluginGUI::cb_Matrix (Fl_Button* o, void* v) {
      ((MatrixPluginGUI*)(o->parent()))->cb_Matrix_i (o, v);
}

inline void  MatrixPluginGUI::cb_MatVol_i (Fl_Button* o, void* v) {
       m_GUICH->Set ("X", *(int*)v/MATY);
       m_GUICH->Set ("Y", *(int*)v%MATY);
       m_GUICH->Set ("Volume", ((Fl_MatrixButton*)o)->GetVolume());
       m_GUICH->SetCommand (MatrixPlugin::MAT_VOLUME);
}

void  MatrixPluginGUI::cb_MatVol (Fl_Button* o, void* v) {
      ((MatrixPluginGUI*)(o->parent()))->cb_MatVol_i (o, v);
}

// Callbacks - Pattern Sequencer

inline void MatrixPluginGUI::cb_PatSeq_i (Fl_Counter* o, void* v) {
       m_GUICH->Set("Num", (int)o->value());
       m_GUICH->Set("Y", *(int*)v);
       m_GUICH->SetCommand (MatrixPlugin::SET_PATSEQ);
}

void MatrixPluginGUI::cb_PatSeq (Fl_Counter* o, void* v) {
     ((MatrixPluginGUI*)(o->parent()))->cb_PatSeq_i (o, v);
}

// Help text

const string MatrixPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "This is a matrix style step sequencer for techno purists. Great for\n" 
	+ "drum programming, but also capable of robotic bass and melodies.\n\n"
	+ "Note events are turned on and off by activating the 64*32 grid of\n"
	+ "toggle buttons on the GUI. The speed and octave of the notes can be\n" 
	+ "set. The length of the pattern in notes and note cut are also\n"
	+ "present. The current play position is indicated by the row of LED's\n" 
	+ "along the top of the plugin window. You can copy and paste between\n"
	+ "different patterns and transpose the notes\n\n"
	+ "16 triggers are supplied for simultaneous triggering of samples in the\n" 
	+ "sampler plugin (for instance).\n\n"
	+ "Each Matrix plugin can contain up to 16 different patterns, these are\n"  
	+ "selectable on the plugin GUI, and triggerable by the play trigger CV.\n"  
	+ "This CV input takes a note frequency CV rather than just a trigger,\n"  
	+ "and the bottom 16 notes on the midi scale trigger the 16 patterns.\n" 
	+ "This means that a matrix can trigger another matrix, if set to a much\n"  
	+ "lower speed, and (set to octave 0) the notes trigger the differnt\n" 
	+ "patterns in the slave matrix.\n\n"
	+ "The external clock input is an important alternative to the matrixes\n" 
	+ "internal clock, it allows you to have multiple matrixes powered from one\n"
	+ "oscillator clock. To allow you to sync these matrixes, the matrix is\n"
	+ "provided with a Reset Trigger, which when plugged into the Play Trigger\n"
	+ "of another matrix, will synch the two so they start at the same clock\n" 
	+ "pulse.\n\n"
	+ "On the right hand side of the matrix you will find the pattern sequencer\n"
	+ "this will advance each time a full pattern is played, and you can use it\n"
	+ "to select which pattern will be played next. There are a maximum of 16\n"
	+ "patterns to the sequence, but you can use less by setting a slot to -1\n"
	+ "this will cause the sequence to loop back to zero";
}
