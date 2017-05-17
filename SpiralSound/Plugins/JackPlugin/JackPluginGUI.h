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
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_LED_Button.H"
#include "JackPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef JACK_GUI_H
#define JACK_GUI_H

using namespace std;	

class JackPluginGUI : public SpiralPluginGUI
{
public:
	JackPluginGUI(int w, int h, JackPlugin *o,ChannelHandler *ch,const HostInfo *Info);
	
	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update();
	
protected:
    const std::string GetHelpText(const std::string &loc);	
		
private:
        JackClient    *m_JackClient;
        JackPlugin    *m_JackPlugin;

        Fl_Color m_GUIColour;

	Fl_LED_Button *m_Indicator;

        Fl_Button     *m_Remove;
        Fl_Button     *m_Add;
        
	Fl_Button     *m_Attach;
	Fl_Button     *m_Detach;
	
	Fl_Scroll *m_Scroll;
        Fl_Pack  *m_InputPack;
        Fl_Pack  *m_OutputPack;
        
	std::vector<char*> m_InputName;
	std::vector<Fl_Box*>      m_InputLabel;
	std::vector<Fl_Button*>  m_InputButton;
	
	std::vector<char*> m_OutputName;
	std::vector<Fl_Box*>      m_OutputLabel;
	std::vector<Fl_Button*> m_OutputButton;

	void RemoveInput() ;		
	void RemoveOutput() ;		

	void AddInput() ;		
	void AddOutput() ;		
	
	//// inline Callbacks ////
	inline void cb_Add_i(Fl_Button* o);
	inline void cb_Remove_i(Fl_Button* o);
	inline void cb_Attach_i(Fl_Button* o);
	inline void cb_Detach_i(Fl_Button* o);
	inline void cb_OutputConnect_i(Fl_Button* o);
	inline void cb_InputConnect_i(Fl_Button* o);

	//// Static Callbacks ////
	static void cb_Add(Fl_Button* o, JackPluginGUI* v)  {v->cb_Add_i(o);}
	static void cb_Remove(Fl_Button* o, JackPluginGUI* v) {v->cb_Remove_i(o);}
	static void cb_Attach(Fl_Button* o, JackPluginGUI* v) {v->cb_Attach_i(o);} 
	static void cb_Detach(Fl_Button* o, JackPluginGUI* v) {v->cb_Detach_i(o);}
	static void cb_OutputConnect(Fl_Button* o, JackPluginGUI* v) {v->cb_OutputConnect_i(o);} 
	static void cb_InputConnect(Fl_Button* o, JackPluginGUI* v) {v->cb_InputConnect_i(o);}
};

#endif
