/*  Canvas Widget
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

#include "FL/fl_draw.H"
#include "Fl_Canvas.h"
#include "Fl_DeviceGUI.h"
#include <iostream>
#include "../../SpiralSound/SpiralInfo.h"

// no of calls to handle when dragged, before the widget is redrawn
// to allow the wire (connection currently being made) to be redrawn
static const int UPDATE_TICKS = 5;

////////////////////////////////////////////////////////////////////////

Fl_Canvas::Fl_Canvas(int x, int y, int w, int h, char *name) :
Fl_Group(x,y,w,h,name),
cb_Connection(NULL),
cb_Unconnect(NULL),
cb_AddDevice(NULL),
m_ToolMenu(false),
m_UpdateTimer(0)
{
	m_IncompleteWire.OutputID=-1;
	m_IncompleteWire.OutputPort=-1;
	m_IncompleteWire.OutputTerminal=false;
	m_IncompleteWire.InputID=-1;
	m_IncompleteWire.InputPort=-1;
	m_IncompleteWire.InputTerminal=false;
	
	m_BG=NULL;
	m_BGData=NULL;
}

////////////////////////////////////////////////////////////////////////

Fl_Canvas::~Fl_Canvas()
{
}

////////////////////////////////////////////////////////////////////////

void Fl_Canvas::draw()
{			
	Fl_Widget*const* a = array();
	
  	if (damage() & ~FL_DAMAGE_CHILD) // redraw the entire thing:
	{ 	    
		if (m_BG)
		{
			int X=0,Y=0;
			while (Y<w())
			{	
				while (X<h())
				{
					m_BG->draw(parent()->x()+X,parent()->y()+Y);
#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION == 0
					X+=m_BG->w;
#else
					X+=m_BG->w();
#endif	
				}		
#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION == 0
 				Y+=m_BG->h;
#else
				Y+=m_BG->h();
#endif
				X=0;
			}
		}
		else
		{
			draw_box();
		}
				
		// draw minimised modules first
   	 	for (int i=children(); i--;) 
		{
   			Fl_Widget& o = **a++;
			if (((Fl_DeviceGUI*)&o)->IsMinimised())
			{
   	 			draw_child(o);
   				draw_outside_label(o);
			}
		}
		
		DrawWires();
		
		// draw maximised modules on top of everything else
   	 	Fl_Widget*const* a = array();
		for (int i=children(); i--;) 
		{
   			Fl_Widget& o = **a++;
			if (!((Fl_DeviceGUI*)&o)->IsMinimised())
			{
   	 			draw_child(o);
   				draw_outside_label(o);
			}
		}
	}
	else // only redraw the children that need it:
	{	
		for (int i=children(); i--;) update_child(**a++);
  	}
	
	if (m_ToolMenu)
	{
		int Pos=0,X=0,Y=0,textw,texth;
		int DegreesPerItem=30;
		float conv=3.151/180.0f;
		bool selected=false;
		m_Selected=-1;
		
		fl_font(fl_font(),10);
		for (vector< pair<string,int> >::iterator i=m_PluginNameList.begin();
			 i!=m_PluginNameList.end(); ++i)
		{								
			textw=0;
			fl_font(fl_font(),10);
			fl_measure(i->first.c_str(), textw, texth);
			
			X=m_x-(textw/2);
			Y=m_y-(m_PluginNameList.size()*5)+Pos*10;
						
			if (Fl::event_y()>Y-10 && Fl::event_y()<Y && 
				Fl::event_x()>X && Fl::event_x()<X+textw) 
			{
				fl_font(fl_font(),15);
				fl_measure(i->first.c_str(), textw, texth);
				X=m_x-(textw/2);
				m_Selected=i->second;
				selected=true;
			}
			else selected=false;
						
			fl_color(FL_GRAY);
						
			fl_color(FL_WHITE);
			fl_draw(i->first.c_str(),X-1,Y+1);

			if (selected) fl_color(FL_BLUE);	
			else fl_color(FL_BLACK);			

			fl_draw(i->first.c_str(),X,Y);			
			
			Pos+=1;	
		}
	}
}

////////////////////////////////////////////////////////////////////////

void Fl_Canvas::Poll()
{
	// bit of a workaround...	
	if (UserMakingConnection()) m_UpdateTimer++;
		
	if (m_UpdateTimer>UPDATE_TICKS) 
	{
		m_UpdateTimer=0;
		redraw();
	}
}

////////////////////////////////////////////////////////////////////////

void Fl_Canvas::DrawWires()
{
	for(vector<CanvasWire>::iterator i=m_WireVec.begin();
		i!=m_WireVec.end(); i++)
	{  
		Fl_DeviceGUI* SourceDevice = FindDevice(i->OutputID);
		Fl_DeviceGUI* DestDevice   = FindDevice(i->InputID);
			 
		if (!SourceDevice || !DestDevice) 
		{
			SpiralInfo::Alert("Cant find source or dest device while drawing wires");
			return;
		}

         Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (SourceDevice->GetPortType(i->OutputPort+SourceDevice->GetInfo()->NumInputs)) {
             case 0:     col = (Fl_Color) WIRE_COL0; break;
             case 1:     col = (Fl_Color) WIRE_COL1; break;
             case 2:     col = (Fl_Color) WIRE_COL2; break;
             case 3:     col = (Fl_Color) WIRE_COL3; break;
             case 4:     col = (Fl_Color) WIRE_COL4; break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		fl_color(col);

		fl_line(SourceDevice->GetPortX(i->OutputPort+SourceDevice->GetInfo()->NumInputs),
				SourceDevice->GetPortY(i->OutputPort+SourceDevice->GetInfo()->NumInputs),
				DestDevice->GetPortX(i->InputPort),
				DestDevice->GetPortY(i->InputPort));	
	}
	
	DrawIncompleteWire();
}

////////////////////////////////////////////////////////////////////////

bool Fl_Canvas::UserMakingConnection()
{
	return 	(m_IncompleteWire.InputID!=-1 || m_IncompleteWire.OutputID!=-1);
}

////////////////////////////////////////////////////////////////////////

void Fl_Canvas::DrawIncompleteWire()
{	
	// draw the wire we are currently connecting
	if(m_IncompleteWire.InputID!=-1)
	{
		Fl_DeviceGUI* Device = FindDevice(m_IncompleteWire.InputID);
		
		if (!Device) 
		{
			SpiralInfo::Alert("Cant find source or dest device while drawing wires");
			return;
		}
		
		
        Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (Device->GetPortType(m_IncompleteWire.InputPort)) {
             case 0:     col = (Fl_Color) WIRE_COL0; break;
             case 1:     col = (Fl_Color) WIRE_COL1; break;
             case 2:     col = (Fl_Color) WIRE_COL2; break;
             case 3:     col = (Fl_Color) WIRE_COL3; break;
             case 4:     col = (Fl_Color) WIRE_COL4; break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		fl_color(col);

		fl_line(Device->GetPortX(m_IncompleteWire.InputPort),
				Device->GetPortY(m_IncompleteWire.InputPort),
				Fl::event_x(),
				Fl::event_y());	
	}
	
	if(m_IncompleteWire.OutputID!=-1)
	{
		Fl_DeviceGUI* Device = FindDevice(m_IncompleteWire.OutputID);
		
		if (!Device) 
		{
			SpiralInfo::Alert("Cant find source or dest device while drawing wires");
			return;
		}
		
		
        Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (Device->GetPortType(m_IncompleteWire.OutputPort+Device->GetInfo()->NumInputs)) {
             case 0:     col = (Fl_Color) WIRE_COL0; break;
             case 1:     col = (Fl_Color) WIRE_COL1; break;
             case 2:     col = (Fl_Color) WIRE_COL2; break;
             case 3:     col = (Fl_Color) WIRE_COL3; break;
             case 4:     col = (Fl_Color) WIRE_COL4; break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		fl_color(col);

		fl_line(Device->GetPortX(m_IncompleteWire.OutputPort+Device->GetInfo()->NumInputs),
				Device->GetPortY(m_IncompleteWire.OutputPort+Device->GetInfo()->NumInputs),
				Fl::event_x(),
				Fl::event_y());	
	}
}

////////////////////////////////////////////////////////////////////////

void Fl_Canvas::ClearIncompleteWire()
{
	// Turn off both ports
	if (m_IncompleteWire.OutputID!=-1)
	{
		FindDevice(m_IncompleteWire.OutputID)->RemoveConnection(m_IncompleteWire.OutputPort+FindDevice(m_IncompleteWire.OutputID)->GetInfo()->NumInputs);
	}
	
	if (m_IncompleteWire.InputID!=-1)
	{
		FindDevice(m_IncompleteWire.InputID)->RemoveConnection(m_IncompleteWire.InputPort);
	}
	m_IncompleteWire.Clear();
}

////////////////////////////////////////////////////////////////////////

int Fl_Canvas::handle(int event)
{
	if (Fl_Group::handle(event)) return 1;
				
	if (event==FL_PUSH) 
	{
		ClearIncompleteWire();
		redraw();
		m_DragX=Fl::event_x();
		m_DragY=Fl::event_y();
	}
	
	if (Fl::event_button()==1 && event==FL_DRAG)
	{
		position(x()+(Fl::event_x()-m_DragX),y()+(Fl::event_y()-m_DragY));	
		m_DragX=Fl::event_x();
		m_DragY=Fl::event_y();	
		redraw();
	}
	
	if (Fl::event_button()==2)
	{
		if (event==FL_PUSH) 
		{	
			m_ToolMenu=true;
			m_x=Fl::event_x();
			m_y=Fl::event_y();
			redraw();
		}
		
		if (event==FL_DRAG)	redraw();
		
		if (event==FL_RELEASE) 
		{
			m_ToolMenu=false;
			if (m_Selected!=-1 && cb_AddDevice) 
			{
				int args[3];
				args[0]=m_Selected;
				args[1]=m_x;
				args[2]=m_y;
				cb_AddDevice(this,args);
			}
			redraw();
		}	
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////

void Fl_Canvas::PortClicked(Fl_DeviceGUI* Device, int Type, int Port, bool Value)
{
	if(Value) // Turned on the port
	{
		if(m_IncompleteWire.InputID==-1 || m_IncompleteWire.OutputID==-1)
		{
			if (Type==Fl_DeviceGUI::OUTPUT)
			{
				// make sure we don't make a output->output connection
				if (m_IncompleteWire.OutputID==-1)
				{	
					m_IncompleteWire.OutputPort=Port;
					m_IncompleteWire.OutputID=Device->GetID();
					m_IncompleteWire.OutputTerminal=Device->IsTerminal();
				}
				else
				{
					ClearIncompleteWire();
				}
			}
			else
			{
				// make sure we don't make a input->input connection
				if (m_IncompleteWire.InputID==-1)				
				{					
					m_IncompleteWire.InputPort=Port;
					m_IncompleteWire.InputID=Device->GetID();
					m_IncompleteWire.InputTerminal=Device->IsTerminal();
				}
				else
				{
					ClearIncompleteWire();
				}
			}

			// if both have now been set...
			if (m_IncompleteWire.InputID!=-1 && m_IncompleteWire.OutputID!=-1)
			{
				m_WireVec.push_back(m_IncompleteWire);	
	
				// send the connect callback
				cb_Connection(this,(void*)&m_IncompleteWire);
				m_Graph.AddConnection(m_IncompleteWire.OutputID,m_IncompleteWire.OutputTerminal,
									  m_IncompleteWire.InputID,m_IncompleteWire.InputTerminal);
				
				// Turn on both ports
				Fl_DeviceGUI* ODGUI = FindDevice(m_IncompleteWire.OutputID);
				ODGUI->AddConnection(m_IncompleteWire.OutputPort+ODGUI->GetInfo()->NumInputs);
					
				Fl_DeviceGUI* IDGUI = FindDevice(m_IncompleteWire.InputID);
				IDGUI->AddConnection(m_IncompleteWire.InputPort);
				
				m_IncompleteWire.Clear();
				
				redraw();
			}
		}
	}
	else // Turned off the port
	{	
		// Find connections using this port
		bool Found=true;
		
		while (Found)
		{
			Found=false;
			
			for(vector<CanvasWire>::iterator i=m_WireVec.begin();
				i!=m_WireVec.end(); i++)
			{
				if ((Type==Fl_DeviceGUI::OUTPUT && i->OutputID==Device->GetID() && i->OutputPort==Port) ||
					(Type==Fl_DeviceGUI::INPUT && i->InputID==Device->GetID() && i->InputPort==Port))
				{
					// Turn off both ports
					Fl_DeviceGUI* ODGUI = FindDevice(i->OutputID);
					ODGUI->RemoveConnection(i->OutputPort+ODGUI->GetInfo()->NumInputs);
					
					Fl_DeviceGUI* IDGUI = FindDevice(i->InputID);
					IDGUI->RemoveConnection(i->InputPort);
									
					// send the unconnect callback	
					cb_Unconnect(this,(void*)&(*i));
					m_Graph.RemoveConnection(i->OutputID,i->InputID);		

					// Remove the wire
					m_WireVec.erase(i);
					
					Found=true;
					break;
				}
			}
		}
		
		redraw();
		
		// Clear the current selection
		m_IncompleteWire.Clear();
	}
}

////////////////////////////////////////////////////////////////////////

void Fl_Canvas::ClearConnections(Fl_DeviceGUI* Device)
{
	bool removedall=false;
	
	//make sure we don't leave a dangling incomplete wire this will cause errors/seg-faults
	if (UserMakingConnection() && Device && ((Device->GetID() == m_IncompleteWire.OutputID) || 
		(Device->GetID() == m_IncompleteWire.InputID)))
	{
		ClearIncompleteWire();
	}	
	
	while (!removedall)
	{
		removedall=true;
		
		for (vector<CanvasWire>::iterator i=m_WireVec.begin();
			 i!=m_WireVec.end(); i++)
		{	
			if (i->OutputID==Device->GetID() ||
			    i->InputID==Device->GetID())
			{
				// Turn off both ports
				FindDevice(i->OutputID)->RemoveConnection(i->OutputPort+FindDevice(i->OutputID)->GetInfo()->NumInputs);
				FindDevice(i->InputID)->RemoveConnection(i->InputPort);

				// send the unconnect callback
				cb_Unconnect(this,(void*)&(*i));
				m_Graph.RemoveConnection(i->OutputID,i->InputID);
				
				m_WireVec.erase(i);
				removedall=false;
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
	
void Fl_Canvas::RemoveDevice(Fl_DeviceGUI* Device)
{
	ClearConnections(Device);
	remove(Device);
	redraw(); 
}

////////////////////////////////////////////////////////////////////////
	
void Fl_Canvas::Clear() 
{ 
	m_Graph.Clear();

	int kids=children();

	for(int n=0; n<kids; n++)
	{
		remove(child(0));		
	}	
	
	m_WireVec.clear(); 
	redraw(); 
}


////////////////////////////////////////////////////////////////////////

void Fl_Canvas::Rename(Fl_DeviceGUI* Device)
{
	if (cb_Rename) cb_Rename(this,Device);
}

////////////////////////////////////////////////////////////////////////

Fl_DeviceGUI *Fl_Canvas::FindDevice(int ID)
{
	for(int n=0; n<children(); n++)
	{
		if(((Fl_DeviceGUI*)child(n))->GetID()==ID)
		{
			return (Fl_DeviceGUI*)child(n);
		}
	}
	return NULL;
}
	
/////////////////////////////////////////////////////////////////////////

void Fl_Canvas::ToTop(Fl_DeviceGUI *o)
{
	if (children()<2) return; //no need to do anything

	// cast away the const :P
	Fl_Widget** a=(Fl_Widget**)array();
	int p=find(o);

	if (p<0) 
	{
		cerr<<"ToTop couldn't find widget!"<<endl;
		return;
	}
	
	Fl_Widget *temp=a[0];
	Fl_Widget *last=a[0];
	
	for(int n=1; n<children(); n++)
	{
		if (n>p) // after the widget in the list
		{	
			// move the widgets up
			a[n-1]=a[n];
		}
	}
	
	a[children()-1]=o; // put the raised one at the top of the list
}

void Fl_Canvas::ToBot(Fl_DeviceGUI *o)
{
	if (children()<2) return; //no need to do anything
	
	// cast away the const :P
	Fl_Widget** a=(Fl_Widget**)array();
	int p=find(o);

	if (p<0) 
	{
		cerr<<"ToBot couldn't find widget!"<<endl;
		return;
	}
	
	Fl_Widget *temp=a[0];
	Fl_Widget *last=a[0];
	
	for(int n=1; n<children(); n++)
	{
		if (n<=p) // before the widget in the list
		{	
			// move the widgets down
			temp=a[n];
			a[n]=last;
			last=temp;
		}
	}
	
	a[0]=o; // put the lowered one at the top of the list	
}

/////////////////////////////////////////////////////////////////////////
	
istream &operator>>(istream &s, Fl_Canvas &o)
{
	int NumWires;
	s>>NumWires;
	
	// my bad, didn't version this stream - remove one day...
	if (NumWires==-1)
	{
		int version;
		s>>version;
		s>>NumWires;
		
		for(int n=0; n<NumWires; n++)
		{
			CanvasWire NewWire;
			int dummy;
			
			s>>NewWire.OutputID;
			s>>dummy;
			s>>NewWire.OutputPort;
			s>>NewWire.OutputTerminal;
			s>>NewWire.InputID;
			s>>dummy;
			s>>NewWire.InputPort;	
			s>>NewWire.InputTerminal;
			
			// if we can turn on both ports
			if (o.FindDevice(NewWire.OutputID)->AddConnection(NewWire.OutputPort+
					o.FindDevice(NewWire.OutputID)->GetInfo()->NumInputs) &&
				o.FindDevice(NewWire.InputID)->AddConnection(NewWire.InputPort))
			{
				o.m_WireVec.push_back(NewWire);	

				// Notify connection by callback
				o.cb_Connection(&o,(void*)&NewWire);
				o.m_Graph.AddConnection(NewWire.OutputID,NewWire.OutputTerminal,NewWire.InputID,NewWire.InputTerminal);
			}				
		}
		
	}
	else
	{		
		for(int n=0; n<NumWires; n++)
		{
			CanvasWire NewWire;
			int dummy;
			
			s>>NewWire.OutputID;
			s>>dummy;
			s>>NewWire.OutputPort;
			s>>NewWire.InputID;
			s>>dummy;
			s>>NewWire.InputPort;	
		
			// if we can turn on both ports
			if (o.FindDevice(NewWire.OutputID)->AddConnection(NewWire.OutputPort+
					o.FindDevice(NewWire.OutputID)->GetInfo()->NumInputs) &&
				o.FindDevice(NewWire.InputID)->AddConnection(NewWire.InputPort))
			{
				o.m_WireVec.push_back(NewWire);	

				// Notify connection by callback
				o.cb_Connection(&o,(void*)&NewWire);
				o.m_Graph.AddConnection(NewWire.OutputID,false,NewWire.InputID,false);
			}					
		}
	}	
	return s;
}

////////////////////////////////////////////////////////////////////////

ostream &operator<<(ostream &s, Fl_Canvas &o)
{
	int version=0;
	s<<-1<<" "<<version<<" ";
	
	s<<o.m_WireVec.size()<<endl;

	for(vector<CanvasWire>::iterator i=o.m_WireVec.begin();
		i!=o.m_WireVec.end(); i++)
	{
		s<<i->OutputID<<" ";
		s<<0<<" ";
		s<<i->OutputPort<<" ";
		s<<i->OutputTerminal<<" ";
		s<<i->InputID<<" ";
		s<<0<<" ";
		s<<i->InputPort<<" ";	
		s<<i->InputTerminal<<endl;
	}

	return s;
}
