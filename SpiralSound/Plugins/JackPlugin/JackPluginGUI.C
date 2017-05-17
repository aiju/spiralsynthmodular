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

#include "JackPlugin.h"
#include "JackPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_file_chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <algorithm>

using namespace std;

////////////////////////////////////////////////////////////////////////

int OptionsList(const std::vector<string> &List)
{
	Fl_Double_Window *Win    = new Fl_Double_Window(300,300);
	Fl_Button *Ok            = new Fl_Button(10,275,40,20,"Ok");
	Ok->labelsize(10);
	Fl_Button *Cancel        = new Fl_Button(50,275,40,20,"Cancel");
	Cancel->labelsize(10);
	Fl_Hold_Browser* Browser = new Fl_Hold_Browser(5,5,290,265,"");

	for (std::vector<string>::const_iterator i = List.begin();
		 i!=List.end(); i++)
	{
		Browser->add(i->c_str());
	}
	
	Win->show();

	int Choice=-1;
	
	for (;;) 
	{
    	Fl::wait();
    	Fl_Widget* o = Fl::readqueue();
    	if (o==Ok || o==Browser) 
		{ 			
			Choice=Browser->value();
			Win->hide();
			delete Win;
			break; 
		}
		if (o==Cancel) 
		{ 
			Choice=-1; 
			Win->hide();
			delete Win;
			break; 
		}
		
		if (o==Win) break; 		
  	}
	
	return Choice;
}

////////////////////////////////////////////////////////////////////////

JackPluginGUI::JackPluginGUI(int w, int h,JackPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
        m_GUIColour = (Fl_Color)Info->GUI_COLOUR;
        m_JackPlugin =o;
        m_JackClient = o->GetJackClient();

	m_Indicator = new Fl_LED_Button(w/2 - 15,15,30,30,"");
	m_Indicator->value(0);
	m_Indicator->color(FL_RED);
        add(m_Indicator);

	m_Remove = new Fl_Button(5,15,25,25,"-");
        m_Remove->box (FL_PLASTIC_UP_BOX);
        m_Remove->color (m_GUIColour);
	m_Remove->type(0);
	m_Remove->labelsize(2);
        m_Remove->selection_color (m_GUIColour);
	m_Remove->callback((Fl_Callback*)cb_Remove, this);
        add(m_Remove);
	
	m_Add = new Fl_Button(30,15,25,25,"+");
        m_Add->box (FL_PLASTIC_UP_BOX);
        m_Add->color (m_GUIColour);
	m_Add->type(0);
	m_Add->labelsize(2);
        m_Add->selection_color (m_GUIColour);
	m_Add->callback((Fl_Callback*)cb_Add, this);
        add(m_Add);

	m_Attach = new Fl_Button(5,45,w - 10,20,"Attach");
        m_Attach->box (FL_PLASTIC_UP_BOX);
        m_Attach->color (m_GUIColour);
	m_Attach->type(0);
	m_Attach->labelsize(10);
        m_Attach->selection_color (m_GUIColour);
	m_Attach->callback((Fl_Callback*)cb_Attach, this);
        add(m_Attach);
	
	m_Detach = new Fl_Button(5,65,w - 10,20,"Detach");
        m_Detach->box (FL_PLASTIC_UP_BOX);
        m_Detach->color (m_GUIColour);
	m_Detach->type(0);
	m_Detach->labelsize(10);
        m_Detach->selection_color (m_GUIColour);
	m_Detach->callback((Fl_Callback*)cb_Detach, this);
        add(m_Detach);

	m_Scroll = new Fl_Scroll(5, 90, w - 10, h - 102);
	m_Scroll->box(FL_PLASTIC_DOWN_BOX);
	m_Scroll->type(Fl_Scroll::VERTICAL_ALWAYS);
        m_Scroll->position(0, 0);
        add(m_Scroll);

        m_OutputPack = new Fl_Pack(15, 90, 85, h - 102);
	m_Scroll->add(m_OutputPack);
	
        m_InputPack = new Fl_Pack(110, 90, 85, h - 102);
	m_Scroll->add(m_InputPack);
	
	for (int n=0; n<m_JackClient->GetJackInputCount(); n++)
	{
		AddOutput();
		AddInput();
	}
	
	end();
}

