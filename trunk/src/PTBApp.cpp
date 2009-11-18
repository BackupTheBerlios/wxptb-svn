/**
 * Name:        PTBApp.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-07-09
 * Copyright:   (c) 2008 Christian Buhtz <exsudat@gmx.de>
 * Licence:     GNU General Public License (Version 3)
 ***
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include "PTBApp.h"

#include <wx/timer.h>
#include <wx/url.h>
#include <wx/filename.h>
#include <wx/msgout.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>

#include "wxPTB.h"
#include "PTBTaker.h"

#define PTB_ID_TIMER_CHECKFOR_EXIT          1 + wxID_HIGHEST

IMPLEMENT_APP_CONSOLE(PTBApp);

/*static*/ PTBApp*      PTBApp::pInstance_          = NULL;
/*static*/ wxString     PTBApp::strApplicationDir_  = ".";

PTBApp::PTBApp ()
{
    pInstance_ = this;
}


/*virtual*/ PTBApp::~PTBApp ()
{
}

/*virtual*/ bool PTBApp::OnInit()
{
    // remember the application/binary directory
    // XXX RememberApplicationDirectory ();

#if defined(__UNIX__)
    // check for the "$HOME/.wxPTB"
    wxString strHome = wxFileName::GetHomeDir();
    if ( !(wxDirExists(strHome + wxFILE_SEP_PATH + PTB_CONFIG_DIR)) )
    {
        wxSetWorkingDirectory(strHome);
        wxFileName::Mkdir(PTB_CONFIG_DIR, 0700);
        wxSetWorkingDirectory(GetApplicationDirectory());
    }
#endif

    //
    InitLog ();

    //
    Log(wxString::Format("%s started...", GetFullApplicationName()));

    // init random numbers
    srand((int)(time(NULL))*(int)(this));

    wxSocketBase::Initialize();

    if ( !ParseCmdLine() )
		Exit();

    // get conf
    config_.Init();

    // check the log-file size
    CareLogSize();

    // start the detached thread
    new PTBTaker(this);

    // create timer
    pTimer_ = new wxTimer(this, PTB_ID_TIMER_CHECKFOR_EXIT);

    // connect timer
    Connect
    (
        PTB_ID_TIMER_CHECKFOR_EXIT,
        wxEVT_TIMER,
        wxTimerEventHandler(PTBApp::OnTimer_CheckForExit),
        NULL,
        this
    );

    // start timer that check if the thread is alive
    pTimer_->Start(1000, wxTIMER_CONTINUOUS);

    return true;
}


/*virtual*/ int PTBApp::OnExit()
{
    return 0;
}


void PTBApp::OnTimer_CheckForExit (wxTimerEvent& rEvent)
{
    if ( PTBTaker::Instance() == NULL )
    {
        delete pTimer_;
        Exit();
    }
}

void PTBApp::InitLog ()
{
    log_.Open(GetLogFileName(), wxFile::write_append);
	log_.Write("\n\n");
}

/*static*/ const wxString PTBApp::GetLogFileName ()
{
    #if defined(__WXMSW__) || defined(__UNIX__)
		wxString str = wxStandardPaths::Get().GetUserConfigDir();

		if ( false == str.EndsWith(wxFILE_SEP_PATH) )
			str << wxFILE_SEP_PATH;

		str << PTB_APP_NAME;

		if ( false == wxDirExists(str) )
			wxMkDir(str);

		str << wxFILE_SEP_PATH
			<< "wxPTB.log";
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif

    return str;
}

/*static*/ const wxString PTBApp::GetConfigFileName ()
{
    #if defined(__WXMSW__) || defined(__UNIX__)
        wxString str = wxStandardPaths::Get().GetUserConfigDir();
		
		if ( false == str.EndsWith(wxFILE_SEP_PATH) )
			str << wxFILE_SEP_PATH;

		str << PTB_APP_NAME;

		if ( false == wxDirExists(str) )
			wxMkDir(str);

		str << wxFILE_SEP_PATH
			<< "wxPTB.conf";
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif

    return str;
}

