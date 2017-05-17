/*  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
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

#ifndef GUI_TYPE_H
#define GUI_TYPE_H

#include <FL/Fl_Group.H>

class Fl_DeviceGUI;

typedef void (SpiralGUISizeCB)(Fl_DeviceGUI *o);

class SpiralGUIType : public Fl_Group {
   public:
      SpiralGUIType (int x, int y, int w, int h, const char *label = 0);
      ~SpiralGUIType ();
      void SetResizeCallback (SpiralGUISizeCB *c, Fl_DeviceGUI *d) { m_ResizeSpiralGUI = c; m_DeviceGUI = d; }
   protected:
      void DoResizeCallback (void) { if (m_ResizeSpiralGUI) m_ResizeSpiralGUI(m_DeviceGUI); }
   private:
      SpiralGUISizeCB *m_ResizeSpiralGUI;
      Fl_DeviceGUI *m_DeviceGUI;
};

#endif
