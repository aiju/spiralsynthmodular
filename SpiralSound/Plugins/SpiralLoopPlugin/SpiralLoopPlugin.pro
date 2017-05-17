HEADERS     = ../SpiralPlugin.h \
			  ../SpiralPluginGUI.h \
			  ../Widgets/Fl_Knob.H \
			  ../Widgets/Fl_DragBar.H \
			  ../../Sample.h \
			  ../../RiffWav.h \			  
			  SpiralSound/Delay.h \
			  SpiralSound/Filter.h \
			  SpiralSound/Reverb.h \
  			  SpiralSound/iir_filter.h \
			  GUI/DelayGUI.h \
			  GUI/FilterGUI.h \
			  GUI/GUIBase.h \
			  GUI/LoopGUI.h \
			  GUI/ReverbGUI.h \			  			  			  
			  GUI/Widgets/Fl_LED_Button.H \
			  GUI/Widgets/Fl_Loop.h \
  			  GUI/Widgets/Fl_Trigger.h \
			  Loop.h \
			  SpiralLoopPlugin.h \
			  SpiralLoopPluginGUI.h
			  
SOURCES     = ../SpiralPlugin.C \
			  ../SpiralPluginGUI.C \
			  ../Widgets/Fl_Knob.cxx \
			  ../Widgets/Fl_DragBar.cxx \
			  ../../Sample.C \	
  			  ../../RiffWav.C \
  			  SpiralSound/Delay.C \
			  SpiralSound/Filter.C \
			  SpiralSound/Reverb.C \
  			  SpiralSound/iir_filter.C \
			  GUI/DelayGUI.C \
			  GUI/FilterGUI.C \
			  GUI/GUIBase.C \
			  GUI/LoopGUI.C \
			  GUI/ReverbGUI.C \			  			  			  
			  GUI/Widgets/Fl_LED_Button.cxx \
			  GUI/Widgets/Fl_Loop.C \
  			  GUI/Widgets/Fl_Trigger.C \
			  Loop.C \
			  SpiralLoopPlugin.C \
			  SpiralLoopPluginGUI.C

TARGET      = SpiralLoopPlugin.so
