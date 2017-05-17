/*  SpiralPlugin
 *  Copyleft (C) 2003 Andy Preston <andy@clublinux.co.uk>
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

#include "TransposePluginGUI.h"
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

TransposePluginGUI::TransposePluginGUI (int w, int h,TransposePlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI (w, h, o, ch)
{
     m_Amount = new Fl_Counter (15, 20, 50, 20, "Amount");
     m_Amount->color (Info->GUI_COLOUR);
     m_Amount->type (FL_SIMPLE_COUNTER);
     m_Amount->box (FL_PLASTIC_UP_BOX);
     m_Amount->color (Info->GUI_COLOUR);
     m_Amount->textsize (10);
     m_Amount->labelsize (10);
     m_Amount->step (1);
     m_Amount->lstep (1);
     m_Amount->minimum (-12);
     m_Amount->maximum (12);
     m_Amount->value (0);
     m_Amount->callback ((Fl_Callback*) cb_Amount);
     add (m_Amount);
     end ();
}

void TransposePluginGUI::UpdateValues (SpiralPlugin *o) {
     TransposePlugin *Plugin = (TransposePlugin *)o;
     m_Amount->value (Plugin->GetAmount ());
}

inline void TransposePluginGUI::cb_Amount_i (Fl_Counter* o, void* v) {
    m_GUICH->Set ("Amount", int (o->value ()));
}

void TransposePluginGUI::cb_Amount (Fl_Counter* o, void* v) {
    ((TransposePluginGUI*)(o->parent ()))->cb_Amount_i (o, v);
}

const string TransposePluginGUI::GetHelpText (const string &loc) {
    return string("")
    + "The first input connects to a note CV from (e.g.) a Matrix.\n"
    + "The second input can be connected to (e.g.) a Keyboard, to\n"
    + "transpose the sequence up by the number of notes indicated\n"
    + "by the key.\n\n"
    + "If you use both inputs the 'Amount' value is ignored.\n"
    + "With only one input connected the 'Amount' value lets you\n"
    + "transpose the input up or down by the indicated number of\n"
    + "notes.";
}