void JackPluginGUI::UpdateValues(SpiralPlugin *o)
{
	//To make sure buttons match ports on loading a patch
	if (! m_GUICH->GetBool("Connected"))
	{
		int i, numbuttons = (int) m_InputName.size(), numports = m_JackClient->GetJackInputCount();
		
		if (numbuttons > numports)
		{
			for (int i=numbuttons-numports; i > 0; i--)
			{
		        	RemoveOutput() ;		
		        	RemoveInput() ;	
		        }	
		} 
		
		if (numbuttons < numports)
		{
			for (int i=0; i < numports-numbuttons; i++)
			{
		        	AddOutput() ;		
		        	AddInput() ;	
		        }	
		} 
	}	
}

void JackPluginGUI::Update()
{
	if (m_GUICH->GetBool("Connected")) {
		m_JackClient->CheckingPortChanges = true;

		for (unsigned int n=0; n<m_JackClient->m_OutputPortsChanged.size(); n++) {
			m_JackClient->m_OutputPortsChanged[n]->Connected = jack_port_connected(m_JackClient->m_OutputPortsChanged[n]->Port);

			if (jack_port_connected(m_JackClient->m_OutputPortsChanged[n]->Port)) {
				if (m_JackClient->m_OutputPortsChanged[n]->ConnectedTo!="") {
					m_OutputButton[n]->label(m_JackClient->m_OutputPortsChanged[n]->ConnectedTo.c_str());
				}
				else
				{	
					const char** connections = jack_port_get_all_connections(m_JackClient->m_Client,m_JackClient->m_OutputPortsChanged[n]->Port);
  					if (connections) {
						m_OutputButton[m_JackClient->m_OutputPortsChanged[n]->PortNo]->label(connections[0]);
						free(connections);
					}  
				}	
				m_OutputButton[m_JackClient->m_OutputPortsChanged[n]->PortNo]->value(1);
			}
			else
			{
				m_OutputButton[m_JackClient->m_OutputPortsChanged[n]->PortNo]->value(0);
				m_OutputButton[m_JackClient->m_OutputPortsChanged[n]->PortNo]->label("None");
			}				
		}

		m_JackClient->m_OutputPortsChanged.clear();

		for (unsigned int n=0; n<m_JackClient->m_InputPortsChanged.size(); n++) {
			m_JackClient->m_InputPortsChanged[n]->Connected = jack_port_connected(m_JackClient->m_InputPortsChanged[n]->Port);

			if (m_JackClient->m_InputPortsChanged[n]->Connected) {
				if (m_JackClient->m_InputPortsChanged[n]->ConnectedTo!="") {
					m_InputButton[n]->label(m_JackClient->m_InputPortsChanged[n]->ConnectedTo.c_str());
				}
				else
				{	
					const char** connections = jack_port_get_all_connections(m_JackClient->m_Client,m_JackClient->m_InputPortsChanged[n]->Port);
  					if (connections) {
						m_InputButton[m_JackClient->m_InputPortsChanged[n]->PortNo]->label(connections[0]);
						free(connections);
					}  
				}	
				m_InputButton[m_JackClient->m_InputPortsChanged[n]->PortNo]->value(1);
			}
			else
			{
				m_InputButton[m_JackClient->m_InputPortsChanged[n]->PortNo]->value(0);
				m_InputButton[m_JackClient->m_InputPortsChanged[n]->PortNo]->label("None");
			}

		}

		m_JackClient->m_InputPortsChanged.clear();

		m_JackClient->CheckingPortChanges = false;
	}
	
	m_GUICH->SetCommand (JackPlugin::CHECK_PORT_CHANGES);

	m_Indicator->value(m_GUICH->GetBool("Connected"));
	redraw();	
}
	
//// Callbacks ////
void JackPluginGUI::RemoveOutput() {
        int n =(int) m_InputName.size() - 1;

	if (m_OutputName[n])
	{
		delete(m_OutputName[n]);
		m_OutputName[n] = NULL;
		m_OutputName.pop_back();
	}

	if (m_OutputLabel[n])
	{
		m_OutputPack->remove(m_OutputLabel[n]);
		delete(m_OutputLabel[n]);
                m_OutputLabel[n] = NULL;
		m_OutputLabel.pop_back();
		m_OutputPack->redraw();
		m_Scroll->redraw();
	}
        
	if (m_OutputButton[n])
	{
		m_OutputPack->remove(m_OutputButton[n]);
		delete(m_OutputButton[n]);
		m_OutputButton[n] = NULL;
		m_OutputButton.pop_back();
		m_OutputPack->redraw();
		m_Scroll->redraw();
	}
}

