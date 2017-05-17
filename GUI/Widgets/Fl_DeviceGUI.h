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

#ifndef DEVICEGUI
#define DEVICEGUI

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.h>
#include <FL/Fl_Menu_Button.h>
#include <FL/Fl_Box.h>
#include "Fl_DragBar.H"
#include "SpiralGUI.H"
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>

#define WIRE_COL0 91
#define WIRE_COL1 FL_GREEN
#define WIRE_COL2 FL_YELLOW
#define WIRE_COL3 FL_BLUE
#define WIRE_COL4 FL_CYAN

using namespace std;

static const int TitleBarHeight = 12;
static const int PortGroupWidth = 12;
static const int PortSize=6;

class Fl_PortButton : public Fl_Button {
   public:
	enum Type {INPUT,OUTPUT};

	Fl_PortButton(int x, int y, int w, int h, char *n);
	virtual ~Fl_PortButton() {};
	void SetType(Type s) { m_Type=s; }
	virtual int handle(int event);

	void Add()      { m_ConnectionCount++; }
	void Remove()   { if (m_ConnectionCount>0) m_ConnectionCount--; }
	int  GetCount() { return m_ConnectionCount; }
	int  GetLastButton() { return m_LastButton; }
   private:
	Type m_Type;
	int  m_ConnectionCount;
	int  m_LastButton;
};

struct DeviceGUIInfo
{
	int XPos;
	int YPos;
	int Width;
	int Height;
	int NumInputs;
	int NumOutputs;
	vector<string> PortTips;
	string Name;
	vector<int> PortTypes;
};

class Fl_DeviceGUI : public Fl_Group {
   public:
      Fl_DeviceGUI (const DeviceGUIInfo& Info, SpiralGUIType *PW, Fl_Pixmap *Icon, bool Terminal=false);
      virtual int handle (int event);
      enum PortType {INPUT,OUTPUT};
      int GetID() { return m_ID; }
      void SetID (int s) { m_ID=s; /*DisplayID(s);*/ }
      bool Killed() { return m_DelMe; }
      int GetPortX (int n) { return m_PortVec[n]->x()+PortSize/2; }
      int GetPortY (int n) { return m_PortVec[n]->y()+PortSize/2; }
      // aesthetic, to keep track of number of connections to know whether to
      // draw the port as occupied or not.
      bool AddConnection (int n);
      void RemoveConnection (int n);
      bool GetPortValue (int n) { return m_PortVec[n]->value(); }
      const DeviceGUIInfo* GetInfo() { return &m_Info; }
      SpiralGUIType* GetPluginWindow() { return m_PluginWindow; }
      string GetName() { return m_Name; }
      void SetName (const string &s) { m_Name=s; m_DragBar->label (m_Name.c_str()); }
      bool IsMinimised() { return m_Minimised; }
      void Minimise();
      void Maximise();
      // automatically called from the constructor, but may be redone at any time.
      virtual void Setup (const DeviceGUIInfo& Info, bool FirstTime = false);
      virtual void Clear();
      int GetPortType (int n) { return m_Info.PortTypes[n]; }
      // do we belong to a plugin that is an output?
      bool IsTerminal() { return m_IsTerminal; }
   protected:
      DeviceGUIInfo m_Info;
      Fl_DragBar* m_DragBar;
      SpiralGUIType* m_PluginWindow;
      Fl_Pixmap*  m_Icon;
      Fl_Button*  m_IconButton;
      Fl_Menu_Button* m_Menu;
   private:
      void ResizeToPluginWindow (void);
      void Resize (int width, int height);
      inline void cb_Resize_i (void);
      static void cb_Resize (Fl_DeviceGUI *o);
      inline void cb_Port_i (Fl_Button* o, void* v);
      static void cb_Port (Fl_Button* o, void* v);
      inline void cb_Rename_i (Fl_Menu_Button* o, void* v);
      static void cb_Rename (Fl_Menu_Button* o, void* v);
      inline void cb_Delete_i (Fl_Menu_Button* o, void* v);
      static void cb_Delete (Fl_Menu_Button* o, void* v);
      vector<Fl_PortButton*> m_PortVec;
      static int Numbers[512];
      string m_Name;
      int m_ID, m_MiniWidth, m_MiniHeight;
      bool m_DelMe, m_IsTerminal, m_Minimised;
};

#endif
