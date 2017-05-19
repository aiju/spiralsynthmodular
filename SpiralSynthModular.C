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

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tooltip.H>

#include "SpiralSynthModular.h"
#include "SpiralSound/PluginManager.h"
#include "SpiralSound/SpiralInfo.h"
#include "SpiralSound/Plugins/SpiralPluginGUI.h"

#include "GUI/SSM.xpm"
#include "GUI/load.xpm"
#include "GUI/save.xpm"
#include "GUI/new.xpm"
#include "GUI/options.xpm"
#include "GUI/comment.xpm"
#include "GUI/Widgets/PawfalYesNo.h"

//#define DEBUG_PLUGINS
//#define DEBUG_STREAM

const static string LABEL = "SpiralSynthModular "+VER_STRING;
static string TITLEBAR;

static const int FILE_VERSION = 4;
static int Numbers[512];

static const int MAIN_WIDTH     = 700;
static const int MAIN_HEIGHT    = 600;
static const int SLIDER_WIDTH   = 15;
static const int ICON_DEPTH     = 3;
static const int COMMENT_ID     = -1;

using namespace std;

map<int,DeviceWin*> SynthModular::m_DeviceWinMap;
RWLock SynthModular::m_DeviceWinMap_lock;
bool SynthModular::m_CallbackUpdateMode = false;
bool SynthModular::m_BlockingOutputPluginIsReady = false;

//////////////////////////////////////////////////////////

DeviceWin::~DeviceWin()
{
}

//////////////////////////////////////////////////////////

SynthModular::SynthModular():
m_NextID(0),
m_PauseAudio(false)
{
	m_Info.BUFSIZE = SpiralInfo::BUFSIZE;
	m_Info.FRAGSIZE = SpiralInfo::FRAGSIZE;
	m_Info.FRAGCOUNT = SpiralInfo::FRAGCOUNT;
	m_Info.SAMPLERATE = SpiralInfo::SAMPLERATE;
	m_Info.OUTPUTFILE = SpiralInfo::OUTPUTFILE;
	m_Info.MIDIFILE = SpiralInfo::MIDIFILE;
	m_Info.POLY = SpiralInfo::POLY;
        m_Info.GUI_COLOUR = SpiralInfo::GUI_COLOUR;
        m_Info.SCOPE_BG_COLOUR = SpiralInfo::SCOPE_BG_COLOUR;
        m_Info.SCOPE_FG_COLOUR = SpiralInfo::SCOPE_FG_COLOUR;
        m_Info.SCOPE_SEL_COLOUR = SpiralInfo::SCOPE_SEL_COLOUR;
        m_Info.SCOPE_IND_COLOUR = SpiralInfo::SCOPE_IND_COLOUR;
        m_Info.SCOPE_MRK_COLOUR = SpiralInfo::SCOPE_MRK_COLOUR;
        m_Info.GUICOL_Device = SpiralInfo::GUICOL_Device;
        m_Info.GUIDEVICE_Box = SpiralInfo::GUIDEVICE_Box;

        for (int n=0; n<512; n++) Numbers[n]=n;

	m_CH.Register("PauseAudio",&m_PauseAudio);
}

//////////////////////////////////////////////////////////

SynthModular::~SynthModular()
{
	ClearUp();
	PluginManager::Get()->PackUpAndGoHome();
}

//////////////////////////////////////////////////////////

void SynthModular::ClearUp()
{
	PauseAudio();

	WLocker wlocker(&m_DeviceWinMap_lock);
	for(map<int,DeviceWin*>::iterator i=m_DeviceWinMap.begin();
		i!=m_DeviceWinMap.end(); i++)
	{
		//Stop processing of audio if any
		if (i->second->m_Device)
		{
			if (i->second->m_Device->Kill());
		}			
		i->second->m_DeviceGUI->Clear();

		if (i->second->m_DeviceGUI->GetPluginWindow())
		{
			i->second->m_DeviceGUI->GetPluginWindow()->hide();
		}

		//Delete Device
		delete i->second->m_Device;
		i->second->m_Device=NULL;
	}

	m_Canvas->Clear();
	m_DeviceWinMap.clear();
	m_NextID=0;

	ResumeAudio();
}

