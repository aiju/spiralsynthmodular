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

#include <stdio.h>
#include "ControllerPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

ControllerPluginGUI::CVGUI::CVGUI(int n, ControllerPluginGUI *p, Fl_Color SelColour)
{
	m_SliderGroup = new Fl_Group(0,0,60,153,"");
	//m_SliderGroup->box (FL_PLASTIC_UP_BOX);
	m_SliderGroup->user_data((void *)p);

	m_Title = new Fl_Input(5,2,50,15,"");
	m_Title->value("Name");
	m_Title->textsize(10);
	m_Title->callback((Fl_Callback*)ControllerPluginGUI::cb_Title,
		(void*)&Numbers[n]);
	m_SliderGroup->add(m_Title);

	m_Max = new Fl_Input(5,18,50,15,"");
	char t[64];
	sprintf(t,"%.6f",1.0f);
	m_Max->value(t);
	m_Max->textsize(10);
	m_Max->callback((Fl_Callback*)ControllerPluginGUI::cb_Max,
		(void*)&Numbers[n]);
	m_SliderGroup->add(m_Max);

	m_Chan = new Fl_Slider(20, 34, 20, 100, "");
	m_Chan->type (FL_VERT_NICE_SLIDER);
        m_Chan->box (FL_PLASTIC_DOWN_BOX);
	m_Chan->selection_color (SelColour);
	m_Chan->maximum(1);
	m_Chan->step(0.01);
	m_Chan->value(0.5);
   	m_Chan->callback((Fl_Callback*)ControllerPluginGUI::cb_Chan,
		(void*)&Numbers[n]);
	m_SliderGroup->add(m_Chan);

	m_Min = new Fl_Input(5,136,50,15,"");
	char t2[64];
	sprintf(t2,"%.6f",-1.0f);
	m_Min->value(t2);
	m_Min->textsize(10);
	m_Min->callback((Fl_Callback*)ControllerPluginGUI::cb_Min,
		(void*)&Numbers[n]);
	m_SliderGroup->add(m_Min);
}

////////////////////////////////////////////

