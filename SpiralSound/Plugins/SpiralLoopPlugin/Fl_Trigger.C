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

#include "Fl_Trigger.h"
#include <iostream>
#include <FL/fl_draw.H>
#include <math.h>
#include <stdio.h>
#include "Fl_Loop.h"

/////////////////////////////////////////////////////////////////////////////

static const float RADCONV = 0.017453292;

Fl_Trigger::Fl_Trigger(int x, int y, int w, int h, const char* label) : 
Fl_Widget(x,y,w,h,label),
m_ID(0),
m_Channel(0),
m_CentreX(0),
m_CentreY(0),
m_CentreRadius(0),
m_Dragging(false),
m_Loop(0),
m_Passes(0),
m_Count(0),
m_Angle(0),
m_MaxLoops(10),
m_MaxPasses(10),
m_Snap(false),
m_SnapDegrees(45)
{
}

void Fl_Trigger::draw()
{	
	x(parent()->x()+(int)((sin(m_Angle*RADCONV)*m_CentreRadius)+m_CentreX-w()/2));
	y(parent()->y()+(int)((cos(m_Angle*RADCONV)*m_CentreRadius)+m_CentreY-h()/2));						
	
	fl_font(fl_font(),8);
						
	fl_color(255,255,255);
	fl_arc(x(), y(), w(), h(), 0, 360);
 	
	int cx=x()+w()/2;
	int cy=y()+h()/2;
	
	char text[32];
	//sprintf(text,"%d",m_Loop);
	//fl_draw(text, cx-2, cy-h()+5); 

	//sprintf(text,"%d",m_Passes);
	//fl_draw(text, cx+w()-5, cy+3);
	
	sprintf(text,"%d",m_Channel);
	fl_draw(text, cx, cy);
	
	//char t[32];
	//sprintf(t,"%d",m_Count);
	//fl_draw(t, cx-2, cy-h()+5);
}

int Fl_Trigger::handle(int  event)
{
	static int LastButtonPushed=0;

	// call base
	if (!Fl_Widget::handle(event))
	{
	int ww,hh;
	
	ww = w();
	hh = h();
	
	int mx = Fl::event_x();
	int my = Fl::event_y();
	
	switch (event) 
	{
  		case FL_PUSH:
			LastButtonPushed=Fl::event_button();			
			
			if (LastButtonPushed==1)			
			{
				m_Dragging=true;
			}
			
			if (LastButtonPushed==3)			
			{
				m_Channel++;
				if (m_Channel>7)
				{
					m_Channel=0;
				}
				redraw();
				Fl_Loop *p=(Fl_Loop*)parent();
				p->DrawEveryThing();
				p->redraw();
			}
			
			// fall through
			
		case FL_DRAG:
    		{				
				if (LastButtonPushed==2)
				{
					
				}
				else if (LastButtonPushed==1)			
				{	
					if (m_Dragging)
					{
						int px = mx-(m_CentreX+parent()->x());
						int py = my-(m_CentreY+parent()->y());
						
						double angle = 90+atan2((float)-py, (float)px)*180/M_PI;
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
															
						redraw();
					}					
				}
				else if (LastButtonPushed==3)			
				{
				
				}
			} 
			break;
		
		case FL_RELEASE:
			{
				m_Dragging=false;
				Fl_Loop *p=(Fl_Loop*)parent();
				p->DrawEveryThing();
				p->redraw();
				do_callback();
			}
			break;
		
		default:
			return 0;
  		}
	
	}
	return 1;
}

istream &operator>>(istream &s, Fl_Trigger &o)
{	
	s>>o.m_CentreX>>o.m_CentreY>>o.m_CentreRadius>>o.m_Dragging>>o.m_ID>>o.m_Passes>>
		o.m_Count>>o.m_Angle>>o.m_MaxLoops>>o.m_MaxPasses;
		
	return s;
}

ostream &operator<<(ostream &s, Fl_Trigger &o)
{
	s<<o.m_CentreX<<" "<<o.m_CentreY<<" "<<o.m_CentreRadius<<" "<<o.m_Dragging<<" "<<
		o.m_ID<<" "<<o.m_Passes<<" "<<o.m_Count<<" "<<o.m_Angle<<" "<<
		o.m_MaxLoops<<" "<<o.m_MaxPasses<<" ";
		
	return s;
}