//////////////////////////////////////////////////////////
void SynthModular::Update()
{
	int morituri;

	m_CH.UpdateDataNow();

	if (m_PauseAudio) return;

	// for all the plugins
	morituri = 0;
	m_DeviceWinMap_lock.rlock();
	for(map<int,DeviceWin*>::iterator i=m_DeviceWinMap.begin(); i!=m_DeviceWinMap.end(); i++)
		if(i->second->m_Device && i->second->m_Device->IsDead()){
			morituri = 1;
			break;
		}
	m_DeviceWinMap_lock.runlock();
	
	if(morituri > 0){
		WLocker wlocker(&m_DeviceWinMap_lock);
		for(map<int,DeviceWin*>::iterator i=m_DeviceWinMap.begin(); i!=m_DeviceWinMap.end(); )
		{
			if (i->second->m_Device && i->second->m_Device->IsDead())			
			{
				//Delete Device
				delete i->second->m_Device;
				i->second->m_Device=NULL;
				
				//Erase Device from DeviceWinMap
				m_DeviceWinMap.erase(i++);
				continue;
			}
			i++;
		}
	}

	RLocker rlocker(&m_DeviceWinMap_lock);
	for(map<int,DeviceWin*>::iterator i=m_DeviceWinMap.begin(); i!=m_DeviceWinMap.end(); i++)
		if (i->second->m_Device) // if it's not a comment
		{
			#ifdef DEBUG_PLUGINS
			cerr<<"Updating channelhandler of plugin "<<i->second->m_PluginID<<endl;
			#endif

			// updates the data from the gui thread, if it's not blocking
 			i->second->m_Device->UpdateChannelHandler();

			#ifdef DEBUG_PLUGINS
			cerr<<"Finished updating"<<endl;
			#endif

			// run any commands we've received from the GUI's
			i->second->m_Device->ExecuteCommands();
		}

	// run the plugins (only ones connected to anything)
	list<int> ExecutionOrder = m_Canvas->GetGraph()->GetSortedList();
	for (list<int>::reverse_iterator i=ExecutionOrder.rbegin();
		 i!=ExecutionOrder.rend(); i++)
	{
		// use the graphsort order to remove internal latency
		map<int,DeviceWin*>::iterator di=m_DeviceWinMap.find(*i);
		if (di!=m_DeviceWinMap.end() && di->second->m_Device  && (! di->second->m_Device->IsDead()))
		{
			#ifdef DEBUG_PLUGINS
			cerr<<"Executing plugin "<<di->second->m_PluginID<<endl;
			#endif

			di->second->m_Device->Execute();

			#ifdef DEBUG_PLUGINS
			cerr<<"Finished executing"<<endl;
			#endif
		}
	}
}

//////////////////////////////////////////////////////////

void SynthModular::UpdatePluginGUIs()
{
	int morituri;
	
	// see if any need deleting
	morituri = 0;
	m_DeviceWinMap_lock.rlock();
	for (map<int,DeviceWin*>::iterator i=m_DeviceWinMap.begin(); i!=m_DeviceWinMap.end(); i++)
	{
		if (i->second->m_DeviceGUI && i->second->m_DeviceGUI->GetPluginWindow())
		{
			SpiralPluginGUI *GUI=(SpiralPluginGUI *)i->second->m_DeviceGUI->GetPluginWindow();
			GUI->Update();
		}
		if (i->second->m_DeviceGUI && i->second->m_DeviceGUI->Killed())
			morituri = 1;

	}
	m_DeviceWinMap_lock.runlock();
	
	if(morituri){
		WLocker wlocker(&m_DeviceWinMap_lock);
		for (map<int,DeviceWin*>::iterator i=m_DeviceWinMap.begin(); i!=m_DeviceWinMap.end(); ){
			if (i->second->m_DeviceGUI && i->second->m_DeviceGUI->Killed())
			{
				bool erase = true;
				
				//Stop processing of audio if any
				if (i->second->m_Device)
				{
					if (i->second->m_Device->Kill());
					erase = false;
				}			
		
				//Clear GUI Device
				i->second->m_DeviceGUI->Clear();
	
				// Hide Device GUI FIRST
				if (i->second->m_DeviceGUI->GetPluginWindow())
				{
					i->second->m_DeviceGUI->GetPluginWindow()->hide();
				}
	
				//Remove Device GUI from canvas
				m_Canvas->RemoveDevice(i->second->m_DeviceGUI);
	
				//Delete Device GUI - must delete here or sometimes plugin will randomly crash
				delete i->second->m_DeviceGUI;
				i->second->m_DeviceGUI = NULL;
	
				//Erase from winmap if no audio to do it
				if (erase){
					m_DeviceWinMap.erase(i++);
					continue;
				}
			}
			i++;
		}
	}

	m_Canvas->Poll();
}

//////////////////////////////////////////////////////////

