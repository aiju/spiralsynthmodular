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

#include "MidiPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Double_Window.H>

using namespace std;

////////////////////////////////////////////////////////////////////////

int OptionsList(const vector<string> &List)
{
	Fl_Double_Window *Win    = new Fl_Double_Window(300,300);
	Fl_Button *Ok            = new Fl_Button(10,275,40,20,"Ok");
	Ok->labelsize(10);
	Fl_Button *Cancel        = new Fl_Button(50,275,40,20,"Cancel");
	Cancel->labelsize(10);
	Fl_Hold_Browser* Browser = new Fl_Hold_Browser(5,5,290,265,"");

	for (vector<string>::const_iterator i = List.begin();
		 i!=List.end(); i++)
	{
		Browser->add(i->c_str());
	}

	Win->show();

	int Choice=-1;

	for (;;)
	{
    	Fl::wait();
    	Fl_Widget* o = Fl::readqueue();
    	if (o==Ok || o==Browser)
		{
			Choice=Browser->value();
			Win->hide();
			delete Win;
			break;
		}
		if (o==Cancel)
		{
			Choice=-1;
			Win->hide();
			delete Win;
			break;
		}

		if (o==Win) break;
  	}

	return Choice;
}

////////////////////////////////////////////////////////////////////////

MidiPluginGUI::MidiPluginGUI(int w, int h,MidiPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{
	m_DeviceNum = new Fl_Counter (12, 15, 56, 20, "Channel");
	m_DeviceNum->type (FL_SIMPLE_COUNTER);
        m_DeviceNum->box (FL_PLASTIC_UP_BOX);
        m_DeviceNum->color (Info->GUI_COLOUR);
	m_DeviceNum->step(1);
	m_DeviceNum->value(0);
	m_DeviceNum->callback((Fl_Callback*)cb_DeviceNum, NULL);

	m_NoteCut = new Fl_Button (0, 52, 80, 20, "Note Cut");
	m_NoteCut->type (FL_TOGGLE_BUTTON);
        m_NoteCut->box (FL_PLASTIC_UP_BOX);
        m_NoteCut->color (Info->GUI_COLOUR);
        m_NoteCut->selection_color (Info->GUI_COLOUR);
	m_NoteCut->labelsize(10);
	m_NoteCut->callback((Fl_Callback*)cb_NoteCut, NULL);

	m_ContinuousNotes = new Fl_Button (0, 72, 80, 20, "Cont Notes");
	m_ContinuousNotes->type (FL_TOGGLE_BUTTON);
        m_ContinuousNotes->box (FL_PLASTIC_UP_BOX);
        m_ContinuousNotes->color (Info->GUI_COLOUR);
        m_ContinuousNotes->selection_color (Info->GUI_COLOUR);
	m_ContinuousNotes->labelsize(10);
	m_ContinuousNotes->callback((Fl_Callback*)cb_ContinuousNotes, NULL);

	m_AddControl = new Fl_Button (0, 92, 80, 20, "Add Control");
        m_AddControl->box (FL_PLASTIC_UP_BOX);
        m_AddControl->color (Info->GUI_COLOUR);
        m_AddControl->selection_color (Info->GUI_COLOUR);
	m_AddControl->labelsize(10);
	m_AddControl->callback((Fl_Callback*)cb_AddControl, NULL);

	m_RemoveControl = new Fl_Button (0, 112, 80, 20, "Remove Control");
        m_RemoveControl->box (FL_PLASTIC_UP_BOX);
        m_RemoveControl->color (Info->GUI_COLOUR);
        m_RemoveControl->selection_color (Info->GUI_COLOUR);
	m_RemoveControl->labelsize(10);
	m_RemoveControl->callback((Fl_Callback*)cb_RemoveControl, NULL);
}

void MidiPluginGUI::UpdateValues(SpiralPlugin *o)
{
	MidiPlugin *Plugin = (MidiPlugin*)o;
	m_DeviceNum->value(Plugin->GetDeviceNum());
}

//// Callbacks ////
inline void MidiPluginGUI::cb_DeviceNum_i(Fl_Counter* o, void* v)
{
	if (o->value()<0) o->value(0);
	if (o->value()>127) o->value(127);
	
	m_GUICH->Set("DeviceNum",(int)o->value()); 
}
void MidiPluginGUI::cb_DeviceNum(Fl_Counter* o, void* v)
{ ((MidiPluginGUI*)(o->parent()))->cb_DeviceNum_i(o,v);}

inline void MidiPluginGUI::cb_NoteCut_i(Fl_Button* o, void* v)
{ m_GUICH->Set("NoteCut",o->value()); }
void MidiPluginGUI::cb_NoteCut(Fl_Button* o, void* v)
{ ((MidiPluginGUI*)(o->parent()))->cb_NoteCut_i(o,v);}

inline void MidiPluginGUI::cb_ContinuousNotes_i(Fl_Button* o, void* v)
{ m_GUICH->Set("ContinuousNotes",o->value()); }
void MidiPluginGUI::cb_ContinuousNotes(Fl_Button* o, void* v)
{ ((MidiPluginGUI*)(o->parent()))->cb_ContinuousNotes_i(o,v);}

inline void MidiPluginGUI::cb_AddControl_i(Fl_Button* o, void* v)
{
	vector<string> List;

	List.push_back("0   Bank Select");
	List.push_back("1   Modulation Wheel (coarse)");
	List.push_back("2   Breath controller (coarse)");
	List.push_back("3   None");
	List.push_back("4   Foot Pedal (coarse)");
	List.push_back("5   Portamento Time (coarse)");
	List.push_back("6   Data Entry (coarse)");
	List.push_back("7   Volume (coarse)");
	List.push_back("8   Balance (coarse)");
	List.push_back("9   None");
	List.push_back("10  Pan position (coarse)");
	List.push_back("11  Expression (coarse)");
	List.push_back("12  Effect Control 1 (coarse)");
	List.push_back("13  Effect Control 2 (coarse)");
	List.push_back("14  None");
	List.push_back("15  None");
	List.push_back("16  General Purpose Slider 1");
	List.push_back("17  General Purpose Slider 2");
	List.push_back("18  General Purpose Slider 3");
	List.push_back("19  General Purpose Slider 4");
	List.push_back("20  None");
	List.push_back("21  None");
	List.push_back("22  None");
	List.push_back("23  None");
	List.push_back("24  None");
	List.push_back("25  None");
	List.push_back("26  None");
	List.push_back("27  None");
	List.push_back("28  None");
	List.push_back("29  None");
	List.push_back("30  None");
	List.push_back("31  None");
	List.push_back("32  Bank Select (fine)");
	List.push_back("33  Modulation Wheel (fine)");
	List.push_back("34  Breath controller (fine)");
	List.push_back("35  None");
	List.push_back("36  Foot Pedal (fine)");
	List.push_back("37  Portamento Time (fine)");
	List.push_back("38  Data Entry (fine)");
	List.push_back("39  Volume (fine)");
	List.push_back("40  Balance (fine)");
	List.push_back("41  None");
	List.push_back("42  Pan position (fine)");
	List.push_back("43  Expression (fine)");
	List.push_back("44  Effect Control 1 (fine)");
	List.push_back("45  Effect Control 2 (fine)");
	List.push_back("46  None");
	List.push_back("47  None");
	List.push_back("48  None");
	List.push_back("49  None");
	List.push_back("50  None");
	List.push_back("51  None");
	List.push_back("52  None");
	List.push_back("53  None");
	List.push_back("54  None");
	List.push_back("55  None");
	List.push_back("56  None");
	List.push_back("57  None");
	List.push_back("58  None");
	List.push_back("59  None");
	List.push_back("60  None");
	List.push_back("61  None");
	List.push_back("62  None");
	List.push_back("63  None");
	List.push_back("64  Hold Pedal (on/off)");
	List.push_back("65  Portamento (on/off)");
	List.push_back("66  Sustenuto Pedal (on/off)");
	List.push_back("67  Soft Pedal (on/off)");
	List.push_back("68  Legato Pedal (on/off)");
	List.push_back("69  Hold 2 Pedal (on/off)");
	List.push_back("70  Sound Variation");
	List.push_back("71  Sound Timbre");
	List.push_back("72  Sound Release Time");
	List.push_back("73  Sound Attack Time");
	List.push_back("74  Sound Brightness");
	List.push_back("75  Sound Control 6");
	List.push_back("76  Sound Control 7");
	List.push_back("77  Sound Control 8");
	List.push_back("78  Sound Control 9");
	List.push_back("79  Sound Control 10");
	List.push_back("80  General Purpose Button 1 (on/off)");
	List.push_back("81  General Purpose Button 2 (on/off)");
	List.push_back("82  General Purpose Button 3 (on/off)");
	List.push_back("83  General Purpose Button 4 (on/off)");
	List.push_back("84  None");
	List.push_back("85  None");
	List.push_back("86  None");
	List.push_back("87  None");
	List.push_back("88  None");
	List.push_back("89  None");
	List.push_back("90  None");
	List.push_back("91  Effects Level");
	List.push_back("92  Tremulo Level");
	List.push_back("93  Chorus Level");
	List.push_back("94  Celeste Level");
	List.push_back("95  Phaser Level");
	List.push_back("96  Data Button increment");
	List.push_back("97  Data Button decrement");
	List.push_back("98  Non-registered Parameter (fine)");
	List.push_back("99  Non-registered Parameter (coarse)");
	List.push_back("100 Registered Parameter (fine)");
	List.push_back("101 Registered Parameter (coarse)");
	List.push_back("102 None");
	List.push_back("103 None");
	List.push_back("104 None");
	List.push_back("105 None");
	List.push_back("106 None");
	List.push_back("107 None");
	List.push_back("108 None");
	List.push_back("109 None");
	List.push_back("110 None");
	List.push_back("111 None");
	List.push_back("112 None");
	List.push_back("113 None");
	List.push_back("114 None");
	List.push_back("115 None");
	List.push_back("116 None");
	List.push_back("117 None");
	List.push_back("118 None");
	List.push_back("119 None");
	List.push_back("120 All Sound Off");
	List.push_back("121 All Controllers Off");
	List.push_back("122 Local Keyboard (on/off)");
	List.push_back("123 All Notes Off");
	List.push_back("124 Omni Mode Off");
	List.push_back("125 Omni Mode On");
	List.push_back("126 Mono Operation");
	List.push_back("127 Poly Operation");

	int c=OptionsList(List)-1;
	if (c>-1)
	{
		m_GUICH->Set("CC",c);
		char Temp[256];
		sprintf(Temp,"%s",List[c].c_str());
		m_GUICH->SetData("Name",Temp);
		m_GUICH->SetCommand(MidiPlugin::ADDCONTROL);
                m_GUICH->Wait ();
                // Make sure GUI frame can show all the ports
                Resize (w(), h());
		//m_Plugin->AddControl(c,List[c]);
	}
}

void MidiPluginGUI::cb_AddControl (Fl_Button* o, void* v) {
   ((MidiPluginGUI*)(o->parent()))->cb_AddControl_i (o, v);
}

inline void MidiPluginGUI::cb_RemoveControl_i (Fl_Button* o, void* v) {
   m_GUICH->SetCommand(MidiPlugin::DELCONTROL);
   m_GUICH->Wait ();
   // Make sure GUI frame can show all the ports
   Resize (w(), h());
}

void MidiPluginGUI::cb_RemoveControl (Fl_Button* o, void* v) {
   ((MidiPluginGUI*)(o->parent()))->cb_RemoveControl_i (o, v);
}

const string MidiPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "A plugin that converts midi signals into SSM's CVs. The midi plugin\n"
	+ "outputs a Frequency CV, which comes from the last note pressed and\n"
	+ "Trigger CV which lets you know the timing and the velocity of the\n"
	+ "event.\n"
    + "It also detects pitchbend, channel pressure and aftertouch information,\n"
	+ "and outputs these as continuous CV data.\n\n"
    + "You can select the Midi device used with the selector. Devices 0-16 are\n"
	+ "valid Midi channels. You can run multiple Midi plugins simultaneously\n"
	+ "to get data from different device numbers to make an externally\n"
	+ "controlled song.\n\n"
    + "The note frequency CV can optionally be switched into \"Note Cut\" mode,\n"
	+ "where the frequecy is set to zero when the key is released. This is\n"
	+ "useful for a number of things, including retriggering samples from\n"
	+ "frequency information (see sampler).\n"
	+ "Extra ports can be added to handle the 128 controller messages. Click\n"
	+ "add control, and you will get a menu window to pick the controller you\n"
	+ "need.\n"
	+ "The MidiPlugin also supports midi note output to external devices";
}

