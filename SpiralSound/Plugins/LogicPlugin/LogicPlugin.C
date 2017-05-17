/*  SpiralSound
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
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
#include <stdio.h>
#include "LogicPlugin.h"
#include "LogicPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

using namespace std;

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new LogicPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 52;
}

string SpiralPlugin_GetGroupName()
{
	return "Maths/Logic";
}
}

///////////////////////////////////////////////////////

LogicPlugin::LogicPlugin() :
m_Operator(AND)
{
        m_Version = 2;
        m_PluginInfo.Name="Logic";
	m_PluginInfo.Width=80;
	m_PluginInfo.Height=140;

        CreatePorts ();
	m_AudioCH->Register("Operator",(int*)&m_Operator);
        m_AudioCH->Register ("Inputs", &m_Inputs);

}

LogicPlugin::~LogicPlugin()
{
}

PluginInfo &LogicPlugin::Initialise(const HostInfo *Host)
{
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *LogicPlugin::CreateGUI()
{
	return new LogicPluginGUI(m_PluginInfo.Width,
						     m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void LogicPlugin::Execute (void) {
    float Freq=0, OldFreq=0;
    for (int n=0; n<m_HostInfo->BUFSIZE; n++) {
        switch (m_Operator) {
          case NOT: // Only Uses Input 1
               if (GetInput(0,n)>0) SetOutput(0,n,-1.0f);
               else SetOutput(0,n,1.0f);
               break;
          case XOR: // Only uses inputs 1 and 2
               if ((GetInput(0,n)>0 || GetInput(1,n)>0) && !(GetInput(0,n)>0 && GetInput(1,n)>0))
                  SetOutput(0,n,1.0f);
               else SetOutput(0,n,-1.0f);
               break;
          case XNOR: // Only uses inputs 1 and 2
               if ((GetInput(0,n)>0 && GetInput(1,n)>0) || (!(GetInput(0,n)>0) && !(GetInput(1,n)>0)))
                  SetOutput(0,n,1.0f);
               else SetOutput(0,n,-1.0f);
               break;
          default: // Uses all available inputs
               int true_val, give_up;
               switch (m_Operator) {
                      case AND:  true_val =  1; give_up = -1; break;
                      case OR:   true_val =  1; give_up =  1; break;
                      case NAND: true_val = -1; give_up =  1; break;
                      case NOR : true_val = -1; give_up = -1; break;
                      default:   true_val =  0; give_up =  0; break;
               }
               int result = 0;
               for (int i=0; i<m_PluginInfo.NumInputs; i++) {
                   if (GetInput (i, n) > 0) result = true_val; else result = -true_val;
                   if (result == give_up) break;
               }
               SetOutput (0, n, (float)result);
               break;
        }
    }
}


void LogicPlugin::ExecuteCommands() {
  if (m_AudioCH->IsCommandWaiting ()) {
    switch (m_AudioCH->GetCommand()) {
      case SETINPUTS:
        SetInputs (m_Inputs);
        break;
    }
  }
}

void LogicPlugin::SetInputs (int n) {
  // once to clear the connections with the current info
  // do we need this????
  UpdatePluginInfoWithHost();
  // Things can get a bit confused deleting and adding inputs
  // so we just chuck away all the ports...
  RemoveAllInputs ();
  RemoveAllOutputs ();
  m_PluginInfo.NumInputs = 0;
  m_PluginInfo.NumOutputs = 0;
  m_PluginInfo.PortTips.clear ();
  // ... and then create some new ones
  CreatePorts (n, true);
  // do the actual update
  UpdatePluginInfoWithHost ();
}

void LogicPlugin::CreatePorts (int n, bool AddPorts) {
  int c;
  m_PluginInfo.NumInputs = n;
  m_Inputs = n;
  char t[256];
  for (c=1; c<=n; c++) {
    sprintf (t, "Input %d", c);
    m_PluginInfo.PortTips.push_back (t);
  }
  m_PluginInfo.NumOutputs = 1;
  m_PluginInfo.PortTips.push_back ("Output");
  if (AddPorts) {
    for (c=0; c<m_PluginInfo.NumInputs; c++) AddInput();
    for (c=0; c<m_PluginInfo.NumOutputs; c++) AddOutput();
  }
}

void LogicPlugin::StreamOut(ostream &s)
{
	s << m_Version << " " << m_PluginInfo.NumInputs << " " << m_Operator;
}

void LogicPlugin::StreamIn(istream &s) {
  int version, datum;
  s >> version;
  switch (version) {
    case 1:
      s >> datum; // Version 1 saved a constant that is not used now
      SetInputs (2);
      break;
    case 2:
      s >> datum;
      SetInputs (datum);
      s >> datum;
      m_Operator = (OperatorType)datum;
      break;
  }
}