SpiralWindowType *SynthModular::CreateWindow()
{
	m_TopWindow = new SpiralWindowType(MAIN_WIDTH, MAIN_HEIGHT, LABEL.c_str());
	//m_TopWindow->resizable(m_TopWindow);

	int but=60;
        int ToolbarHeight=but+0;

        m_Topbar = new Fl_Pack (0, 0, MAIN_WIDTH, ToolbarHeight, "");
        m_Topbar->user_data((void*)(this));
       	m_Topbar->type(FL_HORIZONTAL);
		m_Topbar->color(SpiralInfo::GUICOL_Button);
        m_TopWindow->add(m_Topbar);

        m_ToolbarPanel = new Fl_Pack (0, 0, but*5, ToolbarHeight, "");
        m_ToolbarPanel->user_data((void*)(this));
       	m_ToolbarPanel->type(FL_VERTICAL);
		m_ToolbarPanel->color(SpiralInfo::GUICOL_Button);
        m_Topbar->add(m_ToolbarPanel);

        m_Toolbar = new Fl_Pack (0, 0, but*5, but, "");
        m_Toolbar->user_data((void*)(this));
       	m_Toolbar->type(FL_HORIZONTAL);
		m_Toolbar->color(SpiralInfo::GUICOL_Button);
        m_ToolbarPanel->add(m_Toolbar);

        m_Load = new Fl_Button(0, 0, but, but, "");
	Fl_Pixmap *tPix = new Fl_Pixmap(load_xpm);
	m_Load->image(tPix->copy());
	delete tPix;
        m_Load->type(0);
        m_Load->labelsize (1);
        m_Load->tooltip("Load a patch file");
	m_Load->callback((Fl_Callback*)cb_Load);
	m_Toolbar->add(m_Load);

	m_Save = new Fl_Button(0, 0, but, but, "");
	tPix = new Fl_Pixmap(save_xpm);
	m_Save->image(tPix->copy());
	delete tPix;
        m_Save->type(0);
        m_Save->labelsize (1);
 	m_Save->tooltip("Save a patch file");
	m_Save->callback((Fl_Callback*)cb_Save);
	m_Toolbar->add(m_Save);

	m_New = new Fl_Button(0, 0, but, but, "");
	tPix = new Fl_Pixmap(new_xpm);
	m_New->image(tPix->copy());
	delete tPix;
        m_New->type(0);
  	m_New->labelsize (1);
        m_New->tooltip("New patch");
	m_New->callback((Fl_Callback*)cb_New);
	m_Toolbar->add(m_New);

	m_Options = new Fl_Button(0, 0, but, but, "");
	tPix = new Fl_Pixmap(options_xpm);
	m_Options->image(tPix->copy());
	delete tPix;
        m_Options->type(0);
 	m_Options->labelsize (1);
        m_Options->tooltip("Options");
	m_Options->callback((Fl_Callback*)cb_Rload);
	m_Toolbar->add(m_Options);

	m_NewComment = new Fl_Button(0, 0, but, but, "");
	tPix = new Fl_Pixmap(comment_xpm);
	m_NewComment->image(tPix->copy());
	delete tPix;
        m_NewComment->type(0);
        m_NewComment->labelsize (1);
        m_NewComment->tooltip("New comment");
	m_NewComment->callback((Fl_Callback*)cb_NewComment);
	m_Toolbar->add(m_NewComment);

        m_GroupFiller = new Fl_Group (but*5, 0, 0, ToolbarHeight, "");
		m_GroupFiller->color(SpiralInfo::GUICOL_Button);
	m_Topbar->add (m_GroupFiller);

       	m_GroupTab = new Fl_Tabs (but*5 + m_GroupFiller->w(), 0, MAIN_WIDTH-m_GroupFiller->w()-but*5, ToolbarHeight, "");
        m_GroupTab->user_data ((void*)(this));
        m_GroupTab->callback((Fl_Callback*)cb_GroupTab);
	m_Topbar->add (m_GroupTab);

       	m_Topbar->resizable(m_GroupTab);

        /////////////////

	m_CanvasScroll = new Fl_Scroll(0, ToolbarHeight, MAIN_WIDTH, MAIN_HEIGHT-ToolbarHeight, "");
        m_TopWindow->add(m_CanvasScroll);
	m_TopWindow->resizable(m_CanvasScroll);

	m_Canvas = new Fl_Canvas(-5000, -5000, 10000, 10000, "");
        m_Canvas->type(1);
	m_Canvas->box(FL_FLAT_BOX);
        m_Canvas->labeltype(FL_ENGRAVED_LABEL);
        m_Canvas->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	m_Canvas->color(SpiralInfo::GUICOL_Canvas);
	m_Canvas->user_data((void*)(this));
	m_Canvas->SetConnectionCallback((Fl_Callback*)cb_Connection);
	m_Canvas->SetUnconnectCallback((Fl_Callback*)cb_Unconnect);
	m_Canvas->SetAddDeviceCallback((Fl_Callback*)cb_NewDeviceFromMenu);
	m_CanvasScroll->add(m_Canvas);

	m_SettingsWindow = new SettingsWindow;
	m_SettingsWindow->RegisterApp(this);
	
	Fl_Tooltip::size(12);

	return m_TopWindow;
}

//////////////////////////////////////////////////////////

vector<string> SynthModular::BuildPluginList (const string &Path) {
	// Scan plugin path for plugins.
	DIR *dp;
	struct dirent *ep;
	struct stat sb;
	void *handle;
	string fullpath;
	const char *path = Path.c_str();
	vector<string> ret;

	dp = opendir(path);
	if (!dp) {
	   cerr << "WARNING: Could not open path " << path << endl;
	}
	else {
		while ((ep = readdir(dp))) {
			// Need full path
			fullpath = path;
			fullpath.append(ep->d_name);

                        // Stat file to get type
			if (!stat(fullpath.c_str(), &sb)) {
				// We only want regular files
				if (S_ISREG(sb.st_mode))  {
                                        // We're not fussed about resolving symbols yet, since we are just
                                        // checking if it's a DLL.
					handle = dlopen(fullpath.c_str(), RTLD_LAZY);
					if (!handle) {
						cerr << "WARNING: File " << path << ep->d_name
							<< " could not be examined" << endl;
						cerr << "dlerror() output:" << endl;
						cerr << dlerror() << endl;
					}
					else {
						// It's a DLL. Add name to list
						ret.push_back(ep->d_name);
					}
                                }
                        }
		}
	}
        return ret;
}

