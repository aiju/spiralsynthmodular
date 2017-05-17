#############################################################################
# Makefile for building SpiralSynthModular
#############################################################################

SHELL = @SHELL@

####### Compiler, tools and options

CC	=	gcc
CXX	=	g++
CFLAGS	=	@CFLAGS@ @FLTK_CFLAGS@
CXXFLAGS=	@CXXFLAGS@ @FLTK_CXXFLAGS@
INCPATH	=
LINK	=	g++
LFLAGS	=
LIBS	=   @FLTK_LIBS@ -lm -ldl -lpthread
MOC     = moc
UIC	=

TAR	=	tar -cf
GZIP	=	gzip -9f

INSTALL = @INSTALL@

###### Autoconf variables

prefix = @prefix@
exec_prefix = @exec_prefix@

bindir = @bindir@
sbindir = @sbindir@
libexecdir = @libexecdir@
datadir = @datadir@
sysconfdir = @sysconfdir@
sharedstatedir = @sharedstatedir@
localstatedir = @localstatedir@
libdir = @libdir@
infodir = @infodir@
mandir = @mandir@

SpiralPlugins = $(libdir)/SpiralPlugins

####### Files

HEADERS =	SpiralSynthModular.h \
		GraphSort.h \
		SettingsWindow.h \
		SpiralSynthPluginLocation.h \
                GUI/Widgets/SpiralGUI.H \
		GUI/Widgets/Fl_DeviceGUI.h \
		GUI/Widgets/Fl_CommentGUI.h \
		GUI/Widgets/Fl_DragBar.H \
		GUI/Widgets/Fl_Canvas.h \
		GUI/Widgets/PawfalYesNo.h \
		GUI/Widgets/PawfalInput.h \
		SpiralSound/SpiralInfo.h \
		SpiralSound/Sample.h \
		SpiralSound/PluginManager.h \
		SpiralSound/ChannelHandler.h \
		SpiralSound/Plugins/SpiralPlugin.h \
		SpiralSound/Plugins/SpiralPluginGUI.h
SOURCES =	main.cpp \
		SpiralSynthModular.C \
		GraphSort.C \
		SettingsWindow.C \
                GUI/Widgets/SpiralGUI.C \
		GUI/Widgets/Fl_DeviceGUI.C \
		GUI/Widgets/Fl_CommentGUI.C \
		GUI/Widgets/Fl_DragBar.cxx \
		GUI/Widgets/Fl_Canvas.C \
		GUI/Widgets/PawfalYesNo.C \
		GUI/Widgets/PawfalInput.C \
		SpiralSound/SpiralInfo.C \
		SpiralSound/Sample.C \
		SpiralSound/PluginManager.C \
		SpiralSound/ChannelHandler.C \
		SpiralSound/Plugins/SpiralPlugin.C \
		SpiralSound/Plugins/SpiralPluginGUI.C
OBJECTS =	main.o \
		SpiralSynthModular.o \
		GraphSort.o \
		SettingsWindow.o \
                GUI/Widgets/SpiralGUI.o \
		GUI/Widgets/Fl_DeviceGUI.o \
		GUI/Widgets/Fl_CommentGUI.o \
		GUI/Widgets/Fl_DragBar.o \
		GUI/Widgets/Fl_Canvas.o \
		GUI/Widgets/PawfalYesNo.o \
		GUI/Widgets/PawfalInput.o \
		SpiralSound/SpiralInfo.o \
		SpiralSound/Sample.o \
		SpiralSound/PluginManager.o \
		SpiralSound/ChannelHandler.o \
		SpiralSound/Plugins/SpiralPlugin.o \
		SpiralSound/Plugins/SpiralPluginGUI.o
INTERFACES =
UICDECLS =
UICIMPLS =
SRCMOC	=
OBJMOC	=
DIST	=
TARGET	=	spiralsynthmodular

SUBDIRS	=	@PLUGINLIST@

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules


all: $(TARGET)
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		(cd SpiralSound/Plugins/$$subdir ; make) \
		done;

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)

moc: $(SRCMOC)

tmake: Makefile

Makefile: SpiralSynthModular.pro
	tmake SpiralSynthModular.pro -o Makefile

dist:
	$(TAR) SpiralSynthModular.tar SpiralSynthModular.pro $(SOURCES) $(HEADERS) $(INTERFACES) $(DIST)
	$(GZIP) SpiralSynthModular.tar

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		(cd SpiralSound/Plugins/$$subdir ; make clean) \
		done;

install: spiralsynthmodular
	$(INSTALL) spiralsynthmodular $(bindir)
	$(INSTALL) -d $(SpiralPlugins)
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		(cd SpiralSound/Plugins/$$subdir ; make install) \
		done;

uninstall:
	rm -f $(bindir)/spiralsynthmodular
	rm -rf $(SpiralPlugins)

