/**
 * Name:        PTBConfig.cpp
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-07-19
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


#include "PTBConfig.h"

#include <wx/fileconf.h>
#include <wx/wfstream.h>

#include "PTBApp.h"
#include "wxPTB.h"

PTBConfig::PTBConfig ()
         : lMaxSize_(1024),
           lLoadRetry_(3),
           lLoadDelay_(60)
{
}

void PTBConfig::Init ()
{
    // check if config file exists
    if ( !(wxFile::Exists(PTBApp::GetConfigFileName())) )
    {
        PTBApp::Log
        (
            wxString::Format("Log-file %s doesn't exists! Create a default one...",
                             PTBApp::GetConfigFileName())
        );

        wxFile file(PTBApp::GetConfigFileName(), wxFile::write);

        file.Write(wxString::Format
        (
            "# Configuration file for %s\n\n" \
            "# Maximum size in kilobytes of the log-file \"%s\"\n" \
            "# stored in \"%s\".\n" \
            "# If the size is reached the log-file is renamed with \".old\"\n" \
            "# as suffix and a new and empty one is created.\n" \
            "log-size=%d\n\n" \
            "# How many times should the download of the PTB be retried if it failes.\n" \
            "load-retry=%d\n\n" \
            "# Time to wait in seconds till the next retry of PTB download\n" \
            "# if it was failed.\n" \
            "load-retry-delay=%d\n",
            PTBApp::GetFullApplicationName(),
            PTBApp::GetLogFileName().AfterLast(wxFILE_SEP_PATH),
            PTBApp::GetLogFileName().BeforeLast(wxFILE_SEP_PATH),
            GetMaxLogFileSizeInKB(),
            GetLoadRetry(),
            GetLoadDelayInSeconds()
        ));
    }

    //
    ReadConfig();
}

/*virtual*/ PTBConfig::~PTBConfig ()
{
}


void PTBConfig::ReadConfig ()
{
    wxFileInputStream   is(PTBApp::GetConfigFileName());
    wxFileConfig        fileConfig(is);
    long                lVal;

    // maximum log-file size
    if ( fileConfig.Read("log-size", &lVal) )
        lMaxSize_ = lVal;

    // load retry
    if ( fileConfig.Read("load-retry", &lVal) )
        lLoadRetry_ = lVal;

    // load delay
    if ( fileConfig.Read("load-retry-delay", &lVal) )
        lLoadDelay_ = lVal;
}

bool PTBConfig::SaveConfig ()
{
    wxFileInputStream   is(PTBApp::GetConfigFileName());
    wxFileConfig        fileConfig(is);

    // maximum log-file size
    fileConfig.Write("log-size", lMaxSize_);

    //
    fileConfig.Write("load-retry", lLoadRetry_);

    //
    fileConfig.Write("load-retry-delay", lLoadDelay_);

    // save
    wxFileOutputStream os(PTBApp::GetConfigFileName());

    if ( !(fileConfig.Save(os)) )
    {
        PTBApp::Log
        (
            wxString::Format("Error while saving the config file %s.",
                             PTBApp::GetConfigFileName()),
            true
        );

        return false;
    }

    return true;
}

void PTBConfig::SetMaxLogFileSizeInKB (long lSize)
{
    if (lSize < 0)
        return;

    lMaxSize_ = lSize;
}

long PTBConfig::GetMaxLogFileSizeInKB () const
{
    return lMaxSize_;
}

void PTBConfig::SetLoadRetry (long lRetry)
{
    if (lRetry < 0)
        return;

    lLoadRetry_ = lRetry;
}

long PTBConfig::GetLoadRetry () const
{
    return lLoadRetry_;
}

void PTBConfig::SetLoadDelayInSeconds (long lDelay)
{
    if (lDelay < 0)
        return;

    lLoadDelay_ = lDelay;
}

long PTBConfig::GetLoadDelayInSeconds () const
{
    return lLoadDelay_;
}
