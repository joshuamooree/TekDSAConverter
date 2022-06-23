/***************************************************************
 * Name:      ScopeTransferMain.h
 * Purpose:   Defines Application Frame
 * Author:    Joshua Moore (joshua.moore7@gmail.com)
 * Created:   2007-10-18
 * Copyright: Joshua Moore ()
 * License:
 **************************************************************/

#ifndef SCOPETRANSFERMAIN_H
#define SCOPETRANSFERMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include "ScopeTransferApp.h"

#include "ScopeTransferApp.h"
#include "DSA_11k.h"
#include "configDialog.h"
#include "SerialInputStream.h"
#include "MDIPIctureFrame.h"
#include "miscellaneousUtilities.h"

class ScopeTransferMain: public wxMDIParentFrame
{

	public:
		ScopeTransferMain(   wxFrame *frame,
                    const wxString& title,
                    const wxPoint& pos,
                    const wxSize& size );
		~ScopeTransferMain();
	protected:
		void OnQuit(wxCommandEvent& event);
		void OnClose( wxCloseEvent& event );

		void OnAbout(wxCommandEvent& event);
		void OnOpenHelp( wxCommandEvent& event );
		void OnConfig( wxCommandEvent& event );
		void OnRawFile( wxCommandEvent& event );
		void OnStartTransfer( wxCommandEvent& event );
		void OnListenButton( wxCommandEvent& event );
		void OnPoll( wxTimerEvent& event );
		void OnSaveAs( wxCommandEvent& event );
		void OnOpenPicture( wxCommandEvent& event );

		void SetupToolbar( wxToolBar* toolBar );


		DECLARE_EVENT_TABLE();

        DSA_11k* scopeObject;

        wxTimer pollTimer;

        wxTextCtrl* textScreen;

        wxInputStream* inputStream;

        wxToolBar* toolBar;

        wxHtmlHelpController* helpSystem;

};


#define pollTime 20


#endif // SCOPETRANSFERMAIN_H
