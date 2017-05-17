/*  DeviceGUI composite Widget
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

#include "Fl_DeviceGUI.h"
#include "Fl_Canvas.h"
#include "Fl_Canvas.h"
#include "PawfalInput.h"
#include "../../SpiralSound/SpiralInfo.h"

int Fl_DeviceGUI::Numbers[512];

Fl_PortButton::Fl_PortButton(int x, int y, int w, int h, char *n) :
Fl_Button(x,y,w,h,n)
{
	m_ConnectionCount=0;
}

int Fl_PortButton::handle(int event)
{
	if (event==FL_PUSH)
	{
		m_LastButton=Fl::event_button();

		if (m_LastButton == 1)
		{
			if (m_Type==INPUT && value()) return 1;
			do_callback();
			return 1;
		}

		if (m_LastButton == 3)
		{
			do_callback();
			return 1;
		}
	}

	return 1;
}

Fl_DeviceGUI::Fl_DeviceGUI(const DeviceGUIInfo& Info, SpiralGUIType *PW, Fl_Pixmap *Icon, bool Terminal) :
Fl_Group (Info.XPos, Info.YPos, Info.Width+(PortGroupWidth*2), Info.Height+TitleBarHeight, ""),
m_PluginWindow (NULL),
m_Icon (NULL),
m_Name (Info.Name),
m_ID (-1),
m_DelMe (false),
m_IsTerminal (Terminal),
m_Minimised (true)
{
	for (int n=0; n<512; n++) Numbers[n]=n;

	type(1);
	box((Fl_Boxtype)SpiralInfo::GUIDEVICE_Box);
    labeltype(FL_ENGRAVED_LABEL);
    align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	color(SpiralInfo::GUICOL_Device);

	m_Icon=Icon;
	m_MiniWidth=w();
	m_MiniHeight=h();

	m_DragBar = new Fl_DragBar(Info.XPos, Info.YPos, Info.Width+PortGroupWidth*2, TitleBarHeight, m_Name.c_str());
	m_DragBar->labelsize(10);
	m_DragBar->type(Fl_DragBar::FLDRAG);
	m_DragBar->color(SpiralInfo::GUICOL_Device);

	m_Menu = new Fl_Menu_Button(x(),y(),w(),h(),"");
	m_Menu->type(Fl_Menu_Button::POPUP3);
	m_Menu->textsize(8);

	m_Menu->add("rename", 0, (Fl_Callback*)cb_Rename,this);
	m_Menu->add("delete", 0, (Fl_Callback*)cb_Delete,this);

	int Centx=x()+w()/2;
	int Centy=y()+h()/2;

	if (m_Icon)
	{
		m_IconButton = new Fl_Button(Centx-m_Icon->w()/2,Centy-m_Icon->h()/2,m_Icon->w(),m_Icon->h());
		m_IconButton->box(FL_NO_BOX);
	    m_IconButton->image(m_Icon);
	}
	else
	{
		m_IconButton = NULL;
	}

	m_PluginWindow = PW;
	if (m_PluginWindow) {
           m_PluginWindow->hide();
           add (m_PluginWindow);
           m_PluginWindow->SetResizeCallback (&cb_Resize, this);
	}
	resizable(NULL);
	//Add the input/output ports
	Setup(Info, true);
}

void Fl_DeviceGUI::Clear()
{
	end();
}

inline void Fl_DeviceGUI::cb_Resize_i (void) {
     if (m_PluginWindow && m_PluginWindow->visible() && !m_DelMe && !m_Minimised)
        ResizeToPluginWindow();
}

void Fl_DeviceGUI::cb_Resize (Fl_DeviceGUI *o) {
     o->cb_Resize_i();
}


int Fl_DeviceGUI::handle (int event) {
    int t=Fl_Group::handle(event);
    // Click on icon in minimised device - Maximise, hide icon
    if (m_IconButton && m_IconButton->value()) {
       m_IconButton->value (false);
       if (m_PluginWindow && !m_DelMe) {
          if (!m_PluginWindow->visible()) Maximise();
       }
    }
    // plugin GUI killed - minimise device, show icon
    if (!m_Minimised && !m_PluginWindow->visible()) {
       Minimise();
       if (m_IconButton) m_IconButton->show();
    }
    return 1;
}

void Fl_DeviceGUI::ResizeToPluginWindow (void) {
     if (m_PluginWindow->h()+2 > m_MiniHeight)
        Resize (m_PluginWindow->w()+(PortGroupWidth*2)-5, m_PluginWindow->h()+2);
     else
        Resize (m_PluginWindow->w()+(PortGroupWidth*2)-5, m_MiniHeight);
}

void Fl_DeviceGUI::Minimise() {
     m_Minimised = true;
     Resize (m_MiniWidth, m_MiniHeight);
}

void Fl_DeviceGUI::Maximise() {
     m_Minimised=false;
     m_PluginWindow->show();
     m_IconButton->hide();
     ResizeToPluginWindow();
     ((Fl_Canvas*)parent())->ToTop(this);
}

void Fl_DeviceGUI::Resize (int width, int height) {
     int oldw = w();
     int oldh = h();
     size (width, height);
     m_DragBar->size (width, m_DragBar->h());
     for (int n=m_Info.NumInputs; n<(int)m_PortVec.size(); n++)
         m_PortVec[n]->position (x()+width-8, m_PortVec[n]->y());
     position (x()+(oldw-w())/2, y()+(oldh-h())/2);
     m_Menu->resize (x(), y(), width, height);
     int Centx = x()+w()/2;
     int Centy = y()+h()/2;
     m_IconButton->position (Centx-m_Icon->w()/2, Centy-m_Icon->h()/2);
     parent()->redraw();
}

void Fl_DeviceGUI::Setup(const DeviceGUIInfo& Info, bool FirstTime)
{
	m_Info=Info;

	// Remove all current connections - it's the safest thing to do.
	if (parent() && !FirstTime)
	{
		((Fl_Canvas*)(parent()))->ClearConnections(this);
	}

	// delete the current ports
	for(vector<Fl_PortButton*>::iterator i=m_PortVec.begin();
			i!=m_PortVec.end(); i++)
	{
		remove(*i);
		delete(*i);
	}

	m_PortVec.clear();

	int InputX=x()+2;
	int OutputX=0;
	int StartY=y()+TitleBarHeight;
	int PortDist=10;
	int PortNum=0;

	m_MiniHeight=m_Info.Height+TitleBarHeight;
	bool Maximised = (m_PluginWindow && m_PluginWindow->visible());
	if (!Maximised)
	{
		h(m_MiniHeight);
		OutputX=x()+PortGroupWidth+m_Info.Width+4;
	}
	else
	{
		OutputX=x()+w()-8;
	}

	for (int n=0; n<m_Info.NumInputs; n++)
	{
		Fl_PortButton* NewInput = new Fl_PortButton(InputX,StartY+PortDist*n,PortSize,PortSize,"");
		NewInput->type(1);
		NewInput->SetType(Fl_PortButton::INPUT);
		NewInput->value(false);
		NewInput->box(FL_ROUNDED_BOX);

        Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (m_Info.PortTypes[n]) {
             case 0:     col = (Fl_Color) WIRE_COL0;
                         break;
             case 1:     col = (Fl_Color) WIRE_COL1;
                         break;
             case 2:     col = (Fl_Color) WIRE_COL2;
                         break;
             case 3:     col = (Fl_Color) WIRE_COL3;
                         break;
             case 4:     col = (Fl_Color) WIRE_COL4;
                         break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		NewInput->selection_color(col);

		NewInput->down_box(FL_ROUNDED_BOX);
		NewInput->tooltip(m_Info.PortTips[n].c_str());
		NewInput->callback((Fl_Callback*)cb_Port,(void*)&Numbers[PortNum]);
		m_PortVec.push_back(NewInput);
		add(NewInput);
		PortNum++;
	}

	for (int n=0; n<m_Info.NumOutputs; n++)
	{
		Fl_PortButton* NewOutput= NewOutput = new Fl_PortButton(OutputX,StartY+PortDist*n,PortSize,PortSize,"");
		NewOutput->type(1);
		NewOutput->SetType(Fl_PortButton::OUTPUT);
		NewOutput->value(false);
		NewOutput->box(FL_ROUNDED_BOX);

        Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (m_Info.PortTypes[n+m_Info.NumInputs]) {
             case 0:     col = (Fl_Color) WIRE_COL0;
                         break;
             case 1:     col = (Fl_Color) WIRE_COL1;
                         break;
             case 2:     col = (Fl_Color) WIRE_COL2;
                         break;
             case 3:     col = (Fl_Color) WIRE_COL3;
                         break;
             case 4:     col = (Fl_Color) WIRE_COL4;
                         break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		NewOutput->selection_color(col);

		NewOutput->down_box(FL_ROUNDED_BOX);
		NewOutput->tooltip(m_Info.PortTips[n+m_Info.NumInputs].c_str());
		NewOutput->callback((Fl_Callback*)cb_Port,(void*)&Numbers[PortNum]);
		m_PortVec.push_back(NewOutput);
		add(NewOutput);
		PortNum++;
	}
}

bool Fl_DeviceGUI::AddConnection(int n)
{
	if ( n < (int)m_PortVec.size() )
	{
		m_PortVec[n]->Add();
		m_PortVec[n]->value(1);
		redraw();
		return true;
	}
	return false;
}

void Fl_DeviceGUI::RemoveConnection(int n)
{
	m_PortVec[n]->Remove();
	if (!m_PortVec[n]->GetCount())
	{
		m_PortVec[n]->value(0);
		redraw();
	}
}

inline void Fl_DeviceGUI::cb_Port_i(Fl_Button* o, void* v)
{
	int Port=*(int*)(v);
	Fl_PortButton *PortButton = (Fl_PortButton *)o;
	PortType Pt;

	if (m_DelMe) return;

	// Find out if this is an input or an output.
	if (Port<m_Info.NumInputs)
	{
		Pt=INPUT;
	}
	else
	{
		Pt=OUTPUT;
		Port-=m_Info.NumInputs;
	}

	if (PortButton->GetLastButton()==1)
	{
		((Fl_Canvas*)(parent()))->PortClicked(this,Pt,Port,1);
	}
	else
	{
		((Fl_Canvas*)(parent()))->PortClicked(this,Pt,Port,0);
	}

}
void Fl_DeviceGUI::cb_Port(Fl_Button* o, void* v)
{((Fl_DeviceGUI*)(o->parent()))->cb_Port_i(o,v);}

inline void Fl_DeviceGUI::cb_Rename_i(Fl_Menu_Button* o, void* v)
{
	char name[256];
	if (Pawfal_Input("Rename the module:",m_DragBar->label(),name))
	{
		m_Name=name;
		m_DragBar->label(m_Name.c_str());
		((Fl_Canvas*)(parent()))->Rename(this);
	}
}
void Fl_DeviceGUI::cb_Rename(Fl_Menu_Button* o, void* v)
{((Fl_DeviceGUI*)(o->parent()))->cb_Rename_i(o,v);}

inline void Fl_DeviceGUI::cb_Delete_i(Fl_Menu_Button* o, void* v)
{
	m_DelMe=true;
}
void Fl_DeviceGUI::cb_Delete(Fl_Menu_Button* o, void* v)
{((Fl_DeviceGUI*)(o->parent()))->cb_Delete_i(o,v);}
