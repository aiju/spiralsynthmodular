/*  SpiralLoops
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
#include <FL/Fl_Double_Window.H>
#include <iostream>

#ifndef LOOPWIDGET
#define LOOPWIDGET

class Fl_Loop : public Fl_Group
{
public:
	Fl_Loop(int x, int y, int w, int h, const char* label=0);

	virtual void draw();
	virtual int  handle(int  event);

        void SetData(float const *set, const int Len);
	void SetLength(const int Len);
	int  GetLength() { return m_Length; }
	void SetPos(float p) {m_Pos=p;}
	void SetUpdate(bool p) {m_Update=p;}
	void StopUpdate(bool p) {m_StopUpdate=p;}
	void SetWaveSize(float s) {m_WaveSize=s;}
	void SetMainWin(Fl_Window *s) {m_MainWin=s;}
	void DrawWav();
	void DrawWidgets();
	void DrawEveryThing();
	void DrawPosMarker();
	void SetSnap(bool s) {m_Snap=s;}
	void SetSnapAngle(int s) { m_SnapDegrees=s; }
        void SetBGColour (unsigned c) { m_BGColour=(Fl_Color)c; }
        void SetWaveColour (unsigned c) { m_WaveColour=(Fl_Color)c; }
        void SetSelColour (unsigned c) { m_SelColour=(Fl_Color)c; }
        void SetIndColour (unsigned c) { m_IndColour=(Fl_Color)c; }
        void SetMrkColour (unsigned c) { m_MrkColour=(Fl_Color)c; }

	typedef void (cb)(Fl_Widget *, int);
	void SetMoveCallback(cb *cb_Move);

	void SelectAll();
	long GetRangeStart() { return m_RangeStart; }
	long GetRangeEnd()   { return m_RangeEnd; }

private:

	float const *m_data;

	Fl_Window *m_MainWin;

	int    m_Length;
	int    m_InnerRad;
	int    m_OuterRad;
	int    m_BorderRad;
	int    m_IndSX,m_IndSY,m_IndEX,m_IndEY;
	int    m_MidX,m_MidY;

	float  m_StartAngle;
	float  m_EndAngle;
	float  m_MoveAngle;
	long   m_RangeStart;
	long   m_RangeEnd;
	float  m_Angle;
	float  m_Pos;
	bool   m_Update;
	bool   m_StopUpdate;
	float  m_WaveSize;
	int    m_Move;
	int    m_LastMove;
	bool   m_Snap;
	int    m_SnapDegrees;
	int    m_PosMarkerCount;

	cb *cb_Move;

        Fl_Color m_BGColour, m_WaveColour, m_SelColour, m_IndColour, m_MrkColour;

};

#endif