void SynthModular::LoadPlugins(string pluginPath)
{

	int Width  = 35;
	int Height = 35;

	int SWidth  = 256;
	int SHeight = 256;

	Fl_Pixmap pic(SSM_xpm);

	Fl_Double_Window* Splash = new Fl_Double_Window((Fl::w()/2)-(SWidth/2),
									                (Fl::h()/2)-(SHeight/2),
									                SWidth,SHeight,"SSM");
	Splash->border(0);

	Fl_Box* pbut = new Fl_Box(0,8,SWidth,SHeight,"");
	pbut->box(FL_NO_BOX);
	pic.label(pbut);
	Fl_Box *splashtext = new Fl_Box(5,SHeight-20,200,20,"Loading...");
	splashtext->labelsize(10);
	splashtext->box(FL_NO_BOX);
	splashtext->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
	Splash->add(pbut);
	Splash->add(splashtext);
	Splash->show();

        int ID=-1;

	vector<string> PluginVector;

	if (SpiralInfo::USEPLUGINLIST) {
	   PluginVector=SpiralInfo::PLUGINVEC;
	}
	else {
           if (pluginPath.empty()) {
              PluginVector=BuildPluginList (SpiralInfo::PLUGIN_PATH);
           }
           else {
              string::iterator i = pluginPath.end() - 1;
              if (*i != '/') pluginPath += '/';
              PluginVector=BuildPluginList(pluginPath);
           }
	}

	for (vector<string>::iterator i=PluginVector.begin();
		 i!=PluginVector.end(); i++)
	{
		string Fullpath;
		if (pluginPath=="")
		{
			Fullpath=SpiralInfo::PLUGIN_PATH+*i;
		}
		else
		{
			Fullpath=pluginPath+*"/"+*i;
		}

		ID=PluginManager::Get()->LoadPlugin(Fullpath.c_str());
		if (ID!=PluginError)
		{
			#ifdef DEBUG_PLUGINS
			cerr << ID << " = Plugin [" << *i << "]" << endl;
			#endif

			Fl_ToolButton *NewButton = new Fl_ToolButton(0,0,Width,Height,"");
                        NewButton->user_data((void*)(this));
			NewButton->labelsize(1);
			Fl_Pixmap *tPix = new Fl_Pixmap(PluginManager::Get()->GetPlugin(ID)->GetIcon());
			NewButton->image(tPix->copy(tPix->w(),tPix->h()));
			delete tPix;

			string GroupName = PluginManager::Get()->GetPlugin(ID)->GetGroupName();
                        Fl_Pack* the_group=NULL;

                        // find or create this group, and add an icon
			map<string,Fl_Pack*>::iterator gi=m_PluginGroupMap.find(GroupName);
			if (gi==m_PluginGroupMap.end())
			{
                                the_group = new Fl_Pack (m_GroupTab->x(), 16, m_GroupTab->w(), m_GroupTab->h()-15, GroupName.c_str());
	                        the_group->type(FL_HORIZONTAL);
                                the_group->labelsize(12);
                                the_group->user_data((void*)(this));
                                m_GroupTab->value(the_group);
				m_PluginGroupMap[GroupName]=the_group;
			}
			else
			{
				the_group=gi->second;
			}
                        NewButton->type(0);
			NewButton->box(FL_NO_BOX);
			NewButton->down_box(FL_NO_BOX);
			//NewButton->color(SpiralInfo::GUICOL_Button);
			//NewButton->selection_color(SpiralInfo::GUICOL_Button);
			the_group->add(NewButton);

			string tooltip=*i;
            		// find the first / if there is one, and get rid of everything before and including it
			unsigned int p = tooltip.find ('/');
			if (p < tooltip.length()) tooltip.erase (0, p);
			// find last . and get rid of everything after and including it
			p = tooltip.rfind ('.');
			unsigned int l = tooltip.length ();
			if (p < l) tooltip.erase (p, l);
			m_Canvas->AddPluginName (tooltip, PluginManager::Get()->GetPlugin(ID)->ID);
			splashtext->label (tooltip.c_str());
               		Splash->redraw();
			NewButton->tooltip (tooltip.c_str());

			NewButton->callback((Fl_Callback*)cb_NewDevice,&Numbers[ID]);
			NewButton->show();
			m_DeviceVec.push_back(NewButton);

			the_group->redraw();

//			m_NextPluginButton++;
			Fl::check();
		}
	}
        map<string,Fl_Pack*>::iterator PlugGrp;

        for (PlugGrp = m_PluginGroupMap.begin(); PlugGrp!= m_PluginGroupMap.end(); ++PlugGrp)
		{
                m_GroupTab->add(PlugGrp->second);
				PlugGrp->second->add(new Fl_Box(0,0,600,100,""));
		}

	// try to show the SpiralSound group
        PlugGrp = m_PluginGroupMap.find("SpiralSound");
	// can't find it - show the first plugin group
	if (PlugGrp==m_PluginGroupMap.end()) PlugGrp=m_PluginGroupMap.begin();
        m_GroupTab->value(PlugGrp->second);

	m_Canvas->SortPluginList();

	Splash->hide();
	delete Splash;
}

//////////////////////////////////////////////////////////

