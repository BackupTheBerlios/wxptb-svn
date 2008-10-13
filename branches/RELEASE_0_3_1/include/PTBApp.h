/**
 * Name:        PTBApp.h
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


#ifndef PTBAPP_H
#define PTBAPP_H

// forward declarations
#include "PTBConfig.h"

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/timer.h>


///
class PTBApp : public wxAppConsole
{
    private:
        ///
        static PTBApp*      pInstance_;
        ///
        wxTimer*            pTimer_;

        ///
        static wxString     strApplicationDir_;

        ///
        wxFile              log_;
        ///
        PTBConfig           config_;

        /** The hash to store in a signature file.
            If it is not specified or the hash doesn't exists a random hash is used (default).
            If it is "all" all hashes will saved in separate signature files. */
        wxString    strHash_;

        /** The output signature file.
            If it is not explicite specified the output signature file is named
            a) "ptb.sig" for a random hash (default)
            or
            b) the hash name with ".sig" as prefix if all hashes are used. */
        wxString    strOut_;

        ///
        void Usage ();
        ///
        void About ();
        ///
        void AboutApplicationFiles ();
        ///
        void ParseCmdLine ();

        ///
        void RememberApplicationDirectory ();


        ///
        void InitLog ();
        ///
        void CareLogSize ();

    public:
        /// ctor
        PTBApp ();
        /// virtual dtor
        virtual ~PTBApp ();

        ///
        static wxString GetFullApplicationName ();
        /// return major, minor, release number and extensions as string
        static wxString GetVersion ();

        ///
        static const wxString& GetApplicationDirectory ();
        ///
        static const wxString GetLogFileName ();
        ///
        static const wxString GetConfigFileName ();
        ///
        static const wxString GetDefaultPTBFileName ();

        ///
        static void Log (const wxString& strLogmessage, bool bShowLogMessage = false);

        ///
        wxString GetHash ();
        ///
        wxString GetOut ();
        ///
        const PTBConfig& Config ()
        {
            return config_;
        }

        /// start point like main()
        virtual bool OnInit();
        ///
        virtual int OnExit();

        ///
        void OnTimer_CheckForExit (wxTimerEvent& rEvent);

        ///
        int GetRandomNumber (int iFrom, int iTo);
};

DECLARE_APP(PTBApp)

#endif
