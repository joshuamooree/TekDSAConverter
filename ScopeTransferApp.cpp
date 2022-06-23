/***************************************************************
 * Name:      testingApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Joshua Moore (joshua.moore7@gmail.com)
 * Created:   2007-10-18
 * Copyright: Joshua Moore ()
 * License:
 **************************************************************/

#include "wx_pch.h"

#include "ScopeTransferApp.h"
//#include <wx/image.h>
#include "scopeTransferMain.h"
#include "Resources.zip.h"

//(*AppHeaders
#include <wx/image.h>
//*)

#include <stdio.h>

IMPLEMENT_APP(MyApp);

wxLogChain* logChain;

bool MyApp::OnInit()
{
	wxInitAllImageHandlers();

    //setup file system stuff
    wxFileSystem::AddHandler( new wxArchiveFSHandler );
    wxFileSystem::AddHandler( new wxMemoryFSHandler );
    //add the help zip file to the virtual file system
    wxMemoryFSHandler::AddFileWithMimeType( _T("Resources.zip"), Resources_zip, sizeof(Resources_zip), _T("application/zip") );

    wxInitAllImageHandlers();
    #if defined (__UNIX__)
        config = new wxFileConfig(  _T("Scope Transfer"),
                                    _T("Joshua Moore"),
                                    wxEmptyString,
                                    wxEmptyString,
                                    wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_RELATIVE_PATH );
    #else
        config = new wxFileConfig(  _T("Scope Transfer"),
                                    _T("Joshua Moore"),
                                    _T("ScopeTransfer.config"),
                                    wxEmptyString,
                                    wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_RELATIVE_PATH );
    #endif

    wxConfigBase::Set( config );

	ScopeTransferMain* frame = new ScopeTransferMain(0L, _("Scope Transfer"), wxDefaultPosition, wxSize( 500, 500 ) );



	frame->Show();
	return true;
}

int MyApp::OnExit()
{
    wxConfigBase::Get()->Flush();

    return wxApp::OnExit();
}
