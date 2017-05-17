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

#include <FL/Fl_Group.h>
#include <FL/Fl_Output.h>
#include <FL/Fl_Image.h>
#include <vector>
#include <string>
#include "../../GraphSort.h"

#ifndef CANVAS_WIDGET
#define CANVAS_WIDGET

using namespace std;

class Fl_DeviceGUI;

class CanvasWire
{
public:
	CanvasWire() { Clear(); }
	
	void Clear() 
	{
		OutputPort=-1;
		OutputID=-1;
		OutputTerminal=false;
		InputPort=-1;
		InputID=-1;
		InputTerminal=false;
		DelMe=false;
	}

	int OutputID;
	int OutputPort;
	bool OutputTerminal;
	int InputID;
	int InputPort;
	bool InputTerminal;
	bool DelMe;
};

class Fl_Canvas : public Fl_Group
{
public:
	Fl_Canvas(int x, int y, int w, int h, char *name);
	~Fl_Canvas();
	
	virtual void draw();
	virtual int handle(int event);
	void PortClicked(Fl_DeviceGUI* Device, int Type, int Port, bool Value);
	void Rename(Fl_DeviceGUI* Device);

	void SetConnectionCallback(Fl_Callback* s) { cb_Connection=s; }
	void SetUnconnectCallback(Fl_Callback* s) { cb_Unconnect=s; }
	void SetAddDeviceCallback(Fl_Callback* s) { cb_AddDevice=s; }
	void SetRenameCallback(Fl_Callback* s) { cb_Rename=s; }
	
	void ClearConnections(Fl_DeviceGUI* Device);
	void RemoveDevice(Fl_DeviceGUI* Device);
	void Clear();
	void AddPluginName(const string &s, int ID) 
		{ m_PluginNameList.push_back(pair<string,int>(s,ID)); }
	GraphSort* GetGraph() { return &m_Graph; }
	
	void Poll();

	void ToTop(Fl_DeviceGUI *o);
	void ToBot(Fl_DeviceGUI *o);

private:

	void DrawWires();
	void ClearIncompleteWire();
	void DrawIncompleteWire();
	bool UserMakingConnection();
	Fl_DeviceGUI *FindDevice(int ID);

	Fl_Image  *m_BG;
	char      *m_BGData;
	
	void (*cb_Connection)(Fl_Widget*, void*);
	void (*cb_Unconnect)(Fl_Widget*, void*);
	void (*cb_AddDevice)(Fl_Widget*, void*);
	void (*cb_Rename)(Fl_Widget*, void*);
	
	vector<CanvasWire> m_WireVec;
	CanvasWire         m_IncompleteWire;
	
	bool m_ToolMenu, m_ButtonDown;
	int  m_x,m_y,m_Selected;
	vector< pair<string,int> > m_PluginNameList;

	GraphSort m_Graph;
	int m_UpdateTimer;
	int m_DragX,m_DragY;

	friend istream &operator>>(istream &s, Fl_Canvas &o);
	friend ostream &operator<<(ostream &s, Fl_Canvas &o);	
};

istream &operator>>(istream &s, Fl_Canvas &o);
ostream &operator<<(ostream &s, Fl_Canvas &o);

#endif
