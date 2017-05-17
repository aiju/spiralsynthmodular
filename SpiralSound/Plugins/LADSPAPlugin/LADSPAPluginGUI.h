/*  LADSPAPluginGUI.h
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

#ifndef __ladspa_plugin_gui_h__
#define __ladspa_plugin_gui_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include "../Widgets/Fl_Knob.H"
#include "../Widgets/Fl_LED_Button.H"

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <dlfcn.h>

#include "LADSPAPlugin.h"
#include "LADSPAInfo.h"
#include "../SpiralPluginGUI.h"

class LADSPAPluginGUI : public SpiralPluginGUI
{
public:
	LADSPAPluginGUI(int w, int h,
	                LADSPAPlugin *o,
	                ChannelHandler *ch,
	                const HostInfo *Info,
	                const std::vector<LADSPAInfo::PluginEntry> &PVec);
	~LADSPAPluginGUI(void);

	virtual void UpdateValues(SpiralPlugin *o);
	virtual void Update(void);

protected:
	const std::string GetHelpText(const std::string &loc);
private:
	void ClearPlugin(void);
	void SelectPlugin(void);

	void SetPage(int index);
	void SetUniqueID(unsigned long n);
	void SetName(const char *s);
	void SetMaker(const char *s);
	void SetPortSettings(unsigned long p);
	void SetUpdateInputs(bool state);

	void AddPortInfo(unsigned long p);
	void HandleResize(void);

// For SetControlValue
	enum WhichControl
	{
		KNOB,
		SLIDER,
		BOTH
	};

	void UpdateKnobs(void);
	void UpdateSliders(void);
    void SetControlValue(unsigned long p, WhichControl wc);
	void SetControlRange(unsigned long p, float min, float max);
	void SetPortValue(unsigned long p, float value, int frompage);
	float ConvertControlValue(unsigned long p, float value);

	Fl_Color m_GUIColour;
	Fl_Boxtype m_GUIBox;

// Buttons
	Fl_Button                     *m_BKnob;
	Fl_Button                     *m_BSlider;
	Fl_Button                     *m_BSetup;
// Knobs
	Fl_Group                      *m_KnobGroup;
	std::vector<Fl_Knob *>         m_Knobs;
	std::vector<Fl_Input *>        m_KnobDefaults;
	std::vector<Fl_Box *>          m_KnobLabels;
	std::vector<char *>            m_KnobLabelBuffers;
// Sliders
	Fl_Group                      *m_SliderGroup;
	std::vector<Fl_Slider *>       m_Sliders;
	std::vector<Fl_Input *>        m_SliderDefaults;
	std::vector<Fl_Box *>          m_SliderLabels;
	std::vector<char *>            m_SliderLabelBuffers;
// Setup stuff
	Fl_Group                      *m_SetupGroup;
	Fl_Choice                     *m_Browser;
	Fl_Box                        *m_NameLabel;
	Fl_Box                        *m_MakerLabel;
	Fl_Box                        *m_ValueLabel;
	Fl_Box                        *m_DefaultLabel;
	Fl_Box                        *m_MinLabel;
	Fl_Box                        *m_MaxLabel;
	Fl_Box                        *m_ClampLabel;
	Fl_Box                        *m_PortLabel;
	std::vector<Fl_Output*>        m_PortValue;
	std::vector<Fl_Input*>         m_PortMin;
	std::vector<Fl_Input*>         m_PortMax;
	std::vector<Fl_Check_Button*>  m_PortClamp;
	std::vector<Fl_Input*>         m_PortDefault;

	Fl_Scroll                     *m_InputScroll;
	Fl_Pack                       *m_InputPack;
	Fl_LED_Button                 *m_UpdateInputs;

	unsigned long                  m_UnconnectedInputs;

	std::vector<LADSPAInfo::PluginEntry> m_PluginList;
	std::vector<unsigned long>     m_PluginIDLookup;

	unsigned long                  m_PortIndex;
	float                          m_Default;
	float                          m_Min;
	float                          m_Max;
	bool                           m_Clamp;

	unsigned long                  m_UniqueID;
	int                            m_Page;
	bool                           m_UpdateInputState;
	char                           m_Name[256];
	char                           m_Maker[256];
	unsigned long                  m_MaxInputPortCount;
	unsigned long                  m_InputPortCount;
	char                          *m_InputPortNames;
	PortSetting                   *m_InputPortSettings;
	PortValue                     *m_InputPortValues;
	float                         *m_InputPortDefaults;

	inline void cb_BKnob_i(Fl_Button* o);
	static void cb_BKnob(Fl_Button* o);
	inline void cb_BSlider_i(Fl_Button* o);
	static void cb_BSlider(Fl_Button* o);
	inline void cb_BSetup_i(Fl_Button* o);
	static void cb_BSetup(Fl_Button* o);
	inline void cb_Select_i(Fl_Choice* o);
	static void cb_Select(Fl_Choice* o);
	inline void cb_UpdateInputs_i(Fl_LED_Button* o);
	static void cb_UpdateInputs(Fl_LED_Button* o);
	inline void cb_Default_i(Fl_Input* o);
	static void cb_Default(Fl_Input* o);
	inline void cb_Min_i(Fl_Input* o);
	static void cb_Min(Fl_Input* o);
	inline void cb_Max_i(Fl_Input* o);
	static void cb_Max(Fl_Input* o);
	inline void cb_Clamp_i(Fl_Check_Button* o);
	static void cb_Clamp(Fl_Check_Button* o);
	inline void cb_Knob_i(Fl_Knob *o);
	static void cb_Knob(Fl_Knob *o);
	inline void cb_KnobValue_i(Fl_Input *o);
	static void cb_KnobValue(Fl_Input *o);
	inline void cb_Slider_i(Fl_Slider *o);
	static void cb_Slider(Fl_Slider *o);
	inline void cb_SliderValue_i(Fl_Input *o);
	static void cb_SliderValue(Fl_Input *o);
};

#endif // __ladspa_plugin_gui_h__
