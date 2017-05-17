/*  PawfalYesNo Fltk Dialog
 *  Copyleft (C) 2001 Dan Bethell <dan@pawfal.org>
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
#include "PawfalYesNo.h"
#include <FL/fl_draw.H>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

bool Pawfal_YesNo(const char *a,...)
{  
	va_list ap;
	va_start(ap, a);
	char buffer[1024];
	if (!strcmp(a,"%s")) {
	    strcpy(buffer,va_arg(ap, const char*));
	} else {
	    //: matt: MacOS provides two equally named vsnprintf's...
	    ::vsnprintf(buffer, 1024, a, ap);
	}
	va_end(ap);
#if !__APPLE__
	PawfalYesNo pi(300, 100,buffer);
	if (pi.go()) return true;
#else
	Str255 copy;
	strncpy((char*)copy + 1, buffer, 253);
	copy[0] = strlen((char*)copy+1);
	AlertStdAlertParamRec	rec = { true, false, NULL, 
		(StringPtr)-1, (StringPtr)-1, NULL, 
		kAlertStdAlertOKButton, 0, kWindowAlertPositionParentWindowScreen };
	SInt16					ret = 0;
	StandardAlert(kAlertCautionAlert, copy, NULL, &rec, &ret);
	return ret == kAlertStdAlertOKButton;
#endif
	return false;
}

PawfalYesNo::PawfalYesNo(int w, int h, const char *label):
    Fl_Window(w,h,"Question"),
    lbl(new Fl_Box(50, 15, w-60,20, label)),
    keyhit(false),
    result(false)
{      
    lbl->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    lbl->labelfont(FL_SCREEN_BOLD);
    lbl->labelsize(12);
	
	int	message_w, message_h;
    fl_font(FL_SCREEN_BOLD, 12);
  	message_w = message_h = 0;
  	fl_measure(label, message_w, message_h);
	resize(x(),y(),message_w+60,h);

	w=message_w+60;

    yes = new Fl_Button(w-120, h-25, 50, 20, "yes");
    no = new Fl_Button(w-60, h-25, 50, 20, "no");

    yes->labelfont(FL_SCREEN_BOLD);
    yes->labelsize(12);
    no->labelfont(FL_SCREEN_BOLD);
    no->labelsize(12);

    logo = new Fl_Pixmap(tv_xpm);
    
    end();
    set_modal();
    //clear_border();
}

PawfalYesNo::~PawfalYesNo()
{
    delete logo;
}

int PawfalYesNo::handle(int e)
{
    int result = Fl_Window::handle(e);
        
    if (e==FL_KEYBOARD)
    {
        if (Fl::event_key()==FL_Escape||Fl::event_key()==FL_Enter)
            keyhit=true;
    }
    return result;
}

void PawfalYesNo::draw()
{
    Fl_Window::draw();
    logo->draw(10, 10);
}

void PawfalYesNo::ok_cb(Fl_Button *b, void *d)
{
    assert(b!=NULL);
    PawfalYesNo *ptr = (PawfalYesNo *)d;
    assert(ptr!=NULL);
}

void PawfalYesNo::cancel_cb(Fl_Button *b, void *d)
{
    assert(b!=NULL);
    PawfalYesNo *ptr = (PawfalYesNo *)d;
    assert(ptr!=NULL);
}

bool PawfalYesNo::go()
{    
    result = false;
    show();
    
    for (;;) 
    {
        Fl_Widget *o = Fl::readqueue();
        if (!o&&!keyhit) Fl::wait();
            else if (keyhit)
            {
                int key = Fl::event_key();
        
                if (key==FL_Escape)
                {
                    result = false;
                    break; 
                }
        
                if (key==FL_Enter)
                {
                    result=true;
                    break;                    
                }
            }
            else if (o == yes) {result = true; break;}
            else if (o == no) {result = false; break;}
    }
    
    hide();
    return result;
}
