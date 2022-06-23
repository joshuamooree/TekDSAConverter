/***************************************************************
 * Name:      ScopeTransferMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Joshua Moore (joshua.moore7@gmail.com)
 * Created:   2007-10-18
 * Copyright: Joshua Moore ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "ScopeTransferMain.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}


int idMenuQuit = wxNewId();
int idMenuAbout = wxNewId();
int idStartTransfer = wxNewId();
int idConfig = wxNewId();
int idMainOpenRawFile = wxNewId();
int idListenButton = wxNewId();
int idPollEvent = wxNewId();
int idSaveAs = wxNewId();
int idOpenPicture = wxNewId();
int idOpenHelp = wxNewId();

BEGIN_EVENT_TABLE(ScopeTransferMain, wxMDIParentFrame)
	EVT_MENU(idMenuQuit, ScopeTransferMain::OnQuit)
	EVT_MENU(idMenuAbout, ScopeTransferMain::OnAbout)
	EVT_MENU(idConfig, ScopeTransferMain::OnConfig)
	EVT_MENU( idMainOpenRawFile, ScopeTransferMain::OnRawFile )
	EVT_MENU( idOpenHelp, ScopeTransferMain::OnOpenHelp )
	EVT_TOOL( idStartTransfer, ScopeTransferMain::OnStartTransfer )
	EVT_TOOL( idListenButton, ScopeTransferMain::OnListenButton )
	EVT_TIMER ( idPollEvent, ScopeTransferMain::OnPoll )
	EVT_TOOL( idSaveAs, ScopeTransferMain::OnSaveAs )
	EVT_TOOL( idOpenPicture, ScopeTransferMain::OnOpenPicture )
END_EVENT_TABLE()



ScopeTransferMain::ScopeTransferMain( wxFrame *frame,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size )
	: wxMDIParentFrame(frame, -1, title, pos, size ), pollTimer( this, idPollEvent )
{
	// set the frame icon
	//SetIcon(wxICON(sample));

    //setup the menus
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu(_(""));

    fileMenu ->Append(idMainOpenRawFile, _("Convert &Raw File"), _("Find a raw file to convert") );

    fileMenu ->Append(idOpenPicture, _("Open &picture file"), _("Open a picture file") );

	fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
	mbar->Append(fileMenu, _("&File"));

    wxMenu* editMenu = new wxMenu( _("") );
    editMenu ->Append( idConfig, _("&Preferences\tAlt-P"), _("Open preferences dialog") );
    mbar ->Append( editMenu, _("&Edit") );

	wxMenu* helpMenu = new wxMenu(_(""));
	helpMenu ->Append( idOpenHelp, _("&Help\tF1"), _("Show help for this program") );
	helpMenu->Append(idMenuAbout, _("&About"), _("Show info about this application") );
	mbar->Append(helpMenu, _("&Help"));

	SetMenuBar(mbar);

	this -> CreateStatusBar();

	//create a toolbar
	toolBar = this ->CreateToolBar( wxNO_BORDER | wxTB_HORIZONTAL | wxTB_TEXT );
    SetupToolbar( toolBar );
    toolBar ->Realize();

    LogAtLevel(1, _("Done making GUI"));

    //setup the help system
    helpSystem = new wxHtmlHelpController( wxHF_DEFAULT_STYLE, this );

    helpSystem->AddBook( _T("memory:Resources.zip#zip:Documentation/HelpFile.hhp") );

    LogAtLevel(1, _("Setup Help system"));

    //temporary - for testing

    scopeObject = new DSA_11k();

    LogAtLevel(1, _("Made DSA object"));
}



ScopeTransferMain::~ScopeTransferMain()
{

}


void ScopeTransferMain::SetupToolbar( wxToolBar* toolBar )
{
    #define toolBarWidth 100
    #define toolBarHeight 100

    //wxMask* mask;

    toolBar ->SetToolBitmapSize( wxSize( toolBarWidth, toolBarHeight ) );

    wxImage bitmap; // ( "icons/icon.png", wxBITMAP_TYPE_ANY );


    #define numberOfTools 3
    wxString titleStrings [numberOfTools] = { _("Listen for scope data"),
                                              _("Save as"),
                                              _("Open picture") };

    wxString helpStrings [numberOfTools] =
                          { _("Listen for data from scope and convert picture"),
                            _("Save currently selected bitmap to a file"),
                            _("Open Already existing bitmap picture") };

    int ids[numberOfTools] = {idListenButton, idSaveAs, idOpenPicture};

    wxItemKind toolTypes [numberOfTools] = { wxITEM_CHECK, wxITEM_NORMAL, wxITEM_NORMAL };

    wxString imageLocations [numberOfTools] =
                          { _T("memory:Resources.zip#zip:icons/computerListen.png"),
                            _T("memory:Resources.zip#zip:icons/saveAs.png"),
                            _T("memory:Resources.zip#zip:icons/openPicture.png") };

    //temporary variables for the loop (allows me to get the images out of the embedded zip)
    wxImage tempImage;
    wxFileSystem fs;
    wxFSFile* imageFile;

    for( int toolNumber = 0; toolNumber < numberOfTools; toolNumber++ )
    {
        imageFile = fs.OpenFile( imageLocations[toolNumber] );

        if( imageFile != NULL )
        {
            tempImage.LoadFile( *imageFile->GetStream(), imageFile->GetMimeType() );

            toolBar ->AddTool( ids[toolNumber],
                           titleStrings[toolNumber],
                           tempImage.Scale( toolBarWidth, toolBarHeight ),
                           helpStrings[toolNumber],
                           toolTypes[toolNumber] );
        }
        else
            wxLogError(_("Error loading toolbar icon %i:\n"
            "Cannot find toolbar icon resource.\n"
            "Application is either corrupt or this is a bug"), toolNumber );
    }
}

/*-------------------------------------------------------------
 * No event handlers here.  They are in mainEventHandlers.cpp.
 * They were getting out of hand here.
 *-------------------------------------------------------------*/
