/*  LADSPAPluginGUI.C
 *  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
 *  LADSPA Plugin by Nicolas Noble <nicolas@nobis-crew.org>
 *  Modified by Mike Rawes <myk@waxfrenzy.org>
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

#include <cstdio>
#include <cmath>
#include <cstring>
#include <dlfcn.h>
#include <vector>
#include <algorithm>

#include <FL/fl_draw.H>
#include <FL/Fl_Tooltip.H>

#include "LADSPAPluginGUI.h"
#include "LADSPAInfo.h"

using namespace std;

LADSPAPluginGUI::LADSPAPluginGUI(int w, int h,
                                 LADSPAPlugin *o,
                                 ChannelHandler *ch,
                                 const HostInfo *Info,
                                 const vector<LADSPAInfo::PluginEntry> &PVec) :
SpiralPluginGUI(w,h,o,ch)
{
	m_GUIColour = (Fl_Color)Info->GUI_COLOUR;
        m_GUIBox = (Fl_Boxtype)Info->GUIDEVICE_Box;
	m_PluginList = PVec;

// Get maximum input port count
	m_GUICH->GetData("GetMaxInputPortCount",&(m_MaxInputPortCount));

// Set up buffers for data transfer via ChannelHandler
	m_InputPortNames = (char *)malloc(256 * m_MaxInputPortCount);
	m_InputPortSettings = (PortSetting *)malloc(sizeof(PortSetting) * m_MaxInputPortCount);
	m_InputPortValues = (PortValue *)calloc(m_MaxInputPortCount, sizeof(PortValue));
	m_InputPortDefaults = (float *)calloc(m_MaxInputPortCount, sizeof(float));

	if (!(m_InputPortNames && m_InputPortSettings &&
	      m_InputPortValues && m_InputPortDefaults)) {
		cerr<<"LADSPA Plugin (GUI): Memory allocation error\n"<<endl;
	}

	m_InputPortCount = 0;
	m_UnconnectedInputs = 0;
	m_PortIndex = 0;

// Set up widgets
	m_BKnob = new Fl_Button(5,15,50,20,"Knobs");
	m_BKnob->labelsize (10);
	m_BKnob->type(FL_TOGGLE_BUTTON);
	m_BKnob->box(m_GUIBox);
	m_BKnob->color(Info->GUI_COLOUR);
	m_BKnob->selection_color(m_GUIColour);
	m_BKnob->callback((Fl_Callback *)cb_BKnob);
	add(m_BKnob);

	m_BSlider = new Fl_Button(60,15,50,20,"Sliders");
	m_BSlider->labelsize (10);
	m_BSlider->type(FL_TOGGLE_BUTTON);
	m_BSlider->box(m_GUIBox);
	m_BSlider->color(m_GUIColour);
	m_BSlider->selection_color(m_GUIColour);
	m_BSlider->callback((Fl_Callback *)cb_BSlider);
	add(m_BSlider);

	m_BSetup = new Fl_Button(w - 55,15,50,20,"Setup...");
	m_BSetup->labelsize (10);
	m_BSetup->type(FL_TOGGLE_BUTTON);
	m_BSetup->box(m_GUIBox);
	m_BSetup->color(m_GUIColour);
	m_BSetup->selection_color(m_GUIColour);
	m_BSetup->callback((Fl_Callback *)cb_BSetup);
	add(m_BSetup);

	m_KnobGroup = new Fl_Group(5, 35, w - 10, h - 40, "");
	add(m_KnobGroup);

	m_SliderGroup = new Fl_Group(5, 35, 490, 275, "");
	add(m_SliderGroup);

	m_SetupGroup = new Fl_Group(5, 35, 490, 275, "");

	m_NameLabel = new Fl_Box(10,45,480,15,"None");
	m_NameLabel->align(FL_ALIGN_LEFT|FL_ALIGN_CLIP|FL_ALIGN_INSIDE);
	m_NameLabel->labelsize(12);
	m_SetupGroup->add(m_NameLabel);

	m_MakerLabel = new Fl_Box(10,65,480,15,"None");
	m_MakerLabel->align(FL_ALIGN_LEFT|FL_ALIGN_CLIP|FL_ALIGN_INSIDE);
	m_MakerLabel->labelsize(12);
	m_SetupGroup->add(m_MakerLabel);

	m_Browser = new Fl_Choice(50, 90, 440, 22,"Plugin:");
	m_Browser->box(m_GUIBox);
	m_Browser->labelsize(12);
	m_Browser->textsize(12);
	m_Browser->callback((Fl_Callback *)cb_Select);

	m_Browser->add("(None)");
	m_PluginIDLookup.push_back(0);

// The plugin list is already formatted for addition to our drop-down,
// so just add all items as they are
	unsigned long size = m_Browser->size();
	int depth = 1;

	for (vector<LADSPAInfo::PluginEntry>::iterator i=m_PluginList.begin();
	     i!=m_PluginList.end(); i++)
	{
		m_Browser->add(i->Name.c_str());

		unsigned int dsize = m_Browser->size() - size;
		int ddepth = i->Depth - depth;

		size = m_Browser->size();
		depth = i->Depth;

	// Add blanks to ID Lookup vector to account for sub-menus
		for (unsigned long j = 1; j < (dsize - ddepth); j++) {
			m_PluginIDLookup.push_back(0);
		}
	// Add to ID Lookup vector (this maps Menu Items to Unique IDs)
		m_PluginIDLookup.push_back(i->UniqueID);
	}

	m_Browser->value(0);

	m_SetupGroup->add(m_Browser);

	m_InputScroll = new Fl_Scroll(10,135,480,145);
	m_InputScroll->box(m_GUIBox);
	m_InputScroll->color((Fl_Color)Info->GUICOL_Device);
	m_InputScroll->labelsize(12);
	m_InputScroll->align(FL_ALIGN_TOP_LEFT);
	m_InputScroll->type(Fl_Scroll::VERTICAL);

	m_InputPack = new Fl_Pack(15,140,470,135,"");
	m_InputPack->color((Fl_Color)Info->GUICOL_Device);
	m_InputScroll->add(m_InputPack);

	m_SetupGroup->add(m_InputScroll);

	m_ValueLabel = new Fl_Box(15,120,60,15,"Value");
	m_ValueLabel->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_ValueLabel->labelsize(12);
	m_SetupGroup->add(m_ValueLabel);

	m_DefaultLabel = new Fl_Box(77,120,60,15,"Default");
	m_DefaultLabel->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_DefaultLabel->labelsize(12);
	m_SetupGroup->add(m_DefaultLabel);

	m_MinLabel = new Fl_Box(139,120,60,15,"Min");
	m_MinLabel->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_MinLabel->labelsize(12);
	m_SetupGroup->add(m_MinLabel);

	m_MaxLabel = new Fl_Box(201,120,60,15,"Max");
	m_MaxLabel->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_MaxLabel->labelsize(12);
	m_SetupGroup->add(m_MaxLabel);

	m_ClampLabel = new Fl_Box(275,120,10,15,"Clamp?");
	m_ClampLabel->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_ClampLabel->labelsize(12);
	m_SetupGroup->add(m_ClampLabel);

	m_PortLabel = new Fl_Box(335,120,60,15,"Port Name");
	m_PortLabel->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_PortLabel->labelsize(12);
	m_SetupGroup->add(m_PortLabel);

	m_UpdateInputs = new Fl_LED_Button (10, 282, 25, 25, "Update input values?");
	m_UpdateInputs->labelsize(12);
	m_UpdateInputs->value(true);
	m_UpdateInputs->callback((Fl_Callback *)cb_UpdateInputs);

	m_SetupGroup->add(m_UpdateInputs);

	add(m_SetupGroup);

	m_PortIndex = 0;

	SetPage(2);

	end();
}

LADSPAPluginGUI::~LADSPAPluginGUI(void)
{
	if (m_InputPortNames)    free(m_InputPortNames);
	if (m_InputPortSettings) free(m_InputPortSettings);
	if (m_InputPortValues)   free(m_InputPortValues);
	if (m_InputPortDefaults) free(m_InputPortDefaults);

	m_PluginIDLookup.clear();

	Fl::check();
}

// This is done all the time
void LADSPAPluginGUI::Update(void)
{
	char temp[256];
	bool state_changed = false;

	m_GUICH->GetData("GetInputPortCount", &(m_InputPortCount));
	m_GUICH->GetData("GetInputPortValues", m_InputPortValues);
	m_GUICH->GetData("GetInputPortDefaults", m_InputPortDefaults);

// Need to show that a connection is present
// regardless of Refresh being set
	for (unsigned long p = 0; p < m_InputPortCount; p++) {
	// Check if plugin connect state is different to GUI state
	// A readonly default implies connection
		if ((m_InputPortValues[p].Connected &&
		     !(m_PortDefault[p]->readonly())) ||
		    (m_PortDefault[p]->readonly() &&
		     !(m_InputPortValues[p].Connected))) {
			if (m_InputPortValues[p].Connected) {
			// Disable
				m_PortDefault[p]->readonly(1);
				m_PortDefault[p]->color(FL_BACKGROUND_COLOR);
			} else {
			// Enable
				m_PortDefault[p]->readonly(0);
				m_PortDefault[p]->color(FL_BACKGROUND2_COLOR);
			// Update knob and slider inputs to new value
				sprintf(temp,"%.4f", m_InputPortDefaults[p]);
				m_KnobDefaults[p]->value(temp);
				m_SliderDefaults[p]->value(temp);
			}

			sprintf(temp,"%.4f", m_InputPortDefaults[p]);
			m_PortDefault[p]->value(temp);

			SetControlValue(p, BOTH);

			state_changed = true;
		}

	// Only update values if Refresh is set
		if (m_UpdateInputs->value()) {
			sprintf(temp,"%.4f", m_InputPortValues[p].Value);
			m_PortValue[p]->value(temp);

			if (m_InputPortValues[p].Connected) {
				sprintf(temp,"%.4f", m_InputPortDefaults[p]);
				m_PortDefault[p]->value(temp);
			}
		}
	}

// If a connection has been added/removed, we need to
// rearrange the knobs
	if (state_changed) {
	// Count the unconnected ports
		m_UnconnectedInputs = 0;
		for (unsigned long p = 0; p < m_InputPortCount; p++) {
			if (!(m_InputPortValues[p].Connected)) m_UnconnectedInputs++;
		}
		UpdateKnobs();
		UpdateSliders();

		m_BKnob->resize(x()+5, y()+15, 50, 20);
		m_BSlider->resize(x()+60, y()+15, 50, 20);
		m_BSetup->resize(x() + w() - 55, y()+15, 50, 20);
	}
}

// This lot is only done on patch load
void LADSPAPluginGUI::UpdateValues(SpiralPlugin *o)
{
	LADSPAPlugin* Plugin = (LADSPAPlugin*)o;
	SetUniqueID(Plugin->GetUniqueID());
	SetName(Plugin->GetName());
	SetMaker(Plugin->GetMaker());
	SetUpdateInputs(Plugin->GetUpdateInputs());
	m_InputPortCount = Plugin->GetInputPortCount();

	m_UnconnectedInputs = Plugin->GetUnconnectedInputs();

	const char *name;

	for (unsigned long p = 0; p < m_InputPortCount; p++) {
		name = Plugin->GetInputPortName(p);
		strncpy((char *)(m_InputPortNames + p * 256), name, 256);

		m_InputPortSettings[p] = Plugin->GetInputPortSetting(p);
		m_InputPortDefaults[p] = Plugin->GetInputPortDefault(p);
		m_InputPortValues[p] = Plugin->GetInputPortValue(p);

		AddPortInfo(p);
		SetPortSettings(p);
		SetControlValue(p, BOTH);
	}

	SetPage(Plugin->GetPage());

	m_PortIndex = m_InputPortCount;
}

// ****************************************************************************
// **                    Protected Member Functions                          **
// ****************************************************************************

const string LADSPAPluginGUI::GetHelpText(const string &loc)
{
//	if (loc == "DE") {
//	} else if (loc == "FR") {
//	} else {
	// Default to English?
		return string("LADSPA Plugin\n")
		            + "\n"
		            + "This plugin allows you to use any LADSPA plugin in SSM.\n"
		            + "\n"
		            + "It grows or shrinks the device GUI to allow you to connect\n"
		            + "up the ports as any other native SSM plugin, so you can\n"
		            + "seamlessly use the plugins as part of your layouts.\n"
		            + "\n"
		            + "The GUI window has two tabbed sections, Control and Setup.\n"
		            + "\n"
		            + "Setup is used to choose which LADSPA plugin to use, and\n"
		            + "allows you to configure port information.\n"
		            + "\n"
		            + "Once you have chosen a plugin, a row will appear for each\n"
		            + "input port:\n"
		            + "\n"
		            + "Value\n"
		            + "    The value being input to the port from a connection.\n"
		            + "Default\n"
		            + "    The value used as input if there is no connection. If\n"
		            + "    the port is connected, the default will use the value.\n"
		            + "    Upon disconnection, it will retain the last value\n"
		            + "    received.\n"
		            + "Min, Max\n"
		            + "    The range of values to scale a connected signal to,\n"
		            + "    assuming the signal is in the range -1.0 to +1.0.\n"
		            + "Clamp\n"
		            + "    Whether to scale inputs - if unchecked, the input is\n"
		            + "    not scaled.\n"
		            + "Port Name\n"
		            + "    The name of the port, as supplied by the plugin.\n"
		            + "\n"
		            + "The Control tab will display a control knob for each port\n"
		            + "that is not connected. This allows adjustment of input\n"
		            + "directly.";
//	}
}

// ****************************************************************************
// **                      Private Member Functions                          **
// ****************************************************************************

// Clear plugin selection and all related stuff
void LADSPAPluginGUI::ClearPlugin(void)
{
	m_InputPortCount = 0;
	m_UnconnectedInputs = 0;
	m_PortIndex = 0;

	m_GUICH->SetCommand(LADSPAPlugin::CLEARPLUGIN);
	m_GUICH->Wait();

// Clear out setup scroll
	while (m_InputPack->children() > 0) {
		Fl_Group *g = (Fl_Group *)m_InputPack->child(0);
		while (g->children() > 0) {
			Fl_Widget *w = g->child(0);
			g->remove(w);
		}
		m_InputPack->remove(g);
		delete g;
	}

// Clear out Knob and Slider groups
	while (m_KnobGroup->children() > 0) m_KnobGroup->remove(m_KnobGroup->child(0));
	while (m_SliderGroup->children() > 0) m_SliderGroup->remove(m_SliderGroup->child(0));

// Clear out all port widgets
	for (vector<Fl_Output *>::iterator i = m_PortValue.begin(); i != m_PortValue.end(); i++) delete *i;
	m_PortValue.clear();
	for (vector<Fl_Input *>::iterator i = m_PortMin.begin(); i != m_PortMin.end(); i++) delete *i;
	m_PortMin.clear();
	for (vector<Fl_Input *>::iterator i = m_PortMax.begin(); i != m_PortMax.end(); i++) delete *i;
	m_PortMax.clear();
	for (vector<Fl_Check_Button *>::iterator i = m_PortClamp.begin(); i != m_PortClamp.end(); i++) delete *i;
	m_PortClamp.clear();
	for (vector<Fl_Input *>::iterator i = m_PortDefault.begin(); i != m_PortDefault.end(); i++) delete *i;
	m_PortDefault.clear();
	for (vector<Fl_Knob *>::iterator i = m_Knobs.begin(); i != m_Knobs.end(); i++) delete *i;
	m_Knobs.clear();
	for (vector<Fl_Slider *>::iterator i = m_Sliders.begin(); i != m_Sliders.end(); i++) delete *i;
	m_Sliders.clear();
	for (vector<Fl_Input *>::iterator i = m_KnobDefaults.begin(); i != m_KnobDefaults.end(); i++) delete *i;
	m_KnobDefaults.clear();
	for (vector<Fl_Input *>::iterator i = m_SliderDefaults.begin(); i != m_SliderDefaults.end(); i++) delete *i;
	m_SliderDefaults.clear();
	for (vector<Fl_Box *>::iterator i = m_KnobLabels.begin(); i != m_KnobLabels.end(); i++) delete *i;
	m_KnobLabels.clear();
	for (vector<Fl_Box *>::iterator i = m_SliderLabels.begin(); i != m_SliderLabels.end(); i++) delete *i;
	m_SliderLabels.clear();

// Free label buffers
	for (vector<char *>::iterator i = m_KnobLabelBuffers.begin(); i != m_KnobLabelBuffers.end(); i++) {
		if (*i) free (*i);
	}
	m_KnobLabelBuffers.clear();
	for (vector<char *>::iterator i = m_SliderLabelBuffers.begin(); i != m_SliderLabelBuffers.end(); i++) {
		if (*i) free (*i);
	}
	m_SliderLabelBuffers.clear();

	m_SetupGroup->redraw();
}

// Setup from selected plugin
void LADSPAPluginGUI::SelectPlugin(void)
{
// Now get the new values to populate GUI controls
	m_GUICH->GetData("GetName", m_Name);
	m_GUICH->GetData("GetMaker", m_Maker);
	m_GUICH->GetData("GetInputPortCount", &(m_InputPortCount));
	m_GUICH->GetData("GetInputPortNames", m_InputPortNames);
	m_GUICH->GetData("GetInputPortSettings", m_InputPortSettings);
	m_GUICH->GetData("GetInputPortDefaults", m_InputPortDefaults);

	SetName((const char *)m_Name);
	SetMaker((const char *)m_Maker);

	for (unsigned long p = 0; p < m_InputPortCount; p++) {
		AddPortInfo(p);
		SetPortSettings(p);
		SetControlValue(p, BOTH);
	}

	m_PortIndex = m_InputPortCount;
	m_UnconnectedInputs = m_InputPortCount;

	m_SetupGroup->redraw();

}

// Change current page (Knobs/Sliders/Setup), resizing window
// to fit the contents
void LADSPAPluginGUI::SetPage(int index)
{
	m_Page = index;

	switch (m_Page) {
		case 0:
		{
			m_BKnob->value(1);
			m_BKnob->deactivate();
			m_BSlider->value(0);
			m_BSlider->activate();
			m_BSetup->value(0);
			m_BSetup->activate();

			m_KnobGroup->show();
			m_SliderGroup->hide();
			m_SetupGroup->hide();

			UpdateKnobs();
			break;
		}
		case 1:
		{
			m_BKnob->value(0);
			m_BKnob->activate();
			m_BSlider->value(1);
			m_BSlider->deactivate();
			m_BSetup->value(0);
			m_BSetup->activate();

			m_KnobGroup->hide();
			m_SliderGroup->show();
			m_SetupGroup->hide();

			UpdateSliders();
			break;
		}
		default:
		{
			m_BKnob->value(0);
			m_BKnob->activate();
			m_BSlider->value(0);
			m_BSlider->activate();
			m_BSetup->value(1);
			m_BSetup->deactivate();

			m_KnobGroup->hide();
			m_SliderGroup->hide();
			m_SetupGroup->show();

			Resize(500, 320);

			m_KnobGroup->resize(x()+5, y()+35, w()-10, h()-40);
			m_SliderGroup->resize(x()+5, y()+35, w()-10, h()-40);
			m_SetupGroup->resize(x()+5, y()+35, w()-10, h()-40);
		}
	}

	m_BKnob->resize(x()+5, y()+15, 50, 20);
	m_BSlider->resize(x()+60, y()+15, 50, 20);
	m_BSetup->resize(x() + w() - 55, y()+15, 50, 20);
}

void LADSPAPluginGUI::SetUniqueID(unsigned long n)
{
	m_UniqueID = n;

	vector<unsigned long>::iterator i = std::find(m_PluginIDLookup.begin(), m_PluginIDLookup.end(), m_UniqueID);

	if (i != m_PluginIDLookup.end()) {
		m_Browser->value(i - m_PluginIDLookup.begin());
	} else {
		m_Browser->value(0);
	}
}

void LADSPAPluginGUI::SetName(const char *s)
{
	m_NameLabel->label(s);
}

void LADSPAPluginGUI::SetMaker(const char *s)
{
	char temp[256];
	unsigned int len = strlen(s);

	strncpy(temp, s, len);
	// If this has got an "@" in it FLTK thinks it's a special character not an E.mail address
	int t=0;
	for (unsigned int f=0; f<len; f++) {
		if (t==255) break;
		if (temp[f]=='@') m_Maker[t++]='@';
		m_Maker[t++]=temp[f];
	}
	m_Maker[t]=0;
	m_MakerLabel->label (m_Maker);
}

void LADSPAPluginGUI::SetPortSettings(unsigned long p)
{
	char temp[256];
	sprintf(temp,"%.4f", m_InputPortSettings[p].Min);
	m_PortMin[p]->value(temp);

	sprintf(temp,"%.4f", m_InputPortSettings[p].Max);
	m_PortMax[p]->value(temp);

	sprintf(temp, "%d", m_InputPortSettings[p].Clamp);
	m_PortClamp[p]->value(atoi(temp));

	sprintf(temp, "%.4f", m_InputPortDefaults[p]);
	m_PortDefault[p]->value(temp);
	m_KnobDefaults[p]->value(temp);
	m_SliderDefaults[p]->value(temp);
}

void LADSPAPluginGUI::SetUpdateInputs(bool state)
{
	m_UpdateInputState = state;
	m_UpdateInputs->value(m_UpdateInputState);
}

// Setup all controls (Knob, Slider, Settings) for given port
void LADSPAPluginGUI::AddPortInfo(unsigned long p)
{
	Fl_Group* NewGroup = new Fl_Group(0,0,460,24,"");
	NewGroup->box(FL_FLAT_BOX);
	m_InputPack->add(NewGroup);

// Value
	Fl_Output* NewOutput = new Fl_Output(0,0,60,18,"");
	NewOutput->box(m_GUIBox);
        NewOutput->value(0);
	NewOutput->textsize(10);
	NewOutput->color(FL_BACKGROUND_COLOR);
	NewOutput->readonly(1);
	NewGroup->add(NewOutput);
	m_PortValue.push_back(NewOutput);

// Fixed Value/Default
	Fl_Input* NewInput = new Fl_Input(62,0,60,18,"");
	NewInput->box(fl_down(m_GUIBox));
	//NewInput->color(m_GUIColour);
	NewInput->value(0);
	NewInput->textsize(10);
	NewInput->callback((Fl_Callback *)cb_Default);
	NewGroup->add(NewInput);
	m_PortDefault.push_back(NewInput);

// Min
	NewInput = new Fl_Input(124,0,60,18,"");
	NewInput->box(fl_down(m_GUIBox));
	//NewInput->color(m_GUIColour);
	NewInput->value(0);
	NewInput->textsize(10);
	NewInput->callback((Fl_Callback *)cb_Min);
	NewGroup->add(NewInput);
	m_PortMin.push_back(NewInput);

// Max
	NewInput = new Fl_Input(186,0,60,18,"");
	NewInput->box(fl_down(m_GUIBox));
	//NewInput->color(m_GUIColour);
	NewInput->value(0);
	NewInput->textsize(10);
	NewInput->callback((Fl_Callback *)cb_Max);
	NewGroup->add(NewInput);
	m_PortMax.push_back(NewInput);

// Clamp
	Fl_Check_Button* NewCheckButton = new Fl_Check_Button(255,0,10,18,"");
	NewCheckButton->value(0);
	NewCheckButton->callback((Fl_Callback *)cb_Clamp);
	NewGroup->add(NewCheckButton);
	m_PortClamp.push_back(NewCheckButton);

// Port Name
	Fl_Box* NewText = new Fl_Box(315,0,10,18,"");
	NewText->label((const char *)(m_InputPortNames + p * 256));
	NewText->labelsize(10);
	NewText->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	NewGroup->add(NewText);

	NewGroup->redraw();
	m_InputPack->redraw();
	m_InputScroll->redraw();

// Control knobs and sliders - these aren't positoned or displayed yet,
// as this will depend on what is connected. All that is decided in
// UpdateKnobs()

// Knob
	Fl_Knob* NewKnob = new Fl_Knob(0,0,40,40,"");
	NewKnob->color(m_GUIColour);
	NewKnob->callback((Fl_Callback *)cb_Knob);
	NewKnob->hide();
	m_KnobGroup->add(NewKnob);
	m_Knobs.push_back(NewKnob);

// Direct input box for knob
	NewInput = new Fl_Input(0,0,60,16);
	NewInput->box(fl_down(m_GUIBox));
	//NewInput->color(m_GUIColour);
	NewInput->value(0);
	NewInput->textsize(10);
	NewInput->callback((Fl_Callback *)cb_KnobValue);
	NewInput->hide();
	m_KnobGroup->add(NewInput);
	m_KnobDefaults.push_back(NewInput);

// Knob Label
// Get maximum 19 characters (excluding "(AU)"/"(CV)" bit)
	size_t len = strlen((const char *)(m_InputPortNames + p * 256));
	len = len > 24 ? 19 : len - 5;
	char *kl = (char *)malloc(len+1);
	if (kl) {
		strncpy(kl, (const char *)(m_InputPortNames + p * 256), len);
		kl[len] = '\0';
		m_KnobLabelBuffers.push_back(kl);
	}
	NewText = new Fl_Box(0,0,100,12,(const char *)kl);
	NewText->align(FL_ALIGN_CENTER|FL_ALIGN_CLIP|FL_ALIGN_INSIDE);
	NewText->labelfont(FL_COURIER);
	NewText->labelsize(9);
	NewText->hide();
	m_KnobGroup->add(NewText);
	m_KnobLabels.push_back(NewText);

// Slider
	Fl_Slider* NewSlider = new Fl_Slider(0,0,20,100,"");
	NewSlider->box(fl_down(m_GUIBox));
	NewSlider->type(FL_VERT_NICE_SLIDER);
	NewSlider->selection_color(m_GUIColour);
	NewSlider->callback((Fl_Callback *)cb_Slider);
	NewSlider->hide();
	m_SliderGroup->add(NewSlider);
	m_Sliders.push_back(NewSlider);

// Direct input box for slider
	NewInput = new Fl_Input(0,0,56,16);
	NewInput->box(fl_down(m_GUIBox));
	//NewInput->color(m_GUIColour);
	NewInput->value(0);
	NewInput->textsize(10);
	NewInput->callback((Fl_Callback *)cb_SliderValue);
	NewInput->hide();
	m_SliderGroup->add(NewInput);
	m_SliderDefaults.push_back(NewInput);

// Slider Label
// Copy and truncate knob label to 11 chars for slider
	len = len > 11 ? 11 : len;
	char *sl = (char *)malloc(len+1);
	if (kl && sl) {
		strncpy(sl, (const char *)kl, len);
		sl[len] = '\0';
		m_SliderLabelBuffers.push_back(sl);
	}
	NewText = new Fl_Box(0,0,60,12,(const char *)sl);
	NewText->align(FL_ALIGN_CENTER|FL_ALIGN_CLIP|FL_ALIGN_INSIDE);
	NewText->labelfont(FL_COURIER);
	NewText->labelsize(9);
	NewText->hide();
	m_SliderGroup->add(NewText);
	m_SliderLabels.push_back(NewText);

// Set the range values
	SetControlRange(p, m_InputPortSettings[p].Min, m_InputPortSettings[p].Max);
}

// Rearrange knobs depending on connections
// Knobs corresponding to connected ports are hidden,
// the rest are shown
void LADSPAPluginGUI::UpdateKnobs(void)
{
	float sqrcount = sqrtf((float)m_UnconnectedInputs);
	float fsqrcount = floorf(sqrcount);
	float diff = sqrcount - fsqrcount;
	int cols = (int)fsqrcount;
	int rows = (int)fsqrcount;

	rows += diff > 0.0f ? 1 : 0;
	cols += diff > 0.5f ? 1 : 0;

	if (m_Page == 0) {
	// Size window to fit if current page is showing knobs
		int width;
		int height;

		if (m_UnconnectedInputs == 0) {
		// No knobs - minimal size
			width = 170;
			height = 80;
		} else if (m_UnconnectedInputs < 3) {
		// Single row for one or two knobs
			width = 10+m_UnconnectedInputs*100;
			height = 45+80;
			width = width < 170 ? 170 : width;
		} else {
		// Auto arrange
			width = 10+cols*100;
			height = 45+rows*80;
			width = width < 170 ? 170 : width;
		}
		Resize (width, height);

	// Resize all groups to fit
		m_KnobGroup->resize(x()+5, y()+35, w()-10, h()-40);
		m_SliderGroup->resize(x()+5, y()+35, w()-10, h()-40);
		m_SetupGroup->resize(x()+5, y()+35, w()-10, h()-40);

		redraw();
	}

// Add knobs into group
	int column = 0;
	int row = 0;
	for (unsigned long p = 0; p < m_InputPortCount; p++)
	{
		if (!m_InputPortValues[p].Connected) {
			if (m_UnconnectedInputs == 1) {
			// Single knob - centre
				m_Knobs[p]->resize(x()+35+30,y()+45,40,40);
				m_KnobDefaults[p]->resize(x()+35+20,y()+45+40,60,16);
				m_KnobLabels[p]->resize(x()+5+30,y()+45+55,100,15);
			} else if (m_UnconnectedInputs == 2) {
			// Two knobs - put in single row
				column = row;
				m_Knobs[p]->resize(x()+35+column*100,y()+45,40,40);
				m_KnobDefaults[p]->resize(x()+25+column*100,y()+45+40,60,16);
				m_KnobLabels[p]->resize(x()+5+column*100,y()+45+55,100,15);
			} else {
			// Auto arrange
				m_Knobs[p]->resize(x()+35+column*100, y()+45+row*80,40,40);
				m_KnobDefaults[p]->resize(x()+25+column*100,y()+45+40+row*80,60,16);
				m_KnobLabels[p]->resize(x()+5+column*100,y()+45+55+row*80,100,15);
			}

			if (++column==cols) {
				column = 0;
				row++;
			}

			m_Knobs[p]->show();
			m_KnobDefaults[p]->show();
			m_KnobLabels[p]->show();
		} else {
			m_Knobs[p]->hide();
			m_KnobDefaults[p]->hide();
			m_KnobLabels[p]->hide();
		}
	}
}

// Rearrange sliders depending on connections
// Sliders corresponding to connected ports are hidden,
// the rest are shown
void LADSPAPluginGUI::UpdateSliders(void)
{
	int cols;
	int rows;
	int fullrows = 0;

	if (m_UnconnectedInputs < 9) {
		cols = m_UnconnectedInputs;
		rows = 1;
		fullrows = 1;
	} else {
		float sqrcount = sqrt((float)m_UnconnectedInputs);
		float aspect;
		int diff;

		cols = (int)floorf(sqrcount * 2.0f);
		rows = (int)floorf(sqrcount * 0.5f);
		aspect = (float)cols / (float)rows;
		diff = cols * rows - m_UnconnectedInputs;

		if (diff < 0) {
			if (aspect > 4.0f) {
				rows++;
				diff += cols;
			}
			if (diff > rows - 1) {
				cols -= (int)floorf((float)diff / (float)rows);
			} else if (diff < 0) {
				cols += (int)ceilf(fabsf((float)diff) / (float)rows);
			}
		}
		fullrows = rows - (cols * rows - m_UnconnectedInputs);
	}

	if (m_Page == 1) {
	// Size window to fit if current page is showing sliders
		int width;
		int height;

		if (m_UnconnectedInputs == 0) {
		// No sliders - minimal size
			width = 170;
			height = 80;
		} else if (m_UnconnectedInputs < 3) {
		// Single row of centred sliders if less than three
			width = 170;
			height = 45+140;
		} else if (m_UnconnectedInputs < 9) {
		// Single row for up to eight sliders
			width = 10+m_UnconnectedInputs*60;
			height = 45+140;
			width = width < 170 ? 170 : width;
		} else {
		// Auto arrange
			width = 10+cols*60;
			height = 45+rows*140;
			width = width < 170 ? 170 : width;
		}
		Resize (width, height);

	// Resize all groups to fit
		m_KnobGroup->resize(x()+5, y()+35, w()-10, h()-40);
		m_SliderGroup->resize(x()+5, y()+35, w()-10, h()-40);
		m_SetupGroup->resize(x()+5, y()+35, w()-10, h()-40);
	}

// Add sliders into group
	int column = 0;
	int row = 0;
	for (unsigned long p = 0; p < m_InputPortCount; p++)
	{
		if (!m_InputPortValues[p].Connected) {
			if (m_UnconnectedInputs == 0) {
			// Nothing
			} else if (m_UnconnectedInputs < 3) {
			// Single row of centred sliders if less than three
			// Width is 170, with 5 pixel 'border'
				int offset=(160-m_UnconnectedInputs*60)/2;
				m_Sliders[p]->resize(x()+25+offset+column*60, y()+45,20,100);
				m_SliderDefaults[p]->resize(x()+7+offset+column*60,y()+45+101,56,16);
				m_SliderLabels[p]->resize(x()+5+offset+column*60, y()+45+116,60,15);
			} else {
			// Arrange as per columns and rows
				m_Sliders[p]->resize(x()+25+column*60, y()+45+row*140,20,100);
				m_SliderDefaults[p]->resize(x()+7+column*60,y()+45+101+row*140,56,16);
				m_SliderLabels[p]->resize(x()+5+column*60, y()+45+116+row*140,60,15);
			}

			if (++column==(cols - (row < fullrows ? 0 : 1))) {
				column = 0;
				row++;
			}
			m_Sliders[p]->show();
			m_SliderDefaults[p]->show();
			m_SliderLabels[p]->show();
		} else {
			m_Sliders[p]->hide();
			m_SliderDefaults[p]->hide();
			m_SliderLabels[p]->hide();
		}
	}
}

// Set value of slider and/or knob (both use the same settings)
void LADSPAPluginGUI::SetControlValue(unsigned long p, WhichControl wc)
{
	float min = atof(m_PortMin[p]->value());
	float max = atof(m_PortMax[p]->value());
	float value = atof(m_PortDefault[p]->value());

	float logbase = m_InputPortSettings[p].LogBase;
	if (logbase > 1.0f) {
	// Logarithmic control - requires conversion
		if (fabsf(value) > logbase) {
			if (value > 0.0f) {
				value = logf(value) / logf(logbase);
			} else {
				value = -logf(-value) / logf(logbase);
			}
		} else {
			value /= logbase;
		}
	}

	if (wc == KNOB   || wc == BOTH) m_Knobs[p]->value(value);

// Invert slider value, as sliders are upside down
	if (wc == SLIDER || wc == BOTH) m_Sliders[p]->value(m_Sliders[p]->maximum() - value + m_Sliders[p]->minimum());
}

// Set range of slider and knob (both use the same settings)
void LADSPAPluginGUI::SetControlRange(unsigned long p, float min, float max)
{
	if (m_InputPortSettings[p].Integer) {
	// Integer control - integer steps between minimum and maximum
		min = floorf(min + 0.5f);
		max = floorf(max + 0.5f);

	// Change steps to map to integers
		m_Knobs[p]->step(1.0f);
		m_Knobs[p]->scaleticks((int)(max - min));
		m_Sliders[p]->step(1.0f / (max - min));
	} else {
		float logbase = m_InputPortSettings[p].LogBase;
		if (logbase > 1.0f) {
		// Continuous logarithmic control
			float loglogbase = logf(logbase);

			if (fabsf(min) > logbase) {
				if (min > logbase) {
					min = logf(min) / loglogbase;
				} else {
					min = -logf(-min) / loglogbase;
				}
			} else {
				min /= logbase;
			}
			if (fabsf(max) > logbase) {
				if (max > logbase) {
					max = logf(max) / loglogbase;
				} else {
					max = -logf(-max) / loglogbase;
				}
			} else {
				max /= logbase;
			}
		} else {
		// Continuous linear control
		// Same as given min and max
		}

		m_Knobs[p]->step((max - min) / 10000.0f);
		m_Sliders[p]->step((max - min) / 10000.0f);
	}

	m_Knobs[p]->minimum(min);
	m_Knobs[p]->maximum(max);
	m_Sliders[p]->minimum(min);
	m_Sliders[p]->maximum(max);
}

// The current value ('Default') can be changed from all three pages
void LADSPAPluginGUI::SetPortValue(unsigned long p, float value, int frompage)
{
	m_Default = value;
	m_Min = atof(m_PortMin[p]->value());
	m_Max = atof(m_PortMax[p]->value());

// Pass current port index
	m_GUICH->SetData("SetInputPortIndex", &p);

// If default is out of [Min, Max] range, stretch range
	if (m_Default < m_Min) {
		m_PortMin[p]->value(m_PortDefault[p]->value());
		m_Min = m_Default;

	// Pass new Minimum to plugin
		m_GUICH->SetData("SetInputPortMin", &m_Min);
		m_GUICH->SetCommand(LADSPAPlugin::SETMAX);
		m_GUICH->Wait();

	// Reconfigure knob range
		SetControlRange(m_PortIndex, m_Min, m_Max);
	} else if (m_Default > m_Max) {
		m_PortMax[p]->value(m_PortDefault[p]->value());
		m_Max = m_Default;

	// Pass new Maximum to plugin
		m_GUICH->SetData("SetInputPortMax", &m_Max);
		m_GUICH->SetCommand(LADSPAPlugin::SETMAX);
		m_GUICH->Wait();

	// Reconfigure knob and slider range
		SetControlRange(p, m_Min, m_Max);
	}

// Pass new default to plugin
	m_GUICH->SetData("SetInputPortDefault", &m_Default);
	m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

// Update other two default input boxes
	char temp[256];
	sprintf(temp, "%.4f", m_Default);

	switch (frompage)
	{
		case 0:
		{
		// Set from knob page - update slider and setup defaults
			m_SliderDefaults[p]->value(temp);
			m_PortDefault[p]->value(temp);
			break;
		}
		case 1:
		{
		// Set from slider page - update knob and setup defaults
			m_KnobDefaults[p]->value(temp);
			m_PortDefault[p]->value(temp);
			break;
		}
		default:
		{
		// Set from setup page - update knob and slider defaults
			m_KnobDefaults[p]->value(temp);
			m_SliderDefaults[p]->value(temp);
		}
	}

// Set knob and slider to corresponding position
	SetControlValue(p, BOTH);
}

// Convert value supplied by a control (knob/slider) to actual
// value in range
float LADSPAPluginGUI::ConvertControlValue(unsigned long p, float value)
{
	float logbase = m_InputPortSettings[p].LogBase;
	if (logbase > 1.0f) {
	// Logarithmic control - convert back to actual value
		if (fabsf(value) > 1.0f) {
			if (value > 0.0f) {
				value = powf(logbase, value);
			} else {
				value = -powf(logbase, -value);
			}
		} else {
			value *= logbase;
		}
	}

	return value;
}

// ****************************************************************************
// **                     Widget Callback Functions                          **
// ****************************************************************************

inline void LADSPAPluginGUI::cb_BKnob_i(Fl_Button* o)
{
	SetPage(0);

	m_GUICH->SetData("SetPage", &m_Page);
	m_GUICH->SetCommand(LADSPAPlugin::SETPAGE);
}
void LADSPAPluginGUI::cb_BKnob(Fl_Button* o)
{   //                     GUI
	((LADSPAPluginGUI*)(o->parent()))->cb_BKnob_i(o);
}

inline void LADSPAPluginGUI::cb_BSlider_i(Fl_Button* o)
{
	SetPage(1);

	m_GUICH->SetData("SetPage", &m_Page);
	m_GUICH->SetCommand(LADSPAPlugin::SETPAGE);
}
void LADSPAPluginGUI::cb_BSlider(Fl_Button* o)
{   //                     GUI
	((LADSPAPluginGUI*)(o->parent()))->cb_BSlider_i(o);
}

inline void LADSPAPluginGUI::cb_BSetup_i(Fl_Button* o)
{
	SetPage(2);

	m_GUICH->SetData("SetPage", &m_Page);
	m_GUICH->SetCommand(LADSPAPlugin::SETPAGE);
}
void LADSPAPluginGUI::cb_BSetup(Fl_Button* o)
{   //                     GUI
	((LADSPAPluginGUI*)(o->parent()))->cb_BSetup_i(o);
}

inline void LADSPAPluginGUI::cb_Select_i(Fl_Choice* o)
{
	ClearPlugin();

	unsigned long m_UniqueID = m_PluginIDLookup[o->value()];

	if (m_UniqueID != 0) {
	// Plugin selected
		m_GUICH->SetData("SetUniqueID",&m_UniqueID);
		m_GUICH->SetCommand(LADSPAPlugin::SELECTPLUGIN);
		m_GUICH->Wait();
	}
	SelectPlugin();
        // Make sure GUI frame can fit all the ports
        Resize (w(), h());
}
void LADSPAPluginGUI::cb_Select(Fl_Choice* o)
{   //                     Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()))->cb_Select_i(o);
}

inline void LADSPAPluginGUI::cb_UpdateInputs_i(Fl_LED_Button* o)
{
	m_UpdateInputState = (bool)(o->value());

	m_GUICH->SetData("SetUpdateInputs", &m_UpdateInputState);
	m_GUICH->SetCommand(LADSPAPlugin::SETUPDATEINPUTS);
}
void LADSPAPluginGUI::cb_UpdateInputs(Fl_LED_Button* o)
{   //                     Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()))->cb_UpdateInputs_i(o);
}

inline void LADSPAPluginGUI::cb_Default_i(Fl_Input* o)
{
// Which Default was changed?
	bool do_search = false;
	if (m_PortIndex == m_PortDefault.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortDefault[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Input *>::iterator i = std::find(m_PortDefault.begin(),
		                                           m_PortDefault.end(),
		                                           o);
		m_PortIndex = distance(m_PortDefault.begin(), i);
	}

	float value = atof(o->value());
	SetPortValue(m_PortIndex, value, 2);
}
void LADSPAPluginGUI::cb_Default(Fl_Input* o)
{   //                  Group        Pack      Scroll    Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()))->cb_Default_i(o);
}

inline void LADSPAPluginGUI::cb_Min_i(Fl_Input* o)
{
// Which Min was changed?
	bool do_search = false;
	if (m_PortIndex == m_PortMin.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortMin[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Input *>::iterator i = std::find(m_PortMin.begin(),
		                                           m_PortMin.end(),
		                                           o);
		m_PortIndex = distance(m_PortMin.begin(), i);
	}

// Pass current port index
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);

// Check that min is really min and max is really max
	m_Min = atof(o->value());
	m_Max = atof(m_PortMax[m_PortIndex]->value());

	if (m_Min > m_Max) {
	// Swap min and max (need to set max as well)
		float min = m_Min;
		m_Min = m_Max;
		m_Max = min;

		m_GUICH->SetData("SetInputPortMax", &m_Max);
		m_GUICH->SetCommand(LADSPAPlugin::SETMAX);
		m_GUICH->Wait();

	// Swap displayed min and max
		char temp[256];
		strncpy(temp, m_PortMin[m_PortIndex]->value(), 256);
		m_PortMin[m_PortIndex]->value(m_PortMax[m_PortIndex]->value());
		m_PortMax[m_PortIndex]->value(temp);
		m_PortMin[m_PortIndex]->redraw();
		m_PortMax[m_PortIndex]->redraw();
	}
	m_GUICH->SetData("SetInputPortMin", &m_Min);
	m_GUICH->SetCommand(LADSPAPlugin::SETMIN);

// Clip default to range
	m_Default = atof(m_PortDefault[m_PortIndex]->value());
	if (m_Default < m_Min) {
		m_Default = m_Min;
		m_GUICH->SetData("SetInputPortDefault",&m_Default);

		m_GUICH->Wait();
		m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

	// Print to default input box, and the input boxes on the
	// knob and slider pages
		char temp[256];
		sprintf(temp, "%.4f", m_Default);
		m_PortDefault[m_PortIndex]->value(temp);
		m_KnobDefaults[m_PortIndex]->value(temp);
		m_SliderDefaults[m_PortIndex]->value(temp);
	}

// Reposition and reconfigure knob and slider to reflect new range
	SetControlValue(m_PortIndex, BOTH);
	SetControlRange(m_PortIndex, m_Min, m_Max);
}
void LADSPAPluginGUI::cb_Min(Fl_Input* o)
{   //                  Group        Pack      Scroll    Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()))->cb_Min_i(o);
}

inline void LADSPAPluginGUI::cb_Max_i(Fl_Input* o)
{
// Which Max was changed?
	bool do_search = false;
	if (m_PortIndex == m_PortMax.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortMax[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Input *>::iterator i = std::find(m_PortMax.begin(),
		                                           m_PortMax.end(),
		                                           o);
		m_PortIndex = distance(m_PortMax.begin(), i);
	}

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);

// Check that min is really min and max is really max
	m_Max = atof(o->value());
	m_Min = atof(m_PortMin[m_PortIndex]->value());

	if (m_Min > m_Max) {
	// Swap min and max (need to set max as well)
		float max = m_Min;
		m_Min = m_Max;
		m_Max = max;

		m_GUICH->SetData("SetInputPortMin", &m_Min);
		m_GUICH->SetCommand(LADSPAPlugin::SETMIN);
		m_GUICH->Wait();

	// Swap displayed min and max
		char temp[256];
		strncpy(temp, m_PortMax[m_PortIndex]->value(), 256);
		m_PortMax[m_PortIndex]->value(m_PortMin[m_PortIndex]->value());
		m_PortMin[m_PortIndex]->value(temp);
		m_PortMax[m_PortIndex]->redraw();
		m_PortMin[m_PortIndex]->redraw();
	}

	m_GUICH->SetData("SetInputPortMax", &m_Max);
	m_GUICH->SetCommand(LADSPAPlugin::SETMAX);

// Clip default to range
	m_Default = atof(m_PortDefault[m_PortIndex]->value());
	if (m_Default > m_Max) {
		m_Default = m_Max;
		m_GUICH->SetData("SetInputPortDefault",&m_Default);

		m_GUICH->Wait();
		m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

	// Print to displayed default
		char temp[256];
		sprintf(temp, "%.4f", m_Default);
		m_PortDefault[m_PortIndex]->value(temp);
		m_KnobDefaults[m_PortIndex]->value(temp);
		m_SliderDefaults[m_PortIndex]->value(temp);
	}

// Reposition and reconfigure knob to reflect new range
	SetControlValue(m_PortIndex, BOTH);
	SetControlRange(m_PortIndex, m_Min, m_Max);
}
void LADSPAPluginGUI::cb_Max(Fl_Input* o)
{   //                  Group        Pack      Scroll    Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()))->cb_Max_i(o);
}

inline void LADSPAPluginGUI::cb_Clamp_i(Fl_Check_Button* o)
{
// Which Clamp was changed?
	bool do_search = false;
	if (m_PortIndex == m_PortClamp.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortClamp[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Check_Button *>::iterator i = std::find(m_PortClamp.begin(),
		                                                  m_PortClamp.end(),
		                                                  o);
		m_PortIndex = distance(m_PortClamp.begin(), i);
	}

	m_Clamp = (bool)(o->value());

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);
	m_GUICH->SetData("SetInputPortClamp", &m_Clamp);
	m_GUICH->SetCommand(LADSPAPlugin::SETCLAMP);
}
void LADSPAPluginGUI::cb_Clamp(Fl_Check_Button* o)
{   //                  Group        Pack      Scroll    Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()))->cb_Clamp_i(o);
}

inline void LADSPAPluginGUI::cb_Knob_i(Fl_Knob *o)
{
// First, find which knob is being adjusted
	bool do_search = false;
	if (m_PortIndex == m_Knobs.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_Knobs[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query knob if it is different from last one adjusted
		vector<Fl_Knob *>::iterator i = std::find(m_Knobs.begin(),
		                                          m_Knobs.end(),
		                                          o);
		m_PortIndex = distance(m_Knobs.begin(), i);
	}

// Get value
	m_Default = ConvertControlValue(m_PortIndex, o->value());

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);
	m_GUICH->SetData("SetInputPortDefault", &m_Default);
	m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

// Copy to Default field in Port Setup list
	char temp[256];
	sprintf(temp, "%.4f", m_Default);
	m_PortDefault[m_PortIndex]->value(temp);
	m_KnobDefaults[m_PortIndex]->value(temp);
	m_SliderDefaults[m_PortIndex]->value(temp);

// Set corresponding slider
	SetControlValue(m_PortIndex, SLIDER);
}
void LADSPAPluginGUI::cb_Knob(Fl_Knob *o)
{   //                     Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()))->cb_Knob_i(o);
}

inline void LADSPAPluginGUI::cb_KnobValue_i(Fl_Input *o)
{
// Which Knob Value was changed?
	bool do_search = false;
	if (m_PortIndex == m_KnobDefaults.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_KnobDefaults[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Input *>::iterator i = std::find(m_KnobDefaults.begin(),
		                                           m_KnobDefaults.end(),
		                                           o);
		m_PortIndex = distance(m_KnobDefaults.begin(), i);
	}

	float value = atof(o->value());
	SetPortValue(m_PortIndex, value, 0);
}
void LADSPAPluginGUI::cb_KnobValue(Fl_Input *o)
{   //                     Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()))->cb_KnobValue_i(o);
}

inline void LADSPAPluginGUI::cb_Slider_i(Fl_Slider *o)
{
// First, find which slider is being adjusted
	bool do_search = false;
	if (m_PortIndex == m_Sliders.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_Sliders[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query slider if it is different from last one adjusted
		vector<Fl_Slider *>::iterator i = std::find(m_Sliders.begin(),
		                                            m_Sliders.end(),
		                                            o);
		m_PortIndex = distance(m_Sliders.begin(), i);
	}

// Get value (Invert it, as sliders are upside down)
	m_Default = ConvertControlValue(m_PortIndex, o->maximum() - o->value() + o->minimum());

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);
	m_GUICH->SetData("SetInputPortDefault", &m_Default);
	m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

// Copy to Default field in Port Setup list
	char temp[256];
	sprintf(temp, "%.4f", m_Default);
	m_PortDefault[m_PortIndex]->value(temp);
	m_KnobDefaults[m_PortIndex]->value(temp);
	m_SliderDefaults[m_PortIndex]->value(temp);

// Set corresponding knob
	SetControlValue(m_PortIndex, KNOB);
}
void LADSPAPluginGUI::cb_Slider(Fl_Slider *o)
{   //                     Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()))->cb_Slider_i(o);
}

inline void LADSPAPluginGUI::cb_SliderValue_i(Fl_Input *o)
{
// Which Slider Value was changed?
	bool do_search = false;
	if (m_PortIndex == m_SliderDefaults.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_SliderDefaults[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Input *>::iterator i = std::find(m_SliderDefaults.begin(),
		                                           m_SliderDefaults.end(),
		                                           o);
		m_PortIndex = distance(m_SliderDefaults.begin(), i);
	}

	float value = atof(o->value());
	SetPortValue(m_PortIndex, value, 1);
}
void LADSPAPluginGUI::cb_SliderValue(Fl_Input *o)
{   //                     Group     GUI
	((LADSPAPluginGUI*)(o->parent()->parent()))->cb_SliderValue_i(o);
}
