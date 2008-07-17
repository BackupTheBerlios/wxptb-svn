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
#include <wx/file.h>
#include <wx/msgout.h>


#include "randomc/randomc.h"

#include "wxPTB.h"
#include "PTBTaker.h"

#define PTB_ID_TIMER_CHECKFOR_EXIT          1 + wxID_HIGHEST

IMPLEMENT_APP(PTBApp);


PTBApp::PTBApp ()
      : bDoExit_(false),
        log_(PTB_LOG_FILE, wxFile::write_append)
{
    CareLogSize();

    Log(wxString::Format("\n%s started...", GetFullApplicationName()));
}


/*virtual*/ PTBApp::~PTBApp ()
{
}

void PTBApp::SetDoExit (bool bDoExit /*= true*/)
{
    bDoExit_ = bDoExit;
}

void PTBApp::Log (const wxString& strLogmessage, bool bShowLogMessage /*= false*/)
{
    log_.Write(strLogmessage + '\n');

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
    if (log_.Length() > PTB_LOG_MAXSIZE)
    {
        // close the file
        log_.Close();
        // backup the file
        wxRenameFile(PTB_LOG_FILE, wxString(PTB_LOG_FILE) + ".old", true);
        // create a new empty file
        log_.Open(PTB_LOG_FILE, wxFile::write_append);
    }
}

int PTBApp::DownloadFile(const wxString& strSource, const wxString& strTarget)
{
    #define DLBUFSIZE 4096

    wxURL url(strSource);

    if(url.GetError() == wxURL_NOERR)
    {
        wxInputStream *in = url.GetInputStream();
        wxFile fileOut(strTarget, wxFile::write);

        if(!in)
            return -1;

        unsigned char   buffer[DLBUFSIZE];
        size_t          tRead;

        do
        {
            in->Read(buffer, DLBUFSIZE);
            tRead = in->LastRead();

            if ( tRead > 0 )
                fileOut.Write(buffer, tRead);

        } while( !in->Eof() );

        delete in;
        fileOut.Close();
    }

    return url.GetError();
}

void PTBApp::Usage ()
{
    wxMessageOutput* out = wxMessageOutput::Get();

    out->Printf("");
    out->Printf("=== %s === (GPLv3 licencesed)", GetFullApplicationName());
    out->Printf("USAGE: %s [hashname] [-o|--output filename] [-h|--help]", PTB_APP_NAME);
    out->Printf("");
    out->Printf("  [hashname]     Specify the hash-name to store as a signature file.");
    out->Printf("                 If there is no hash specified a randome one is used.");
    out->Printf("                 Use \"all\" to store all hashes in signature files.");
    out->Printf("                 The option \"-o\" is ignored in that case!");
    out->Printf("");
    out->Printf("  -o|--output    Use to specify a outputfilename. By default it is %s.", PTB_OUT_DEFAULT);
    out->Printf("");
    out->Printf("  -h             Display this text.");
    out->Printf("");
}

void PTBApp::ParseCmdLine ()
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
        else
        {
            strHash_ = argv[i];
        }
    }
}


/*virtual*/ bool PTBApp::OnInit()
{
    wxSocketBase::Initialize();

    ParseCmdLine();

    // start the thread
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

    // start timer
    pTimer_->Start(1000, wxTIMER_CONTINUOUS);

    return true;
}


/*virtual*/ int PTBApp::OnExit()
{
    return 0;
}


void PTBApp::OnTimer_CheckForExit (wxTimerEvent& rEvent)
{
    if (bDoExit_)
        Exit();
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
    TRandomMersenne     rg(time(0));        // random number generator
    return rg.IRandom(iFrom, iTo);
}

wxString PTBApp::GetHash ()
{
    return strHash_;
}


wxString PTBApp::GetOut ()
{
    return strOut_;
}