DeviceGUIInfo SynthModular::BuildDeviceGUIInfo(PluginInfo &PInfo)
{
	DeviceGUIInfo Info;
	int Height=50;

	// tweak the size if we have too many ins/outs
	if (PInfo.NumInputs>4 || PInfo.NumOutputs>4)
	{
		if (PInfo.NumInputs<PInfo.NumOutputs)
		{
			Height=PInfo.NumOutputs*10+5;
		}
		else
		{
			Height=PInfo.NumInputs*10+5;
		}
	}

	// Make the guiinfo struct
	Info.XPos       = 0;
	Info.YPos       = 0;
	Info.Width      = 40;
	Info.Height     = Height;
	Info.NumInputs  = PInfo.NumInputs;
	Info.NumOutputs = PInfo.NumOutputs;
	Info.Name       = PInfo.Name;
	Info.PortTips   = PInfo.PortTips;
	Info.PortTypes  = PInfo.PortTypes;

	return Info;
}

//////////////////////////////////////////////////////////

DeviceWin* SynthModular::NewDeviceWin(int n, int x, int y)
{
	DeviceWin *nlw = new DeviceWin;
	const HostsideInfo* Plugin=PluginManager::Get()->GetPlugin(n);

	if (!Plugin) return NULL;

	nlw->m_Device=Plugin->CreateInstance();

	if (!nlw->m_Device) return NULL;

        nlw->m_Device->SetBlockingCallback(cb_Blocking);
	nlw->m_Device->SetUpdateCallback(cb_Update);
	nlw->m_Device->SetParent((void*)this);

	PluginInfo PInfo    = nlw->m_Device->Initialise(&m_Info);
	SpiralGUIType *temp = nlw->m_Device->CreateGUI();
	Fl_Pixmap *Pix      = new Fl_Pixmap(Plugin->GetIcon());
	nlw->m_PluginID     = n;

	if (temp) temp->position(x+10,y);

	DeviceGUIInfo Info=BuildDeviceGUIInfo(PInfo);

	Info.XPos       = x; //TOOLBOX_WIDTH+(rand()%400);
	Info.YPos       = y; //rand()%400;

	nlw->m_DeviceGUI = new Fl_DeviceGUI(Info, temp, Pix, nlw->m_Device->IsTerminal());
	m_Canvas->add(nlw->m_DeviceGUI);
	m_Canvas->redraw();

	return nlw;
}

//////////////////////////////////////////////////////////

void SynthModular::AddDevice(int n, int x=-1, int y=-1)
{
	//cerr<<"Adding "<<m_NextID<<endl;

	if (x==-1)
	{
		x = m_CanvasScroll->x()+50;
		y = m_CanvasScroll->y()+50;
	}

	DeviceWin* temp = NewDeviceWin(n,x,y);
	if (temp)
	{
		int ID=m_NextID++;
		//cerr<<"adding device "<<ID<<endl;
		temp->m_DeviceGUI->SetID(ID);
		temp->m_Device->SetUpdateInfoCallback(ID,cb_UpdatePluginInfo);
		WLocker rlocker(&m_DeviceWinMap_lock);
		m_DeviceWinMap[ID]=temp;
	}
}

//////////////////////////////////////////////////////////

DeviceWin* SynthModular::NewComment(int n, int x=-1, int y=-1)
{
	DeviceWin *nlw = new DeviceWin;

	if (x==-1)
	{
		x = m_CanvasScroll->x()+50;
		y = m_CanvasScroll->y()+50;
	}

	nlw->m_Device=NULL;
	nlw->m_PluginID  = COMMENT_ID;

	DeviceGUIInfo Info;

	Info.XPos       = x;
	Info.YPos       = y;
	Info.Width      = 50;
	Info.Height     = 20;
	Info.NumInputs  = 0;
	Info.NumOutputs = 0;
	Info.Name = "";

	nlw->m_DeviceGUI = new Fl_CommentGUI(Info, NULL, NULL);

	m_Canvas->add(nlw->m_DeviceGUI);
	m_Canvas->redraw();

	return nlw;
}

//////////////////////////////////////////////////////////

void SynthModular::AddComment(int n)
{
	//cerr<<"Adding "<<m_NextID<<endl;
	DeviceWin* temp = NewComment(n);
	if (temp)
	{
		int ID=m_NextID++;
		//cerr<<"adding comment "<<ID<<endl;
		temp->m_DeviceGUI->SetID(ID);
		WLocker rlocker(&m_DeviceWinMap_lock);
		m_DeviceWinMap[ID]=temp;
	}
}

//////////////////////////////////////////////////////////

void SynthModular::UpdateHostInfo()
{
	// used to use string streams, but this seems to cause a compiler bug
	// at the moment, so fall back to using a temporary file

	//std::stringstream str;
	fstream ofs("___temp.ssmtmp",ios::out);
	//str<<*this;
	ofs<<*this;

	ClearUp();

	// update the settings
	m_Info.BUFSIZE    = SpiralInfo::BUFSIZE;
	m_Info.FRAGSIZE   = SpiralInfo::FRAGSIZE;
	m_Info.FRAGCOUNT  = SpiralInfo::FRAGCOUNT;
	m_Info.SAMPLERATE = SpiralInfo::SAMPLERATE;
	m_Info.OUTPUTFILE = SpiralInfo::OUTPUTFILE;
	m_Info.MIDIFILE   = SpiralInfo::MIDIFILE;
	m_Info.POLY       = SpiralInfo::POLY;

	fstream ifs("___temp.ssmtmp",ios::in);
	//str>>*this;
	ifs>>*this;

	system("rm -f ___temp.ssmtmp");

}

