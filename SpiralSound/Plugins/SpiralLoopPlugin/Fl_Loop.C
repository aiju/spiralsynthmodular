/*  LoopWidget
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

#include "Fl_Loop.h"
#include <iostream>
#include <FL/fl_draw.H>
#include <FL/x.H>
#include <math.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////

static const int   REZ   = 1000;
static const float RADCONV = 0.017453292;
static const int   INDW = 3; // indicator width
static const int   UPDATECYCLES = 4;

static const int   POSMARKER_MAX = 50;

Fl_Loop::Fl_Loop(int x, int y, int w, int h, const char* label) :
Fl_Group(x,y,w,h,label),
m_data(NULL),
m_MainWin(NULL),
m_Length(1000),
m_InnerRad((int)(w/2*0.75f)),
m_OuterRad((int)(w/2*0.95f)),
m_BorderRad(w/2),
m_IndSX(0),
m_IndSY(0),
m_IndEX(0),
m_IndEY(0),
m_StartAngle(0),
m_EndAngle(1),
m_MoveAngle(0.0f),
m_RangeStart(0),
m_RangeEnd(0),
m_Angle(0),
m_Pos(0),
m_Update(true),
m_StopUpdate(false),
m_WaveSize(1.0f),
m_Move(0),
m_LastMove(0),
m_Snap(false),
m_SnapDegrees(45),
m_PosMarkerCount(0),
cb_Move(NULL),
m_BGColour (FL_BLACK),
m_WaveColour (FL_GREEN),
m_SelColour (FL_WHITE),
m_IndColour (FL_BLUE),
m_MrkColour (FL_YELLOW)
{
	box(FL_NO_BOX);
	m_MidX=(w/2)+x;
	m_MidY=(h/2)+y-20;
}

void Fl_Loop::SetData(float const *set, const int Len)
{
	if (m_data!=NULL) delete[] m_data;
	m_data = new float[Len];
	memcpy((void*)m_data,(void*)set,Len*sizeof(float));

	SetLength(Len);
}

void Fl_Loop::SetLength(const int Len)
{
	m_Length=Len;

	// recalc start and end points
	m_RangeStart=(long)(m_StartAngle*(m_Length/360.0f));
	while (m_RangeStart < 0) m_RangeStart += m_Length;
	while (m_RangeStart > m_Length) m_RangeStart -= m_Length;
					
	m_RangeEnd=(long)(m_EndAngle*(m_Length/360.0f));
	while (m_RangeEnd < 0) m_RangeEnd += m_Length;
	while (m_RangeEnd > m_Length) m_RangeEnd -= m_Length;
}

void Fl_Loop::SelectAll()
{
	m_RangeStart=0;
	m_RangeEnd=m_Length;
}

void Fl_Loop::DrawWav()
{
	int Thickness=(m_OuterRad-m_InnerRad)/2;
	int n=0,X=0,Y=0,ox=0,oy=0, c=0;
	bool FirstTime=true, DrawnSnap=false;
	float Angle=0;
	float Sample=0;
	float SampleAngle=360.0f/(float)(REZ);
			
	fl_color (m_WaveColour);				
		
	while(m_Length>0 && n<m_Length)
	{
		n=(int)((Angle/360.0f)*m_Length);
	
		if (m_data)
		{
			Sample=m_data[n]*m_WaveSize;
			if (Sample>1) Sample=1;
			if (Sample<-1) Sample=-1;
		}
			
		Angle=c*SampleAngle;
		
		ox=X;
		oy=Y;
			
		float pos=Sample*Thickness+m_InnerRad+Thickness;
			
		X=(int)(m_MidX+x()+sin(Angle*RADCONV)*pos);
		Y=(int)(m_MidY+y()+cos(Angle*RADCONV)*pos);
			
		if (Angle>m_StartAngle && Angle<m_EndAngle)
		{
			// draw the selection indicator
			fl_color (m_SelColour);
		}
		else
		{
			fl_color (m_WaveColour);
		}
				
		if (!FirstTime) fl_line(X,Y,ox,oy);

		// draw the snap points
		if(m_SnapDegrees && (int)Angle%m_SnapDegrees==0)
		{
			if (!DrawnSnap)
			{
				fl_color (m_MrkColour);
				fl_line((int)(m_MidX+x()+sin(Angle*RADCONV)*m_InnerRad),
						(int)(m_MidY+y()+cos(Angle*RADCONV)*m_InnerRad),
						(int)(m_MidX+x()+sin(Angle*RADCONV)*m_OuterRad),
						(int)(m_MidY+y()+cos(Angle*RADCONV)*m_OuterRad));
				DrawnSnap=true;
			}
		}
		else
		{
			DrawnSnap=false;
		}
		
		c++;
		FirstTime=false;
	}
	/*XSetFunction(fl_display,fl_gc,GXxor);
	fl_line_style(FL_SOLID, 3, NULL);
	fl_line(m_IndSX,m_IndSY,m_IndEX,m_IndEY);
	fl_line_style(FL_SOLID, 1, NULL);
	XSetFunction(fl_display,fl_gc,GXcopy);	*/
}