void JackPluginGUI::RemoveInput() {
        int n = (int) m_InputName.size() - 1;

	if (m_InputName[n])
	{
		delete(m_InputName[n]);
		m_InputName[n] = NULL;
		m_InputName.pop_back();
	}

	if (m_InputLabel[n])
	{
		m_InputPack->remove(m_InputLabel[n]);
		delete(m_InputLabel[n]);
                m_InputLabel[n] = NULL;
                m_InputLabel.pop_back();
		m_InputPack->redraw();
		m_Scroll->redraw();
	}
        
	if (m_InputButton[n])
	{
		m_InputPack->remove(m_InputButton[n]);
		delete(m_InputButton[n]);
		m_InputButton[n] = NULL;
		m_InputButton.pop_back();
		m_InputPack->redraw();
		m_Scroll->redraw();

	}
}

void JackPluginGUI::AddOutput() {
        int n = (int) m_OutputName.size();
        char *NewName = new char [256];
                
        sprintf(NewName,"Output %d",n);
        m_OutputName.push_back(NewName);
		
        m_OutputLabel.push_back(new Fl_Box(0,n*30,90,10,m_OutputName[n]));
        m_OutputLabel[n]->labelsize(8);
        m_OutputPack->add(m_OutputLabel[n]);

        m_OutputButton.push_back(new Fl_Button(0,n*30+10,90,20,"None"));
        m_OutputButton[n]->type(1);
        m_OutputButton[n]->labelsize(8);
        m_OutputButton[n]->callback((Fl_Callback*)cb_OutputConnect,this);
        m_OutputPack->add(m_OutputButton[n]);

	redraw();
	Fl::check();
}

void JackPluginGUI::AddInput() {
        int n = (int) m_InputName.size();
        char *NewName = new char [256];
                
        sprintf(NewName,"Input %d",n);
        m_InputName.push_back(NewName);
		
        m_InputLabel.push_back(new Fl_Box(95,n*30,90,10, m_InputName[n]));
        m_InputLabel[n]->labelsize(8);
        m_InputPack->add(m_InputLabel[n]);

        m_InputButton.push_back(new Fl_Button(95,n*30+10,90,20,"None"));
        m_InputButton[n]->type(1);
        m_InputButton[n]->labelsize(8);
        m_InputButton[n]->callback((Fl_Callback*)cb_InputConnect,this);
        m_InputPack->add(m_InputButton[n]);
        
	redraw();
	Fl::check();	
}

inline void  JackPluginGUI::cb_Remove_i(Fl_Button* o)
{ 
        int n = (int) m_InputName.size();

        if (n > MIN_PORTS)
        {
	        RemoveOutput() ;		
	        RemoveInput() ;	

		m_GUICH->Set ("NumInputs", n-1);
		m_GUICH->Set ("NumOutputs", n-1);
		m_GUICH->SetCommand (JackPlugin::SET_PORT_COUNT);
                m_GUICH->Wait ();
                
		/* removing connections live must be called directly from here in the GUI thread */
                if (m_GUICH->GetBool("Connected")) {
                	m_JackClient->RemoveInputPort(n-1);
                	m_JackClient->RemoveOutputPort(n-1);
                }
                
                if (n > 19) {
			resize (x(), y(), w(), h()-7);

			m_Indicator->resize (x()+w()/2 - 15,y()+15,30,30);
			m_Remove->resize (x()+5,y()+15,25,25);
			m_Add->resize (x()+30,y()+15,25,25);
			m_Attach->resize (x()+5,y()+45,w() - 10,20);
			m_Detach->resize (x()+5,y()+65,w() - 10,20);
			m_Scroll->resize (x()+5, y()+90, w() - 10, h() - 102);
			m_OutputPack->resize (x()+15, y()+90, 85, h() - 102);
			m_InputPack->resize (x()+110, y()+90, 85, h() - 102);
               }  
        }	
}

inline void  JackPluginGUI::cb_Add_i(Fl_Button* o)
{ 
        int n = (int) m_OutputName.size();
        
       if (n <= MAX_PORTS) 
       {
		/* Adding connections live must be called directly from here in the GUI thread */
                if (m_GUICH->GetBool("Connected")) {
                	m_JackClient->AddInputPort(n);
                	m_JackClient->AddOutputPort(n);
                }

		m_GUICH->Set ("NumInputs", n+1);
		m_GUICH->Set ("NumOutputs", n+1);
		m_GUICH->SetCommand (JackPlugin::SET_PORT_COUNT);
                m_GUICH->Wait ();
                
	        AddOutput() ;		
	        AddInput() ;	

		if (n > 20) {
			resize (x(), y(), w(), h()+12);

			m_Indicator->resize (x()+w()/2 - 15,y()+15,30,30);
			m_Remove->resize (x()+5,y()+15,25,25);
			m_Add->resize (x()+30,y()+15,25,25);
			m_Attach->resize (x()+5,y()+45,w() - 10,20);
			m_Detach->resize (x()+5,y()+65,w() - 10,20);
			m_Scroll->resize (x()+5, y()+90, w() - 10, h() - 102);
			m_OutputPack->resize (x()+15, y()+90, 85, h() - 102);
			m_InputPack->resize (x()+110, y()+90, 85, h() - 102);
		}	
       }	
}