//////////////////////////////////////////////////////////

// called when a callback output plugin wants to run the audio thread
void SynthModular::cb_Update(void* o, bool mode)
{
	m_CallbackUpdateMode=mode;
	((SynthModular*)o)->Update();
}

// called by a blocking output plugin to notify the engine its ready to
// take control of the update timing (so take the brakes off)
void SynthModular::cb_Blocking(void* o, bool mode)
{
	m_BlockingOutputPluginIsReady=mode;
}

//////////////////////////////////////////////////////////

istream &operator>>(istream &s, SynthModular &o)
{
	o.PauseAudio();

	string dummy,dummy2;
	int ver;
	s>>dummy>>dummy>>dummy>>ver;

	if (ver>FILE_VERSION)
	{
		SpiralInfo::Alert("Bad file, or more recent version.");
		return s;
	}

	if (ver>2)
	{
		int MainWinX,MainWinY,MainWinW,MainWinH;
		int EditWinX,EditWinY,EditWinW,EditWinH;

		s>>MainWinX>>MainWinY>>MainWinW>>MainWinH;
		s>>EditWinX>>EditWinY>>EditWinW>>EditWinH;

		//o.m_MainWindow->resize(MainWinX,MainWinY,MainWinW,MainWinH);
		//o.m_EditorWindow->resize(EditWinX,EditWinY,EditWinW,EditWinH);
	}

	int Num, ID, PluginID, x,y,ps,px,py;
	s>>dummy>>Num;

	for(int n=0; n<Num; n++)
	{
		#ifdef DEBUG_STREAM
		cerr<<"Loading Device "<<n<<endl;
		#endif

		s>>dummy; // "Device"
		s>>ID;
		s>>dummy2; // "Plugin"
		s>>PluginID;
		s>>x>>y;

		string Name;

		if (ver>3)
		{
			// load the device name
			int size;
			char Buf[1024];
			s>>size;
			s.ignore(1);
			if (size > 0) {
				s.get(Buf,size+1);
				Name=Buf;
			} else {
				Name = "";
			}
        }

		#ifdef DEBUG_STREAM
		cerr<<dummy<<" "<<ID<<" "<<dummy2<<" "<<PluginID<<" "<<x<<" "<<y<<endl;
		#endif

		if (ver>1) s>>ps>>px>>py;

		// Check we're not duplicating an ID
		WLocker rlocker(&o.m_DeviceWinMap_lock);
		if (o.m_DeviceWinMap.find(ID)!=o.m_DeviceWinMap.end())
		{
			SpiralInfo::Alert("Duplicate device ID found in file - aborting load");
			return s;
		}

		if (PluginID==COMMENT_ID)
		{
			DeviceWin* temp = o.NewComment(PluginID, x, y);
			if (temp)
			{
				temp->m_DeviceGUI->SetID(ID);
				o.m_DeviceWinMap[ID]=temp;
				((Fl_CommentGUI*)(o.m_DeviceWinMap[ID]->m_DeviceGUI))->StreamIn(s); // load the plugin
				if (o.m_NextID<=ID) o.m_NextID=ID+1;
			}
		}
		else
		{
			DeviceWin* temp = o.NewDeviceWin(PluginID, x, y);
			if (temp)
			{
				temp->m_DeviceGUI->SetID(ID);
				if (ver>3)
				{
					// set the titlebars
					temp->m_DeviceGUI->SetName(Name);
				}

				temp->m_Device->SetUpdateInfoCallback(ID,o.cb_UpdatePluginInfo);
				o.m_DeviceWinMap[ID]=temp;
				o.m_DeviceWinMap[ID]->m_Device->StreamIn(s); // load the plugin
				// load external files
				o.m_DeviceWinMap[ID]->m_Device->LoadExternalFiles(o.m_FilePath+"_files/");

				if (ver>1 && o.m_DeviceWinMap[ID]->m_DeviceGUI->GetPluginWindow())
				{
					// set the GUI up with the loaded values
					// looks messy, but if we do it here, the plugin and it's gui can remain
					// totally seperated.
					((SpiralPluginGUI*)(o.m_DeviceWinMap[ID]->m_DeviceGUI->GetPluginWindow()))->
						UpdateValues(o.m_DeviceWinMap[ID]->m_Device);

					// updates the data in the channel buffers, so the values don't
					// get overwritten in the next tick. (should maybe be somewhere else)
					o.m_DeviceWinMap[ID]->m_Device->GetChannelHandler()->FlushChannels();

					// position the plugin window in the main window
					//o.m_DeviceWinMap[ID]->m_DeviceGUI->GetPluginWindow()->position(px,py);

					if (ps)
					{
						o.m_DeviceWinMap[ID]->m_DeviceGUI->Maximise();
						// reposition after maximise
						o.m_DeviceWinMap[ID]->m_DeviceGUI->position(x,y);
					}
					else o.m_DeviceWinMap[ID]->m_DeviceGUI->Minimise();
				}

				if (o.m_NextID<=ID) o.m_NextID=ID+1;
			}
			else
			{
				// can't really recover if the plugin ID doesn't match a plugin, as
			    // we have no idea how much data in the stream belongs to this plugin
				SpiralInfo::Alert("Error in stream, can't really recover data from here on.");
				return s;
			}
		}
	}

	s>>*o.m_Canvas;

	o.ResumeAudio();
        return s;
}

