/**
 * Name:        PTBTaker.cpp
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


#include "PTBTaker.h"
#include "PTBApp.h"
#include "wxPTB.h"

#include <stdlib.h>
#include <wx/url.h>
#include <wx/file.h>


PTBTaker::PTBTaker (PTBApp* pCaller)
        : pCaller_(pCaller),
          lRetrys_(0)
{
    arrHashes_.Add("sha512");
    arrHashes_.Add("whirlpool");
    arrHashes_.Add("ripemd128");
    arrHashes_.Add("ripemd160");
    arrHashes_.Add("haval");
    arrHashes_.Add("tiger");
    arrHashes_.Add("sha384");
    arrHashes_.Add("sha256");
    arrHashes_.Add("sha160");
    arrHashes_.Add("sha224");
    arrHashes_.Add("tiger128");
    arrHashes_.Add("tiger160");
    arrHashes_.Add("tiger2");
    arrHashes_.Add("whirlpool0");
    arrHashes_.Add("whirlpool2");
    arrHashes_.Add("ripemd256");
    arrHashes_.Add("ripemd320");

    lRetrys_ = pCaller_->Config().GetLoadRetry();

    Create();
    wxThread::Run();
}


/*virtual*/ PTBTaker::~PTBTaker ()
{
}


wxString PTBTaker::GetNodeContent (const wxString& strNodeName)
{
    return GetNodeContent(docXml_.GetRoot()->GetChildren(), strNodeName);
}

wxString PTBTaker::GetNodeContent (wxXmlNode* pNodeRoot, const wxString& strNodeName)
{
    wxString strReturn;

    while ( pNodeRoot )
    {
        if ( pNodeRoot->GetName() == strNodeName )
            return pNodeRoot->GetNodeContent();

        wxXmlNode* pChilds = pNodeRoot->GetChildren();

        if (pChilds)
        {
            strReturn = GetNodeContent(pChilds, strNodeName);
            if ( !(strReturn.IsEmpty()) )
                return strReturn;
        }

        pNodeRoot = pNodeRoot->GetNext();
    }

    return strReturn;
}


void PTBTaker::WriteHashSig (const wxString& strHash,
                             const wxString& strOutputFilename /*= wxEmptyString*/)
{
    // build the output string
    wxString strOut =  wxString::Format ("publictimestamp.org/ptb/PTB-%s %s %s\n",
                                         GetNodeContent("id"),
                                         strHash,
                                         GetNodeContent("tstamp"));

    wxString strH = GetNodeContent(strHash);
    strH = strH.Mid(0, 70) + '\n' + strH.Mid(70);

    strOut = strOut + strH;

    // create output filename
    wxString strFilename = strOutputFilename;
    if (strFilename.IsEmpty())
        strFilename = strHash;

    if ( !(strFilename.EndsWith(PTB_OUT_SUFFIX)) )
        strFilename = strFilename + PTB_OUT_SUFFIX;

    // create the file and write the string out
    wxFile file(strFilename, wxFile::write);
    file.Write(strOut);

    // status message
    pCaller_->Log(wxString::Format("Store hash \"%s\" in \"%s\".\n--\n%s", strHash, strFilename, strOut));
}

bool PTBTaker::Work ()
{
    // the url
    wxURL url(PTB_URL_TIMESTAMP);

    // check url
    if ( !(url.IsOk()) )
    {
        if (lRetrys_ < 1)
            pCaller_->Log(PTB_ERR_URL, true);
        else
            pCaller_->Log(PTB_ERR_URL, false);

        return false;
    }

    // the input stream
    wxInputStream* pInStream = url.GetInputStream();

    // check stream
    if ( !pInStream || !(pInStream->IsOk()) )
    {
        if (lRetrys_ < 1)
            pCaller_->Log(PTB_ERR_STREAM, true);
        else
            pCaller_->Log(PTB_ERR_STREAM, false);

        return false;
    }


    // load the xml file
    docXml_.Load(*pInStream);

    // check xml document
    if ( !(docXml_.IsOk()) )
    {
        if (lRetrys_ < 1)
            pCaller_->Log(PTB_ERR_XMLDOC, true);
        else
            pCaller_->Log(PTB_ERR_XMLDOC, false);

        return false;
    }

    // select the hash
    wxString strHash = pCaller_->GetHash();

    // the output filename
    wxString strOuputFilename = pCaller_->GetOut();

    if ( strOuputFilename.IsEmpty() )
        strOuputFilename = PTB_OUT_DEFAULT;


    if (strHash.IsEmpty())
    {
        // use a random hash
        WriteHashSig(arrHashes_[pCaller_->GetRandomNumber(0, arrHashes_.GetCount()-1)], strOuputFilename);
    }
    else
    {
        if (strHash == PTB_HASH_ALL)
        {
            // use all hashes and write them separatly to files named by the hashes
            for (size_t i = 0; i < arrHashes_.GetCount(); ++i)
                WriteHashSig(arrHashes_[i]);
        }
        else
        {
            if (arrHashes_.Index(strHash) == wxNOT_FOUND)
                strHash = arrHashes_[pCaller_->GetRandomNumber(0, arrHashes_.GetCount()-1)];

            // use the specifiied
            WriteHashSig(strHash, PTB_OUT_DEFAULT);
        }
    }

    return true;
}

/*virtual*/ void* PTBTaker::Entry()
{
    if ( Work() == false )
    {
        while ( lRetrys_ > 0 )
        {
            // decrement the retrys
            --lRetrys_;

            // delay
            wxSleep(pCaller_->Config().GetLoadDelayInSeconds());

            //
            PTBApp::Log("Retry...");

            // try again
            if (Work())
                break;
        }
    }

    // exit the thread and the application
    pCaller_->SetDoExit();
    Exit(0);
    return NULL;
}

