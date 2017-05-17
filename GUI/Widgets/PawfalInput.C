/*  PawfalInput Fltk Dialog
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
#include "PawfalInput.h"
#include <string.h>

bool Pawfal_Input(const char *a, const char *b, char *out)
{
	PawfalInput pi(300, 100,a,b);
	if (pi.go())
	{
                strcpy(out,pi.getText());
		return true;
	}
	
	return false;
}

PawfalInput::PawfalInput(int x, int y, int w, int h, const char *label, const char *dflt):
    Fl_Window(x,y,w, h),
    lbl(new Fl_Box(50, ((h-30)/2)-20, w-60,20, label)),
    input(new Fl_Input(50,((h-30)/2)+0,w-60,20)),
    ok(new Fl_Button(w-100, h-25, 30, 20, "ok")),
    cancel(new Fl_Button(w-60, h-25, 50, 20, "cancel")),
    keyhit(false)
{      
 
    lbl->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    lbl->labelfont(FL_SCREEN_BOLD);
    lbl->labelsize(12);
    
    ok->labelfont(FL_SCREEN_BOLD);
    ok->labelsize(12);
    cancel->labelfont(FL_SCREEN_BOLD);
    cancel->labelsize(12);

    input->value(dflt);
    input->textfont(FL_SCREEN_BOLD);
    input->textsize(12);
    logo = new Fl_Pixmap(tv_xpm);
    
    end();
    set_modal();
    //clear_border();
}

PawfalInput::PawfalInput(int w, int h, const char *label, const char *dflt):
    Fl_Window(w, h, "Enter text"),
    lbl(new Fl_Box(50, ((h-30)/2)-20, w-60,20, label)),
    input(new Fl_Input(50,((h-30)/2)+0,w-60,20)),
    ok(new Fl_Button(w-100, h-25, 30, 20, "ok")),
    cancel(new Fl_Button(w-60, h-25, 50, 20, "cancel")),
    keyhit(false)
{      
 
    lbl->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    lbl->labelfont(FL_SCREEN_BOLD);
    lbl->labelsize(12);
    
    ok->labelfont(FL_SCREEN_BOLD);
    ok->labelsize(12);
    cancel->labelfont(FL_SCREEN_BOLD);
    cancel->labelsize(12);

    input->value(dflt);
    input->textfont(FL_SCREEN_BOLD);
    input->textsize(12);
    logo = new Fl_Pixmap(tv_xpm);
    
    end();
    set_modal();
    //clear_border();
}

PawfalInput::~PawfalInput()
{
    delete logo;
}

int PawfalInput::handle(int e)
{
    int result = Fl_Window::handle(e);
    
    if (e==FL_KEYBOARD)
    {
        if (Fl::event_key()==FL_Escape||Fl::event_key()==FL_Enter)
            keyhit=true;
    }
    return result;
}

void PawfalInput::draw()
{
    Fl_Window::draw();
    logo->draw(10, 10);
}

void PawfalInput::ok_cb(Fl_Button *b, void *d)
{
    assert(b!=NULL);
    PawfalInput *ptr = (PawfalInput *)d;
    assert(ptr!=NULL);
}

void PawfalInput::cancel_cb(Fl_Button *b, void *d)
{
    assert(b!=NULL);
    PawfalInput *ptr = (PawfalInput *)d;
    assert(ptr!=NULL);
}

bool PawfalInput::go()
{    
    bool result = false;
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
            else if (o == ok) {result = true; break;}
            else if (o == cancel) {result = false; break;}
    }
    
    hide();
    return result;
}