//////////////////////////////////////////////////////////

ostream &operator<<(ostream &s, SynthModular &o)
{
	o.PauseAudio();

	s<<"SpiralSynthModular File Ver "<<FILE_VERSION<<endl;

	// make external files dir
	bool ExternalDirUsed=false;
	string command("mkdir "+o.m_FilePath+"_files");
	system(command.c_str());

	if (FILE_VERSION>2)
	{
		s<<o.m_TopWindow->x()<<" "<<o.m_TopWindow->y()<<" ";
		s<<o.m_TopWindow->w()<<" "<<o.m_TopWindow->h()<<" ";
		s<<0<<" "<<0<<" ";
		s<<0<<" "<<0<<endl;
	}

	RLocker rlocker(&o.m_DeviceWinMap_lock);

	// save out the SynthModular
	s<<"SectionList"<<endl;
	s<<o.m_DeviceWinMap.size()<<endl;

	for(map<int,DeviceWin*>::iterator i=o.m_DeviceWinMap.begin();
		i!=o.m_DeviceWinMap.end(); i++)
	{
                // 0.2.2fix1 Andrew Johnson Critical BUG in 0.2.2 Sun, 14 Mar 2004
                // if (i->second->m_DeviceGUI && i->second->m_Device)
                if (i->second->m_DeviceGUI && ((i->second->m_Device) || (i->second->m_PluginID==COMMENT_ID)))
                {
			s<<endl;
			s<<"Device ";
			s<<i->first<<" "; // save the id
			s<<"Plugin ";
			s<<i->second->m_PluginID<<endl;
			s<<i->second->m_DeviceGUI->x()<<" ";
			s<<i->second->m_DeviceGUI->y()<<" ";
			s<<i->second->m_DeviceGUI->GetName().size()<<" ";
			s<<i->second->m_DeviceGUI->GetName()<<" ";

			if (i->second->m_DeviceGUI->GetPluginWindow())
			{
				s<<i->second->m_DeviceGUI->GetPluginWindow()->visible()<<" ";
				s<<i->second->m_DeviceGUI->GetPluginWindow()->x()<<" ";
				s<<i->second->m_DeviceGUI->GetPluginWindow()->y()<<" ";
			}
			else
			{
				s<<0<<" "<<0<<" "<<0;
			}

			s<<endl;

			if (i->second->m_PluginID==COMMENT_ID)
			{
				// save the comment gui
				((Fl_CommentGUI*)(i->second->m_DeviceGUI))->StreamOut(s);
			}
			else
			{
				// save the plugin
				i->second->m_Device->StreamOut(s);
			}
			s<<endl;

			// save external files
			if (i->second->m_Device && i->second->m_Device->SaveExternalFiles(o.m_FilePath+"_files/"))
			{
				ExternalDirUsed=true;
			}
		}
	}

	s<<endl<<*o.m_Canvas<<endl;

	// remove it if it wasn't used
	if (!ExternalDirUsed)
	{
		// i guess rmdir won't work if there is something in the dir
		// anyway, but best to be on the safe side. (could do rm -rf) :)
		string command("rmdir "+o.m_FilePath+"_files");
		system(command.c_str());
	}

	o.ResumeAudio();

	return s;
}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Close_i(Fl_Window* o, void* v)
{
	m_SettingsWindow->hide();
	delete m_SettingsWindow;
	m_TopWindow->hide();
	delete m_TopWindow;
	o->hide();
}