void Fl_Loop::DrawPosMarker()
{	
	if (!m_Update) return;

	//Fl_Window* Last = current();
	
	//if (Last!=this && Last!=m_MainWin) return;
	//if (Last==m_MainWin) return;
	
	if (!visible() || !window()->visible() || !parent()->visible()) return;
	window()->make_current();

	float Angle=(m_Pos/m_Length)*360.0;
	fl_line_style(FL_SOLID, 3, NULL);
#if !__APPLE__
    XSetFunction(fl_display,fl_gc,GXxor);
#endif
	fl_line(m_IndSX,m_IndSY,m_IndEX,m_IndEY);
		
	fl_color (m_IndColour);
		
	m_IndSX=(int)(m_MidX+x()+sin(Angle*RADCONV)*m_InnerRad);
	m_IndSY=(int)(m_MidY+y()+cos(Angle*RADCONV)*m_InnerRad);
	m_IndEX=(int)(m_MidX+x()+sin(Angle*RADCONV)*m_OuterRad);
	m_IndEY=(int)(m_MidY+y()+cos(Angle*RADCONV)*m_OuterRad);
			
	fl_line(m_IndSX,m_IndSY,m_IndEX,m_IndEY);
	fl_line_style(FL_SOLID, 1, NULL);
#if !__APPLE__
	XSetFunction(fl_display,fl_gc,GXcopy);					
#endif
	
	if (m_PosMarkerCount>POSMARKER_MAX)
	{
		redraw();
		m_PosMarkerCount=0;
	}
	
	m_PosMarkerCount++;
	
	//Last->make_current();
}

void Fl_Loop::DrawWidgets()
{
	Fl_Group::draw();
}

void Fl_Loop::DrawEveryThing()
{
	if (damage() & (FL_DAMAGE_EXPOSE|FL_DAMAGE_ALL))
	{
		if (m_PosMarkerCount>POSMARKER_MAX)
		{
			m_PosMarkerCount=0;
		}

		//fl_color(color());
		//fl_rectf(x(),y(),w(),h());

		m_InnerRad-=5;
		m_OuterRad+=5;
		fl_color (m_BGColour);
		fl_pie(m_MidX+x()-m_OuterRad, m_MidY+y()-m_OuterRad, m_OuterRad*2, m_OuterRad*2, 0, 360);
		fl_color (color());
		fl_pie(m_MidX+x()-m_InnerRad, m_MidY+y()-m_InnerRad, m_InnerRad*2, m_InnerRad*2, 0, 360);
		m_OuterRad-=5;
		m_InnerRad+=5;	
		
		DrawWav();	
	}

	DrawWidgets();
}

void Fl_Loop::draw()
{	
	DrawEveryThing();
}

int Fl_Loop::handle(int event)
{
	static int LastButtonPushed=0;

	// call base
	if (!Fl_Group::handle(event))
	{
	switch (event) 
	{
  		case FL_PUSH:
			LastButtonPushed=Fl::event_button();
			// fall through
		case FL_DRAG:
    		{				
				int mx = Fl::event_x()-(m_MidX+x());
				int my = Fl::event_y()-(m_MidY+y());
				if (!mx && !my) break;
				    			
				double angle = 90+atan2((float)-my, (float)mx)*180/M_PI;
				while (angle < m_Angle-180) angle += 360;
				while (angle > m_Angle+180) angle -= 360;
				
				while (angle < 0) angle += 360;
				while (angle > 360) angle -= 360;
				
				m_Angle=angle;
				
				// snap
				if (m_Snap)
				{
					m_Angle-=(int)m_Angle%m_SnapDegrees;
				}
				
				if (LastButtonPushed==2)
				{
					m_Pos=m_Angle*(m_Length/360.0f);
						
					while (m_Pos < 0) m_Pos += m_Length;
					while (m_Pos > m_Length) m_Pos -= m_Length;
				}
				else if (LastButtonPushed==1)			
				{	
					switch (event) 
					{
  						case FL_PUSH:
						{					
							m_StartAngle=m_Angle;
							m_EndAngle=m_Angle;			
							redraw();
						}
						break;
							
						case FL_DRAG:    								
						{
							if (m_Angle>m_StartAngle)
							{
								m_EndAngle=m_Angle;
							}
							else
							{
								m_StartAngle=m_Angle;
							}
							redraw();
						}
						break;
					}
					
					// convert angle to sample data
					m_RangeStart=(long)(m_StartAngle*(m_Length/360.0f));
					while (m_RangeStart < 0) m_RangeStart += m_Length;
					while (m_RangeStart > m_Length) m_RangeStart -= m_Length;
					
					m_RangeEnd=(long)(m_EndAngle*(m_Length/360.0f));
					while (m_RangeEnd < 0) m_RangeEnd += m_Length;
					while (m_RangeEnd > m_Length) m_RangeEnd -= m_Length;
					
				}else if (LastButtonPushed==3)			
				{
				switch (event) 
					{
						case FL_PUSH:
						{
							m_MoveAngle=m_Angle;
							
							// reset the last
							// convert angle to sample data
							m_LastMove=(int)(m_MoveAngle*(m_Length/360.0f));
							while (m_LastMove < 0) m_LastMove += m_Length;
							while (m_LastMove > m_Length) m_Move -= m_Length;
						}
						break;
						
						case FL_DRAG:    								
						{
							m_MoveAngle=m_Angle;
							redraw();
						}
						break;
					}
					
					// convert angle to sample data
					m_Move=(int)(m_MoveAngle*(m_Length/360.0f));
					while (m_Move < 0) m_Move += m_Length;
					while (m_Move > m_Length) m_Move -= m_Length;
																
					// do the move
					if (cb_Move) cb_Move(this,m_LastMove-m_Move);
					
					m_LastMove=m_Move;
				}
			} 
			break;
		
		case FL_RELEASE:
			break;
		
		default:
			return 0;
  		}
	
	}
	return 1;
}
