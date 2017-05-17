/*  SpiralSynthModular
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <FL/Fl.H>
#include <FL/Fl_Tooltip.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "SpiralSynthModular.h"
#include "SpiralSound/SpiralInfo.h"

pthread_t loopthread,watchdogthread;
SynthModular *synth;

char watchdog_check = 1;
char gui_watchdog_check = 1;

int pthread_create_realtime (pthread_t *new_thread,
			 void *(*start)(void *), void *arg,
			 int priority);

bool CallbackOnly = false;
bool FIFO = false;
bool GUI = true;

/////////////////////////////////////////////////////////////

void watchdog (void *arg)
{
	pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	watchdog_check = 0;
	gui_watchdog_check = 0;

	while (1)
	{
		usleep (10000000);
		// gui watchdog goes off with modal dialog boxes
		if (watchdog_check == 0)// || gui_watchdog_check== 0)
		{
			cerr<<"ssm watchdog: timeout - killing ssm"<<endl;
			if (watchdog_check==0) cerr<<"diagnosis: audio hung?"<<endl;
			if (gui_watchdog_check==0) cerr<<"diagnosis: gui starved by audio"<<endl;
			exit (1);
		}
		watchdog_check = 0;
		gui_watchdog_check = 0;
	}
}

///////////////////////////////////////////////////////////////////////

void audioloop(void* o)
{
	while(1)
	{
		if (!synth->CallbackMode())
		{
			// do funky stuff
			synth->Update();

			// put the brakes on if there is no blocking output running
			if (!synth->IsBlockingOutputPluginReady()||
				 synth->IsPaused())
			{
				usleep(10000);
			}
		}
		else
		{
			// the engine is currently in callback mode, so we don't
			// need to do anything unless we are switched back
			usleep(1000000);
		}

		watchdog_check = 1;
	}
}

//////////////////////////////////////////////////////
#if __APPLE__
#include <CoreFoundation/CFBundle.h>
#include <libgen.h>
#endif

#include "GraphSort.h"

int main(int argc, char **argv)
{
#if __APPLE__
	// --with-plugindir=./Libraries
	system("pwd");
	CFBundleRef main	= CFBundleGetMainBundle();
	CFURLRef	url		= main ? CFBundleCopyExecutableURL(main) : NULL;
	CFStringRef path	= url ? CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle) : NULL;
	char *		dst		= (char*)CFStringGetCStringPtr(path, 0);

	printf("main %p url %p path %p dst %p", main, url, path, dst);
	if (dst) {
		printf("Have a valid name '%s'\n", dst);
		chdir(dirname(dst));
		chdir("..");
	} else
		printf("No base pathname\n");
#endif
        srand(time(NULL));
	SpiralInfo::Get()->LoadPrefs();

	// get args
    string cmd_filename="";
    bool cmd_specd = false;
	string cmd_pluginPath="";
	// parse the args
    if (argc>1)
	{
		for (int a=1; a<argc; a++)
		{
			if (!strcmp(argv[a],"--NoGUI")) GUI = false;
			else if (!strcmp(argv[a],"--Realtime")) FIFO = true;
			else if (!strcmp(argv[a],"-h"))
			{
				cout<<"usage: spiralsynthmodular [patch.ssm] [options]"<<endl<<endl
				<<"options list"<<endl
				<<"-h : help"<<endl
				<<"-v : print version"<<endl
				<<"--NoGUI : run without GUI (only useful when loading patch from command line"<<endl
				<<"--Realtime : spawn audio thread with FIFO scheduling (run as root)"<<endl
				<<"--PluginPath <PATH> : look for plugins in the specified directory"<<endl;
				exit(0);
			}
			else if (!strcmp(argv[a],"-v")) 
			{
				cout<<VER_STRING<<endl; exit(0);
			} 			
			else if (!strcmp(argv[a],"--PluginPath"))
			{
				a++;
				cmd_pluginPath=argv[a];
			}
			else 
			{
				cmd_filename = argv[1];
				cmd_specd = true;
			}
		}
    }	
	
	// set some fltk defaults
	Fl_Tooltip::size(10);	
	Fl::visible_focus(false);
	Fl::visual(FL_DOUBLE|FL_RGB);
	
	synth=new SynthModular;

	// setup the synth
	Fl_Window* win = synth->CreateWindow();
	
	synth->LoadPlugins(cmd_pluginPath);
	win->xclass("");
	if (GUI) win->show(1, argv); // prevents stuff happening before the plugins have loaded
	
	// spawn the audio thread
	if (FIFO) 
	{	
		pthread_create_realtime(&watchdogthread,(void*(*)(void*))watchdog,NULL,sched_get_priority_max(SCHED_FIFO));
		pthread_create_realtime(&loopthread,(void*(*)(void*))audioloop,NULL,sched_get_priority_max(SCHED_FIFO)-1);
	}
	else 
	{
		pthread_create(&loopthread,NULL,(void*(*)(void*))audioloop,NULL);
		// reduce the priority of the gui
		if (setpriority(PRIO_PROCESS,0,20)) cerr<<"Could not set priority for GUI thread"<<endl;
	}
	
	// do we need to load a patch on startup? 
    if (cmd_specd) synth->LoadPatch(cmd_filename.c_str());        
	
	if (!GUI)
	{	
		// if there is no gui needed, there is nothing more to do here
		Fl::check();
		for (;;) sleep(1);
	}
	
    for (;;)
	{
    	if (!Fl::check()) break;    	
		synth->UpdatePluginGUIs(); // deletes any if necc
		usleep(10000);
		gui_watchdog_check=1;
  	}
	
	//pthread_cancel(loopthread);
        delete synth;

	return 1;
}

// nicked from Paul Barton-Davis' Ardour code :)
int pthread_create_realtime (pthread_t *new_thread,
			 void *(*start)(void *), void *arg,
			 int priority)

{	
	pthread_attr_t *rt_attributes;
	struct sched_param *rt_param;
	int retval;

	rt_attributes = (pthread_attr_t *) malloc (sizeof (pthread_attr_t));
	rt_param = (struct sched_param *) malloc (sizeof (struct sched_param));

	pthread_attr_init (rt_attributes);

	if (seteuid (0)) {
		cerr << "Cannot obtain root UID for RT scheduling operations"
		      << endl;
		return -1;

	} else {

	    if (pthread_attr_setschedpolicy (rt_attributes, SCHED_FIFO)) {
		cerr << "Cannot set FIFO scheduling attributes for RT thread" << endl;

	    }
	    
	    if (pthread_attr_setscope (rt_attributes, PTHREAD_SCOPE_SYSTEM)) {
		cerr << "Cannot set scheduling scope for RT thread" << endl;

	    }
	    
	    rt_param->sched_priority = priority;
	    if (pthread_attr_setschedparam (rt_attributes, rt_param)) {
		cerr << "Cannot set scheduling priority for RT thread" << endl;

	    }
	}

	retval = pthread_create (new_thread, rt_attributes, start, arg);
	seteuid (getuid());

	return retval;
}