void SynthModular::cb_Close(Fl_Window* o, void* v)
{((SynthModular*)(o->user_data()))->cb_Close_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Load_i(Fl_Button* o, void* v)
{
	if (m_DeviceWinMap.size()>0 && !Pawfal_YesNo("Load - Lose changes to current patch?"))
	{
		return;
	}

	char *fn=fl_file_chooser("Load a patch", "*.ssm", NULL);

	if (fn && fn!='\0')
	{
		ifstream inf(fn);

		if (inf)
		{
			m_FilePath=fn;

			ClearUp();
			inf>>*this;

			TITLEBAR=LABEL+" "+fn;
			m_TopWindow->label(TITLEBAR.c_str());
		}
	}
}

void SynthModular::cb_Load(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_Load_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Save_i(Fl_Button* o, void* v)
{
	char *fn=fl_file_chooser("Save a patch", "*.ssm", NULL);

	if (fn && fn!='\0')
	{
		ifstream ifl(fn);
		if (ifl)
		{
			if (!Pawfal_YesNo("File [%s] exists, overwrite?",fn))
			{
				return;
			}
		}

		ofstream of(fn);

		if (of)
		{
			m_FilePath=fn;

			of<<*this;

			TITLEBAR=LABEL+" "+fn;
			m_TopWindow->label(TITLEBAR.c_str());
		}
		else
		{
			fl_message(string("Error saving "+string(fn)).c_str());
		}
	}
}
void SynthModular::cb_Save(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_Save_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_New_i(Fl_Button* o, void* v)
{
	if (m_DeviceWinMap.size()>0 && !Pawfal_YesNo("New - Lose changes to current patch?"))
	{
		return;
	}

	m_TopWindow->label(TITLEBAR.c_str());

	ClearUp();
}
void SynthModular::cb_New(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_New_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_NewDevice_i(Fl_Button* o, void* v)
{
        AddDevice(*((int*)v));
}
void SynthModular::cb_NewDevice(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_NewDevice_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_NewDeviceFromMenu_i(Fl_Canvas* o, void* v)
{
	AddDevice(*((int*)v),*((int*)v+1),*((int*)v+2));
}
void SynthModular::cb_NewDeviceFromMenu(Fl_Canvas* o, void* v)
{((SynthModular*)(o->user_data()))->cb_NewDeviceFromMenu_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_NewComment_i(Fl_Button* o, void* v)
{
	AddComment(-1);
}
void SynthModular::cb_NewComment(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_NewComment_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_GroupTab_i(Fl_Tabs* o, void* v)
{
        m_GroupTab->redraw();
}

void SynthModular::cb_GroupTab(Fl_Tabs* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_GroupTab_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Rload_i(Fl_Button* o, void* v)
{
	m_SettingsWindow->show();

	/*PluginManager::Get()->UnloadAll();

	m_ToolBox->remove(m_ToolPack);
	delete m_ToolPack;

	m_ToolPack = new Fl_Pack(5,20,TOOLBOX_WIDTH-10, TOOLBOX_HEIGHT-40,"");
    m_ToolPack->type(FL_VERTICAL);
	m_ToolPack->box(FL_NO_BOX);
	m_ToolPack->color(SpiralInfo::GUICOL_Tool);
    m_ToolPack->user_data((void*)(this));
	m_ToolBox->add(m_ToolPack);
	m_ToolBox->redraw();

	LoadPlugins();*/
}
void SynthModular::cb_Rload(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_Rload_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Connection_i(Fl_Canvas* o, void* v)
{
	CanvasWire *Wire;
	Wire=(CanvasWire*)v;

	RLocker rlocker(&m_DeviceWinMap_lock);
	map<int,DeviceWin*>::iterator si=m_DeviceWinMap.find(Wire->OutputID);
	if (si==m_DeviceWinMap.end())
	{
		char num[32];
		sprintf(num,"%d",Wire->OutputID);
		SpiralInfo::Alert("Warning: Connection problem - can't find source "+string(num));
		return;
	}
	
	map<int,DeviceWin*>::iterator di=m_DeviceWinMap.find(Wire->InputID);
	if (di==m_DeviceWinMap.end())
	{
		char num[32]; 
		sprintf(num,"%d",Wire->InputID);
		SpiralInfo::Alert("Warning: Connection problem - can't find destination "+string(num));
		return;
	}

	Sample *sample=NULL;
	
	if (!si->second->m_Device->GetOutput(Wire->OutputPort,&sample))
	{ 
		char num[32]; 
		sprintf(num,"%d,%d",Wire->OutputID,Wire->OutputPort);
		SpiralInfo::Alert("Warning: Connection problem - can't find source output "+string(num));
		return; 
	}
	
	if (!di->second->m_Device->SetInput(Wire->InputPort,(const Sample*)sample))
	{ 
		char num[32]; 
		sprintf(num,"%d,%d",Wire->InputID,Wire->InputPort);
		SpiralInfo::Alert("Warning: Connection problem - can't find source input "+string(num));
		return;	
	}
}
void SynthModular::cb_Connection(Fl_Canvas* o, void* v)
{((SynthModular*)(o->user_data()))->cb_Connection_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Unconnect_i(Fl_Canvas* o, void* v)
{
	CanvasWire *Wire;
	Wire=(CanvasWire*)v;
	
	//cerr<<Wire->InputID<<" "<<Wire->InputPort<<endl;
	
	RLocker rlocker(&m_DeviceWinMap_lock);
	map<int,DeviceWin*>::iterator di=m_DeviceWinMap.find(Wire->InputID);
	if (di==m_DeviceWinMap.end())
	{
		//cerr<<"Can't find destination device "<<Wire->InputID<<endl;
		return;
	}

	SpiralPlugin *Plugin=di->second->m_Device;
	if (Plugin && !Plugin->SetInput(Wire->InputPort,NULL))
	{ cerr<<"Can't find destination device's Input"<<endl; return;	}
}
void SynthModular::cb_Unconnect(Fl_Canvas* o, void* v)
{((SynthModular*)(o->user_data()))->cb_Unconnect_i(o,v);}

//////////////////////////////////////////////////////////

void SynthModular::cb_UpdatePluginInfo(int ID, void *PInfo)
{
	map<int,DeviceWin*>::iterator i=m_DeviceWinMap.find(ID);
	if (i!=m_DeviceWinMap.end())
	{
		DeviceGUIInfo Info=BuildDeviceGUIInfo(*((PluginInfo*)PInfo));

		(*i).second->m_DeviceGUI->Setup(Info);
		(*i).second->m_DeviceGUI->redraw();
	}
}

//////////////////////////////////////////////////////////

void SynthModular::LoadPatch(const char *fn)
{
    ifstream inf(fn);

    if (inf) 
	{	
	    m_FilePath=fn;	

		ClearUp();
	    inf>>*this;

	    TITLEBAR=LABEL+" "+fn;
	    m_TopWindow->label(TITLEBAR.c_str());
    }
}

//////////////////////////////////////////////////////////