/*static*/ const wxString PTBApp::GetLicenseFileName ()
{
    #if defined(__WXMSW__) || defined(__UNIX__)
		wxString str = wxStandardPaths::Get().GetExecutablePath().BeforeLast(wxFILE_SEP_PATH);

		str << wxFILE_SEP_PATH << "LICENSE";
    #else
        #error "Unsupported plattform! Please contact the project maintainer for support!"
    #endif

    return str;
}

/*static*/ const wxString PTBApp::GetDefaultPTBFileName ()
{
    wxString str = wxStandardPaths::Get().GetAppDocumentsDir();

	if ( false == str.EndsWith(wxFILE_SEP_PATH) )
		str << wxFILE_SEP_PATH;
	
	str << "wxPTB.sig";

	return str;
}


/*static*/ wxString PTBApp::GetFullApplicationName ()
{
    return wxString::Format(_T("%s %d.%d.%d %s"),
                               PTB_APP_NAME,
                               PTB_VERSION_MAJOR,
                               PTB_VERSION_MINOR,
                               PTB_VERSION_RELEASE,
                               PTB_VERSION_EXTENSION);
}

/*static*/ wxString PTBApp::GetVersion ()
{
    return wxString::Format("%d.%d.%d %s",
                            PTB_VERSION_MAJOR,
                            PTB_VERSION_MINOR,
                            PTB_VERSION_RELEASE,
                            PTB_VERSION_EXTENSION);
}

int PTBApp::GetRandomNumber (int iFrom, int iTo)
{
    return rand() % (iTo - iFrom + 1) + iFrom;
}

wxString PTBApp::GetHash ()
{
    return strHash_;
}


wxString PTBApp::GetOut ()
{
    return strOut_;
}

/*static*/ void PTBApp::Log (const wxString& strLogmessage, bool bShowLogMessage /*= false*/)
{
	// create message with timestamp
    wxString str;
	str << wxDateTime::Now().Format("%Y-%m-%d %H:%M:%S : ")
		<< strLogmessage
		<< '\n';
	
    pInstance_->log_.Write(str);


    if (bShowLogMessage)
    {
        wxMessageOutput* pOut = wxMessageOutput::Get();

        if (pOut)
            pOut->Printf(strLogmessage);
    }
}

void PTBApp::CareLogSize ()
{
    // check log-file size
    if (log_.Length() > (config_.GetMaxLogFileSizeInKB() * 1024))
    {
        // close the file
        log_.Close();
        // backup the file
        wxRenameFile(GetLogFileName(), wxString(GetLogFileName()) + ".old", true);
        // create a new empty file
        log_.Open(GetLogFileName(), wxFile::write_append);
    }
}

bool PTBApp::ParseCmdLine ()
{
    //wxApp::argc;
    //wxApp::argv;

    wxString str;

    for (int i = 1; i < argc; ++i)
    {
        str = argv[i];

        if ( str.StartsWith("-h")
          || str.StartsWith("/h")
          || str.StartsWith("--help")
          || str.StartsWith("-?")
          || str.StartsWith("/?") )
        {
            Usage();
			return false;
        }
        else if ( str.StartsWith("-i")
          || str.StartsWith("/i")
          || str.StartsWith("--info") )
        {
            About();
			return false;
        }
        else if ( str.StartsWith("-l")
          || str.StartsWith("/l")
          || str.StartsWith("--license") )
		{
			License();
			return false;
		}
        else if ( str.StartsWith("-o")
               || str.StartsWith("/o")
               || str.StartsWith("--output") )
        {
            if (argc > i)
            {
                ++i;
                strOut_ = argv[i];
            }
        }
        else if ( str.StartsWith("-") )
        {
            Usage();
			return false;
        }
        else
        {
            strHash_ = argv[i];
        }
    }

	return true;
}

/*void PTBApp::RememberApplicationDirectory ()
{
    strApplicationDir_ = argv[0].BeforeLast(wxFILE_SEP_PATH);

    if (strApplicationDir_.IsEmpty())
        strApplicationDir_ = wxGetCwd();
}*/

/*static* const wxString& PTBApp::GetApplicationDirectory ()
{
    return strApplicationDir_;
}*/

