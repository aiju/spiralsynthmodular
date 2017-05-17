/*  SpiralPlugin
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

#include "SampleHoldPluginGUI.h"
#include <FL/fl_draw.H>
#include <FL/fl_draw.H>

using namespace std;

////////////////////////////////////////////

SampleHoldPluginGUI::SampleHoldPluginGUI(int w, int h,SampleHoldPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	end();
}



void SampleHoldPluginGUI::UpdateValues(SpiralPlugin *o)
{
}

const string SampleHoldPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "Holds the input value until the clock input is triggered.\n"
    + "Useful for wacky 60's computer sounds...\n"
    + "\n"
    + "Author: Yves Usson\n";
}	
