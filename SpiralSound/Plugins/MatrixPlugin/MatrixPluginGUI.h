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

#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Box.H>
#include "MatrixPlugin.h"
#include "../SpiralPluginGUI.h"
#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_LED_Button.H"

#ifndef MatrixGUI
#define MatrixGUI

class Fl_MatrixButton : public Fl_Button
{
public:
	Fl_MatrixButton(int x, int y, int w, int h, char* n);
	~Fl_MatrixButton() {}

	virtual int handle(int event);

	float GetVolume() { return m_VolVal/255.0f; }
	void  SetVolume(float s)
	{
		m_VolVal=s*255;
		fl_color((char)m_VolVal,(char)m_VolVal,255);
		selection_color(fl_color());
	}
	void SetVolCallback(Fl_Callback* s, void *c)   { cb_VolChange=s; cb_context=c; }
        void SetSelColour (unsigned c) { m_SelCol=(Fl_Color)c; }

private:
	bool       m_SliderHidden;
	Fl_Slider *m_Volume;
	float      m_VolVal;
        Fl_Color   m_SelCol;

	void (*cb_VolChange)(Fl_Widget*, void*);
	void  *cb_context;

	inline void cb_Vol_i(Fl_Slider* o, void* v);
	static void cb_Vol(Fl_Slider* o, void* v);

};

class MatrixPluginGUI : public SpiralPluginGUI
{
public:
	MatrixPluginGUI(int w, int h, MatrixPlugin *o,ChannelHandler *ch,const HostInfo *Info);

	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update();

protected:
    const std::string GetHelpText(const std::string &loc);

private:

	void UpdateMatrix();

	int Numbers[MATX*MATY];
	Pattern m_GUIMatrix[NUM_PATTERNS];
	int m_LastLight;
	int m_LastPatSeqLight;

	Fl_Button*  m_NoteCut;
	Fl_Counter* m_Pattern;
	Fl_Counter* m_PlayPattern;
	Fl_Counter* m_Length;
	Fl_Knob*    m_Speed;
	Fl_MatrixButton*  m_Matrix[MATX][MATY];
	Fl_Counter* m_Octave;
	Fl_Counter* m_SpeedVal;
	Fl_LED_Button* m_Flash[MATX];
	Fl_Button* m_CopyBtn;
	Fl_Button* m_PasteBtn;
	Fl_Button* m_ClearBtn;
	Fl_Box* m_TransLbl;
        Fl_Button* m_TransUpBtn;
	Fl_Button* m_TransDnBtn;
	Fl_Counter* m_PatSeq[NUM_PATSEQ];
	Fl_LED_Button* m_PatSeqFlash[NUM_PATSEQ];
	//// Callbacks ////
	inline void cb_Matrix_i (Fl_Button* o, void* v);
	static void cb_Matrix (Fl_Button* o, void* v);
	inline void cb_MatVol_i (Fl_Button* o, void* v);
	static void cb_MatVol (Fl_Button* o, void* v);
	inline void cb_Pattern_i (Fl_Counter* o, void* v);
	static void cb_Pattern (Fl_Counter* o, void* v);
	inline void cb_PlayPattern_i (Fl_Counter* o, void* v);
	static void cb_PlayPattern (Fl_Counter* o, void* v);
	inline void cb_Length_i (Fl_Counter* o, void* v);
	static void cb_Length (Fl_Counter* o, void* v);
	inline void cb_Speed_i (Fl_Knob* o, void* v);
	static void cb_Speed (Fl_Knob* o, void* v);
	inline void cb_SpeedVal_i (Fl_Counter* o, void* v);
	static void cb_SpeedVal (Fl_Counter* o, void* v);
	inline void cb_NoteCut_i (Fl_Button* o, void* v);
	static void cb_NoteCut (Fl_Button* o, void* v);
	inline void cb_Octave_i (Fl_Counter* o, void* v);
	static void cb_Octave (Fl_Counter* o, void* v);
        inline void cb_CopyBtn_i (Fl_Button* o, void* v);
	static void cb_CopyBtn (Fl_Button* o, void* v);
        inline void cb_PasteBtn_i (Fl_Button* o, void* v);
	static void cb_PasteBtn (Fl_Button* o, void* v);
        inline void cb_ClearBtn_i (Fl_Button* o, void* v);
	static void cb_ClearBtn (Fl_Button* o, void* v);
        inline void cb_TransUpBtn_i (Fl_Button* o, void* v);
	static void cb_TransUpBtn (Fl_Button* o, void* v);
        inline void cb_TransDnBtn_i (Fl_Button* o, void* v);
	static void cb_TransDnBtn (Fl_Button* o, void* v);
	inline void cb_PatSeq_i (Fl_Counter* o, void* v);
	static void cb_PatSeq (Fl_Counter* o, void* v);
};

#endif