####### Sub-libraries


###### Combined headers


####### Compile

main.o : 	main.cpp \
		SpiralSynthModular.h

SpiralSynthModular.o: SpiralSynthModular.C \
		SpiralSynthModular.h \
		SpiralSound/SpiralInfo.h \
		SettingsWindow.h \
		GUI/SSM.xpm \
		GUI/load.xpm \
		GUI/save.xpm \
		GUI/new.xpm \
		GUI/options.xpm \
		GUI/comment.xpm \
                GUI/Widgets/SpiralGUI.H \
                GUI/Widgets/Fl_DeviceGUI.h \
		GUI/Widgets/Fl_CommentGUI.h \
		GUI/Widgets/Fl_Canvas.h \
		GUI/Widgets/PawfalYesNo.h \
		GUI/Widgets/PawfalInput.h \
		SpiralSound/PluginManager.h \
		SpiralSound/Plugins/SpiralPlugin.h

GraphSort.o: GraphSort.C \
		GraphSort.h

SpiralSound/ChannelHandler.o: SpiralSound/ChannelHandler.C \
		SpiralSound/ChannelHandler.h

GUI/Widgets/Fl_DeviceGUI.o: GUI/Widgets/Fl_DeviceGUI.C \
                GUI/Widgets/SpiralGUI.H \
		GUI/Widgets/Fl_DeviceGUI.h \
		GUI/Widgets/Fl_DragBar.H \
		GUI/Widgets/Fl_Canvas.h \
		GraphSort.h \
		SpiralSound/SpiralInfo.h

GUI/Widgets/Fl_CommentGUI.o: GUI/Widgets/Fl_CommentGUI.C \
                GUI/Widgets/SpiralGUI.H \
		GUI/Widgets/Fl_CommentGUI.h \
		GUI/Widgets/Fl_DeviceGUI.h \
		GUI/Widgets/Fl_DragBar.H \
		SpiralSound/SpiralInfo.h

GUI/Widgets/Fl_DragBar.o: GUI/Widgets/Fl_DragBar.cxx \
		GUI/Widgets/Fl_DragBar.H

GUI/Widgets/Fl_Canvas.o: GUI/Widgets/Fl_Canvas.C \
		GUI/Widgets/Fl_Canvas.h \
                GUI/Widgets/SpiralGUI.H \
		GUI/Widgets/Fl_DeviceGUI.h \
		GUI/Widgets/Fl_DragBar.H \
		GraphSort.h \
		SpiralSound/SpiralInfo.h

GUI/Widgets/PawfalInput.o: GUI/Widgets/PawfalInput.C \
		GUI/Widgets/PawfalInput.h

GUI/Widgets/PawfalYesNo.o: GUI/Widgets/PawfalYesNo.C \
		GUI/Widgets/PawfalYesNo.h

SpiralSound/SpiralInfo.o: SpiralSound/SpiralInfo.C \
		SpiralSynthPluginLocation.h \
		SpiralSound/SpiralInfo.h \
		SpiralSound/Sample.h

SpiralSound/Sample.o: SpiralSound/Sample.C \
		SpiralSound/Sample.h \
		SpiralSound/SpiralInfo.h

SpiralSound/Plugins/SpiralPlugin.o: SpiralSound/Plugins/SpiralPlugin.C \
		SpiralSound/Plugins/SpiralPlugin.h \
		SpiralSound/Sample.h \
		SpiralSound/SpiralInfo.h \
		SpiralSound/Sample.h

SpiralSound/Plugins/SpiralPluginGUI.o: SpiralSound/Plugins/SpiralPluginGUI.C \
		SpiralSound/Plugins/SpiralPluginGUI.h \
		SpiralSound/Plugins/SpiralPlugin.h \
		SpiralSound/Sample.h \
		SpiralSound/SpiralInfo.h \
		SpiralSound/Sample.h

SpiralSound/PluginManager.o: SpiralSound/PluginManager.C \
		SpiralSound/PluginManager.h \
		SpiralSound/Sample.h \
		SpiralSound/SpiralInfo.h \
		SpiralSound/Sample.h \
		SpiralSound/Plugins/SpiralPlugin.h

SettingsWindow.o: SettingsWindow.C \
		SettingsWindow.h \
		SpiralSynthModular.h \
		GraphSort.h \
		SpiralSound/SpiralInfo.h \
		SpiralSound/Sample.h \
		SpiralSound/Plugins/SpiralPlugin.h \
                GUI/Widgets/SpiralGUI.H \
		GUI/Widgets/Fl_DeviceGUI.h \
		GUI/Widgets/Fl_DragBar.H \
		GUI/Widgets/Fl_CommentGUI.h \
		GUI/Widgets/Fl_Canvas.h \
		GUI/options.xpm