ControllerPluginGUI::ControllerPluginGUI(int w, int h,ControllerPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
m_CVCount(0)
{
        for (int n=0; n<MAX_CHANNELS; n++)
	{
		Numbers[n]=n;
	}

        m_GUIColour = (Fl_Color)Info->GUI_COLOUR;
	m_MainPack = new Fl_Pack(0,20,w,h-44);
	m_MainPack->type(FL_HORIZONTAL);
        add (m_MainPack);

	// start with four...
	AddCV();
	AddCV();
	AddCV();
	AddCV();

        m_Buttons = new Fl_Pack (0, h-22, 45, 20);
	m_Buttons->type (FL_HORIZONTAL);
        add (m_Buttons);
        m_Delete = new Fl_Button(2, 0, 20, 20, "-");
        m_Delete->box (FL_PLASTIC_UP_BOX);
        m_Delete->color (m_GUIColour);
	m_Delete->callback ((Fl_Callback*)cb_Delete);
	m_Buttons->add (m_Delete);
        m_Add = new Fl_Button (24, 0, 20, 20, "+");
        m_Add->box (FL_PLASTIC_UP_BOX);
        m_Add->color (m_GUIColour);
	m_Add->callback ((Fl_Callback*)cb_Add);
	m_Buttons->add (m_Add);

}

void ControllerPluginGUI::AddCV() {
     CVGUI *NewCV = new CVGUI (m_CVCount, this, m_GUIColour);
     m_GUIVec.push_back (NewCV);
     m_MainPack->add (NewCV->m_SliderGroup);
     m_CVCount++;
}

void ControllerPluginGUI::DeleteCV() {
     vector<CVGUI*>::iterator i=m_GUIVec.end();
     i--;
     m_MainPack->remove ((*i)->m_SliderGroup);
     delete *i;
     m_GUIVec.erase (i);
     m_CVCount--;
}

void ControllerPluginGUI::Clear() {
     for (vector<ControllerPluginGUI::CVGUI*>::iterator i=m_GUIVec.begin(); i!=m_GUIVec.end(); i++) {
         m_MainPack->remove ((*i)->m_SliderGroup);
         delete *i;
     }
     m_GUIVec.clear();
     m_CVCount = 0;
}

void ControllerPluginGUI::UpdateValues (SpiralPlugin *o) {
     ControllerPlugin *Plugin = (ControllerPlugin *)o;
     int c;
     float min, max, val;
     string Title, Min, Max;
     char temp[64];
     Clear();
     c=Plugin->GetNum();
     for (int n=0; n<c; n++) {
         AddCV();
         m_GUIVec[n]->m_Title->value (Plugin->GetName (n).c_str());
         min = Plugin->GetMin (n);
         sprintf(temp, "%.6f", min);
         m_GUIVec[n]->m_Min->value (temp);
         max = Plugin->GetMax (n);
         sprintf (temp, "%.6f", max);
         m_GUIVec[n]->m_Max->value (temp);
         // Scale and/or invert value to match slider range (0->1)
         float val = 1.0f - (Plugin->GetVal(n) - min) / (max - min);
         m_GUIVec[n]->m_Chan->value (val);
     }
     Resize (c*60, h());
}

inline void ControllerPluginGUI::cb_Title_i(Fl_Input* o, void* v)
{
	int num=*(int*)(v);
	char temp[256];
	sprintf(temp,"%s",m_GUIVec[num]->m_Title->value());
	m_GUICH->Set("Number",num);
	m_GUICH->SetData("Name",(void*)temp);
	m_GUICH->SetCommand(ControllerPlugin::SETNAME);
}
void ControllerPluginGUI::cb_Title(Fl_Input* o, void* v)
{ ((ControllerPluginGUI*)(o->parent()->user_data()))->cb_Title_i(o,v);}

inline void ControllerPluginGUI::cb_Max_i(Fl_Input* o, void* v)
{
	int num=*(int*)(v);

	float min = atof(m_GUIVec[num]->m_Min->value());
	float max = atof(m_GUIVec[num]->m_Max->value());
	if (min > max) {
	// Swap values if arse over tit...
		float temp = min;
		char t[64];

		min = max;
		max = temp;

		sprintf(t,"%.6f",min);
		m_GUIVec[num]->m_Min->value(t);
		sprintf(t,"%.6f",max);
		m_GUIVec[num]->m_Max->value(t);
	}

	m_GUICH->Set("Number",num);
	m_GUICH->Set("Max",max);
	m_GUICH->SetCommand(ControllerPlugin::SETMAX);
}
void ControllerPluginGUI::cb_Max(Fl_Input* o, void* v)
{ ((ControllerPluginGUI*)(o->parent()->user_data()))->cb_Max_i(o,v);}

inline void ControllerPluginGUI::cb_Chan_i(Fl_Slider* o, void* v)
{
	int num=*(int*)(v);
	// swap em over, cos it's the easiqest way to reverse
	// the fltk slider, which is upside down imho
	float min = atof(m_GUIVec[num]->m_Min->value());
	float max = atof(m_GUIVec[num]->m_Max->value());
	float val = (1.0f-o->value())*(max-min)+min;
	m_GUICH->Set("Number",num);
	m_GUICH->Set("Value",val);

	m_GUICH->SetCommand(ControllerPlugin::SETCHANNEL);
}
void ControllerPluginGUI::cb_Chan(Fl_Slider* o, void* v)
{ ((ControllerPluginGUI*)(o->parent()->user_data()))->cb_Chan_i(o,v);}

inline void ControllerPluginGUI::cb_Min_i(Fl_Input* o, void* v)
{
	int num=*(int*)(v);

	float min = atof(m_GUIVec[num]->m_Min->value());
	float max = atof(m_GUIVec[num]->m_Max->value());
	if (min > max) {
	// Swap values if arse over tit...
		float temp = min;
		char t[64];

		min = max;
		max = temp;

		sprintf(t,"%.6f",min);
		m_GUIVec[num]->m_Min->value(t);
		sprintf(t,"%.6f",max);
		m_GUIVec[num]->m_Max->value(t);
	}
	m_GUICH->Set("Number",num);
	m_GUICH->Set("Min",min);
	m_GUICH->SetCommand(ControllerPlugin::SETMIN);
}
void ControllerPluginGUI::cb_Min(Fl_Input* o, void* v)
{ ((ControllerPluginGUI*)(o->parent()->user_data()))->cb_Min_i(o,v);}

inline void ControllerPluginGUI::cb_Add_i (Fl_Button* o, void* v) {
   if (m_CVCount < MAX_CHANNELS) {
      AddCV();
      int num = (int)m_GUIVec.size();
      float min = atof (m_GUIVec[num - 1]->m_Min->value());
      float max = atof (m_GUIVec[num - 1]->m_Max->value());
      float val = (1.0f-o->value()) * (max-min) + min;
      char temp[256];
      sprintf (temp,"%s",m_GUIVec[num - 1]->m_Title->value());
      m_GUICH->Set ("Number", num);
      m_GUICH->SetCommand (ControllerPlugin::SETNUM);
      m_GUICH->Wait ();
      m_GUICH->Set ("Number", num);
      m_GUICH->SetData ("Name", (void*)temp);
      m_GUICH->Set ("Max", max);
      m_GUICH->Set ("Value", val);
      m_GUICH->Set ("Min", min);
      m_GUICH->SetCommand (ControllerPlugin::SETALL);
      m_GUICH->Wait();
      Resize (w()+60, h());
   }
}

void ControllerPluginGUI::cb_Add (Fl_Button* o, void* v) {
     ((ControllerPluginGUI*)(o->parent()->parent()))->cb_Add_i (o, v);
}

inline void ControllerPluginGUI::cb_Delete_i (Fl_Button* o, void* v) {
   if (m_GUIVec.size()>1) {
      DeleteCV();
      m_GUICH->Set ("Number", (int)m_GUIVec.size());
      m_GUICH->SetCommand (ControllerPlugin::SETNUM);
      m_GUICH->Wait();
      Resize (w()-60, h());
   }
}

void ControllerPluginGUI::cb_Delete (Fl_Button* o, void* v) {
     ((ControllerPluginGUI*)(o->parent()->parent()))->cb_Delete_i (o, v);
}

const string ControllerPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "This is a simple plugin to allow you to generate CV values\n"
    + "interatively with sliders in the plugin window. Useful if you\n"
    + "can't use Midi, or for controlling LADSPA plugins. The slider\n"
    + "ranges can be set, and titles can be given to each slider.\n"
    + "You can add or delete sliders from the plugin using the\n"
    + "+ or - buttons.\n";
}

