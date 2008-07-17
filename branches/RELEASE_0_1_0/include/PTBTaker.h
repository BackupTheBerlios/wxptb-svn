/**
 * Name:        PTBTaker.h
 * Purpose:
 * Author:      Christian Buhtz
 * Modified by:
 * Created:     2008-07-11
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


#ifndef PTBTAKER_H
#define PTBTAKER_H

#include <wx/thread.h>
#include <wx/arrstr.h>
#include <wx/xml/xml.h>

//
class PTBApp;

///
class PTBTaker : public wxThread
{
    private:
        ///
        wxXmlDocument   docXml_;
        ///
        wxArrayString   arrHashes_;

        ///
        PTBApp*         pCaller_;


        /** Search for a node named 'strNodeName' and return its content. 'pNodeRoot' is
            used to begin the search. */
        wxString GetNodeContent (wxXmlNode* pNodeRoot, const wxString& strNodeName);
        ///
        wxString GetNodeContent (const wxString& strNodeName);

        /** Create the signatur for 'strHash' and write it to the file 'strOutputFilename'.
            If there is no filename specified the hash-name is used as filename. */
        void WriteHashSig (const wxString& strHash, const wxString& strOutputFilename = wxEmptyString);

    public:
        /// ctor
        PTBTaker (PTBApp* pCaller);

        /// virtual dtor
        virtual ~PTBTaker ();

        /// thread execution starts here
        virtual void *Entry();
};

#endif
