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
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Counter.H>
#include "Fl_Loop.h"
#include "Fl_Trigger.h"

#include "SpiralLoopPlugin.h"
#include "../SpiralPluginGUI.h"
#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_LED_Button.H"

#ifndef MixerGUI
#define MixerGUI


class SpiralLoopPluginGUI : public SpiralPluginGUI
{
public:
	SpiralLoopPluginGUI(int w, int h, SpiralLoopPlugin *o,ChannelHandler *ch,const HostInfo *Info);
	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update();
	virtual const std::string GetHelpText(const std::string &loc);
	
private:
	void UpdateSampleDisplay();
	
	Fl_LED_Button *m_Play;
	
	Fl_Knob   *m_Volume;
	Fl_Knob   *m_Speed;
	Fl_Knob   *m_Length;
	Fl_Knob   *m_WavSize;	

	Fl_Counter *m_Ticks;	
	
	Fl_Button *m_Rec;
	Fl_Button *m_OverDub;
	Fl_Button *m_Load;
	Fl_Button *m_Save;
	Fl_Button *m_Trig;
	Fl_Button *m_Cut;
	Fl_Button *m_Copy;
	Fl_Button *m_Paste;
	Fl_Button *m_PasteMix;
	Fl_Button *m_ZeroR;
	Fl_Button *m_ReverseR;
	Fl_Button *m_SelectAll;	
	Fl_Button *m_Double;	
	Fl_Button *m_Half;	
	Fl_Button *m_Crop;	
	Fl_Button *m_Hold;	
	Fl_Button *m_Undo;	
		
	std::vector<Fl_Trigger*> m_TriggerVec;

	Fl_Loop* m_LoopGUI;
	
	char m_TextBuf[256];
	long m_SampleSize;
	
	inline void cb_Play_i(Fl_LED_Button* o, void* v);
	static void cb_Play(Fl_LED_Button* o, void* v);

	inline void cb_Volume_i(Fl_Knob* o, void* v);
	static void cb_Volume(Fl_Knob* o, void* v);
	inline void cb_Speed_i(Fl_Knob* o, void* v);
	static void cb_Speed(Fl_Knob* o, void* v);
	inline void cb_Length_i(Fl_Knob* o, void* v);
	static void cb_Length(Fl_Knob* o, void* v);
	inline void cb_WavSize_i(Fl_Knob* o, void* v);
	static void cb_WavSize(Fl_Knob* o, void* v);
	
	inline void cb_Rec_i(Fl_Button* o, void* v);
	static void cb_Rec(Fl_Button* o, void* v);
	inline void cb_OverDub_i(Fl_Button* o, void* v);
	static void cb_OverDub(Fl_Button* o, void* v);
	inline void cb_Load_i(Fl_Button* o, void* v);
	static void cb_Load(Fl_Button* o, void* v);
	inline void cb_Save_i(Fl_Button* o, void* v);
	static void cb_Save(Fl_Button* o, void* v);
	inline void cb_Trig_i(Fl_Button* o, void* v);
	static void cb_Trig(Fl_Button* o, void* v);
	inline void cb_Cut_i(Fl_Button* o, void* v);
	static void cb_Cut(Fl_Button* o, void* v);
	inline void cb_Copy_i(Fl_Button* o, void* v);
	static void cb_Copy(Fl_Button* o, void* v);
	inline void cb_Paste_i(Fl_Button* o, void* v);
	static void cb_Paste(Fl_Button* o, void* v);
	inline void cb_PasteMix_i(Fl_Button* o, void* v);
	static void cb_PasteMix(Fl_Button* o, void* v);
	inline void cb_ZeroR_i(Fl_Button* o, void* v);
	static void cb_ZeroR(Fl_Button* o, void* v);
	inline void cb_ReverseR_i(Fl_Button* o, void* v);
	static void cb_ReverseR(Fl_Button* o, void* v);
	inline void cb_SelectAll_i(Fl_Button* o, void* v);
	static void cb_SelectAll(Fl_Button* o, void* v);
	inline void cb_Double_i(Fl_Button* o, void* v);
	static void cb_Double(Fl_Button* o, void* v);
	inline void cb_Half_i(Fl_Button* o, void* v);
	static void cb_Half(Fl_Button* o, void* v);
	inline void cb_Crop_i(Fl_Button* o, void* v);
	static void cb_Crop(Fl_Button* o, void* v);
	inline void cb_Hold_i(Fl_Button* o, void* v);
	static void cb_Hold(Fl_Button* o, void* v);
	inline void cb_Undo_i(Fl_Button* o, void* v);
	static void cb_Undo(Fl_Button* o, void* v);
	inline void cb_Trigger_i(Fl_Trigger* o, void* v);
	static void cb_Trigger(Fl_Trigger* o, void* v);
	inline void cb_Ticks_i(Fl_Counter* o, void* v);
	static void cb_Ticks(Fl_Counter* o, void* v);
};
#endif