/*
// call for version <3
istream &operator>>(istream &s, ControllerPluginGUI &o)
{
	int c;
	string Title,Min,Max;
	float Val;

	o.Clear();

	s>>c;
	for (int n=0; n<c; n++)
	{
		s>>Title>>Min>>Max>>Val;
		o.AddCV();
		o.m_GUIVec[n]->m_Title->value(Title.c_str());
		o.m_GUIVec[n]->m_Min->value(Min.c_str());
		o.m_GUIVec[n]->m_Max->value(Max.c_str());
		o.m_GUIVec[n]->m_Chan->value(Val);
	}

	o.resize (o.x(), o.y(), c*60, o.h());
	return s;
}


// call for version >=3
// another version of the stream to fix the old style string streaming problem
void ControllerPluginGUI::StreamIn(istream &s)
{
	int c,version,size;
	string Title,Min,Max;
	float Val;
	char Buf[4096];

	Clear();
	s>>version;
	s>>c;
	for (int n=0; n<c; n++)
	{
		AddCV();

		s>>size;
		s.ignore(1);
		s.get(Buf,size+1);
		m_GUIVec[n]->m_Title->value(Buf);

		s>>Min>>Max>>Val;

		m_GUIVec[n]->m_Min->value(Min.c_str());
		m_GUIVec[n]->m_Max->value(Max.c_str());
		m_GUIVec[n]->m_Chan->value(Val);
	}

	resize(x(),y(),c*60,h());
}

void ControllerPluginGUI::StreamOut(ostream &s)
{
	// version
	s<<1<<endl;
	s<<m_GUIVec.size()<<endl;
	for (vector<ControllerPluginGUI::CVGUI*>::iterator i=m_GUIVec.begin();
		 i!=m_GUIVec.end(); i++)
	{
		s<<strlen((*i)->m_Title->value())<<endl;
		s<<(*i)->m_Title->value()<<" ";
		s<<(*i)->m_Min->value()<<" ";
		s<<(*i)->m_Max->value()<<" ";
		s<<(*i)->m_Chan->value()<<endl;
	}
}
*/