inline void  JackPluginGUI::cb_Attach_i(Fl_Button* o)
{ 
	m_JackPlugin->Attach();
}

inline void  JackPluginGUI::cb_Detach_i(Fl_Button* o)
{
	for (int n=0; n<(int)m_OutputName.size(); n++)
	{		
		m_OutputButton[n]->value(false);
		m_OutputButton[n]->label("None");
	}
	
	for (int n=0; n<(int)m_InputName.size(); n++)
	{
		m_InputButton[n]->value(false);
		m_InputButton[n]->label("None");
	}

	m_JackPlugin->Detach();
}

inline void JackPluginGUI::cb_OutputConnect_i(Fl_Button* o)
{
	int index=0;
	std::vector<Fl_Button *>::iterator it = std::find( m_OutputButton.begin(), m_OutputButton.end(), o );

	if ( it != m_OutputButton.end() )
		index = std::distance( m_OutputButton.begin(), it );

	if ((o->value()) && m_GUICH->GetBool("Connected"))
	{
		m_GUICH->SetCommand(JackPlugin::UPDATE_NAMES);
		m_GUICH->Wait();

		// bit of a hack for multithreaded safety
		int ninputs=m_GUICH->GetInt("NumOutputPortNames");
		char inputs[MAX_PORTS][256];
		m_GUICH->GetData("InputPortNames",inputs);

		vector<string> Inputs;
		for (int n=0; n<ninputs; n++) Inputs.push_back(inputs[n]);
		int choice=OptionsList(Inputs);
			
		// connect this plugin's output to a jack input
		if (choice>0)
		{		
			m_JackClient->ConnectOutput(index,inputs[choice-1]);
			
			o->label(inputs[choice-1]);
			o->redraw();
		}
		else {
			o->label("None");
			o->value(0);
			o->redraw();
		}
	}
	else
	{
		m_JackClient->DisconnectOutput(index);
		o->label("None");
		o->value(0);
		o->redraw();
	}
}

inline void JackPluginGUI::cb_InputConnect_i(Fl_Button* o)
{ 
	int index=0;
	std::vector<Fl_Button *>::iterator it = std::find( m_InputButton.begin(), m_InputButton.end(), o );

	if ( it != m_InputButton.end() )
		index = std::distance( m_InputButton.begin(), it );

	if ((o->value()) && (m_JackClient) && (m_JackClient->IsAttached()))
	{
		m_GUICH->SetCommand(JackPlugin::UPDATE_NAMES);	
		m_GUICH->Wait();
		
		// bit of a hack for multithreaded safety
		int noutputs=m_GUICH->GetInt("NumOutputPortNames");
		char outputs[MAX_PORTS][256];
		m_GUICH->GetData("OutputPortNames",outputs);
			
		vector<string> Outputs;
		for (int n=0; n<noutputs; n++) Outputs.push_back(outputs[n]);
		int choice=OptionsList(Outputs);
	
		// connect this plugin's input to a jack output
		if (choice>0)
		{			
			m_JackClient->ConnectInput(index,outputs[choice-1]);
			
			o->label(outputs[choice-1]);
			o->redraw();
		}
		else {
			o->label("None");
			o->value(0);
			o->redraw();
		}
	}
	else
	{
		m_JackClient->DisconnectInput(index);
		o->label("None");
		o->value(0);
		o->redraw();
	}
}

const string JackPluginGUI::GetHelpText(const string &loc)
{
	return string("") 
		+ "JACK is the Jack Audio Connection Kit, and allows multiple Linux audio\n"
		+ "apps to be connected together and run simultaneously in a low latency.\n"
		+ "environment.\n\n"	
		+ "This plugin allows you to connect up to 64 inputs and outputs to other\n"
		+ "JACK apps (providing a server is running and your system can handle it)\n"
		+ "You can use the JackPlugin to connect the ports, or an external program\n"
		+ "such as the excellent qjackconnect app.\n\n"
		+ "When using JACK, make sure the buffer size and samplerate are set to\n"
		+ "match the JACK server, otherwise glitchy playback, and/or crashes may\n"
		+ "result";
}