void PTBApp::License ()
{
	wxMessageOutput* out = wxMessageOutput::Get();

	out->Printf("");
	out->Printf("=================== LICENSE ===================");

	wxTextFile file;

	file.Open( GetLicenseFileName() );

	if ( file.IsOpened() )
	{
		out->Printf(file.GetFirstLine());

		while ( !file.Eof() )
			out->Printf(file.GetNextLine());
	}
	else
	{
		out->Printf("(error while loading the license file (%s)", GetLicenseFileName() );
	}
}

void PTBApp::Usage ()
{
    wxMessageOutput* out = wxMessageOutput::Get();

    out->Printf("");
    out->Printf("=== %s === (GPLv3 licensed)", GetFullApplicationName());
	out->Printf("USAGE: %s [hashname] [-o|--output filename] [-h|--help] [-i|--info] [-l|--license]", PTB_APP_NAME);
    out->Printf("");
    out->Printf("  [hashname]     Specify the hash-name to store as a signature file.");
    out->Printf("                 If there is no hash specified a random one is used.");
    out->Printf("                 Use \"all\" to store all hashes in signature files.");
    out->Printf("                 The option \"-o\" is ignored in that case!");
    out->Printf("");
    out->Printf("  -o|--output    Use to specify a outputfilename. By default it is %s.", PTBApp::GetDefaultPTBFileName().AfterLast(wxFILE_SEP_PATH));
    out->Printf("");
    out->Printf("  -h|--help      Display this text.");
    out->Printf("");
    out->Printf("  -i|--info      Display infos about the application.");
    out->Printf("");
    out->Printf("  -l|--license   Display the license information file.");
    out->Printf("");
    AboutApplicationFiles();
}

void PTBApp::About ()
{
    wxMessageOutput* out = wxMessageOutput::Get();

    out->Printf("%s Copyright (C) 2008 Christian Buhtz <blackfisk@web.de>", GetFullApplicationName());
	out->Printf("  Website: <http://wxptb.berlios.de>");
    out->Printf("  This program comes with ABSOLUTELY NO WARRANTY;");
    out->Printf("  This is free software, and you are welcome to redistribute it");
    out->Printf("  under certain conditions;");
    out->Printf("  for details see the file LICENSE or use the option '-l';");
    out->Printf("");
    out->Printf("Developers:");
    out->Printf("  Christian Buhtz");
    out->Printf("");
    out->Printf("3rd-Party-Components:");
    out->Printf("  wxWidgets (v%d.%d.%d) -> Cross-Platform GUI Library on <www.wxwidgets.org>", wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER);
    out->Printf("");
    out->Printf("used Tools:");
    out->Printf("  Code::Blocks -> open source, cross platform C++ IDE on <www.codeblocks.org>.");
    out->Printf("  MinGW -> GNU based compiler system for Windows on <www.mingw.org>.");
	out->Printf("  MS Visual Studio 2008 Express Edition -> compiler system and IDE");
    out->Printf("  Inkscape -> open source vector graphics editor on <www.inkscape.org>.");
    out->Printf("  IcoFX -> icon editor on <icofx.ro>.");
	out->Printf("  Inno Setup -> Create a install routine for windows. <http://www.jrsoftware.org/isinfo.php>");
    out->Printf("");
    out->Printf("Supporters:");
    out->Printf("  Jan Kechel -> Maintainer of <www.publictimestamp.org>.");
    out->Printf("  BerliOS -> free service to Open Source developers on <berlios.de>.");
    out->Printf("  Gmane -> mail-to-news-Gateway and mailing list archive on <gmane.org>.");
	out->Printf("  QSC -> DSL provider that take a good care of my connection to the world.");

    out->Printf("");
    AboutApplicationFiles();
}

void PTBApp::AboutApplicationFiles ()
{
    wxMessageOutput* out = wxMessageOutput::Get();

    out->Printf("Files and directories used by %s :", GetFullApplicationName());
    out->Printf("  %s\n\t-> Configuration file.", GetConfigFileName());
    out->Printf("  %s\n\t-> Log file.", GetLogFileName());
    out->Printf("  %s\n\t-> Default ptb-signature file.", GetDefaultPTBFileName());
	out->Printf("  %s\n\t-> Executable path.", wxStandardPaths::Get().GetExecutablePath());
}
