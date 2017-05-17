/*  TriggerWidget
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
#include <FL/Fl_Widget.H>
#include <iostream>

#ifndef TRIGGERWIDGET
#define TRIGGERWIDGET

using namespace std;

class Fl_Trigger : public Fl_Widget
{
public:
	Fl_Trigger(int x, int y, int w, int h, const char* label=0);
	
	virtual void draw();
	virtual int  handle(int  event);
	
	void SetID(int s) {m_ID=s;}
	int  GetID() { return m_ID;}
	void SetChannel(int s) {m_Channel=s;}
	int  GetChannel() { return m_Channel;}
	void SetCentreX(int s) {m_CentreX=s;}
	void SetCentreY(int s) {m_CentreY=s;}
	void SetCentreRadius(int s) {m_CentreRadius=s;}
	void IncCount() 
	{
		m_Count++; if (m_Count>m_Passes) m_Count=0;
	}
	bool Completed() {return (m_Count==m_Passes); }
	
	float GetAngle() {return m_Angle;}
	void  SetAngle(float s) {m_Angle=s;}
	int   GetLoop() {return m_Loop;}
	int   GetPass() {return m_Passes;}
	void  SetSnap(bool s) {m_Snap=s;}
	void  SetSnapAngle(int s) {m_SnapDegrees=s;}
	
private:

	int m_ID;
	int m_Channel;
	int m_CentreX;
	int m_CentreY;
	int m_CentreRadius;

	bool m_Dragging;
	int  m_Loop;
	int  m_Passes;
	int  m_Count;
	
	float m_Angle;
	
	int  m_MaxLoops;
	int  m_MaxPasses;
	
	bool m_Snap;
	int  m_SnapDegrees;
	
	friend istream &operator>>(istream &s, Fl_Trigger &o);
	friend ostream &operator<<(ostream &s, Fl_Trigger &o);	
};

istream &operator>>(istream &s, Fl_Trigger &o);
ostream &operator<<(ostream &s, Fl_Trigger &o);

#endif
