/*  CommentGUI composite Widget
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

#ifndef COMMENTGUI
#define COMMENTGUI

#include <FL/Fl_Input.h>
#include "Fl_DeviceGUI.h"

class Fl_CommentGUI : public Fl_DeviceGUI
{
public:
	Fl_CommentGUI(const DeviceGUIInfo& Info, SpiralGUIType *PW, Fl_Pixmap *Icon);
	virtual int  handle(int event);
	virtual void Setup(const DeviceGUIInfo& Info, bool FirstTime = false);
	virtual void Clear();
	virtual void	    StreamOut(ostream &s);
	virtual void	    StreamIn(istream &s);
protected:
	string m_Comment;
private:
        void NewText(void);
};

#endif
