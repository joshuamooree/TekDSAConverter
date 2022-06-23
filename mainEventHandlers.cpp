#include "ScopeTransferMain.h"


void ScopeTransferMain::OnQuit(wxCommandEvent& event)
{
    LogAtLevel(1, "Received OnQuit event.  Closing...");

    Destroy();
	//Close();
}

void ScopeTransferMain::OnClose(wxCloseEvent &event)
{
    LogAtLevel(1, "Received OnClose event.  Closing...");

    Destroy();
}

void ScopeTransferMain::OnAbout(wxCommandEvent& event)
{
    LogAtLevel(1, "Received OnAbout event.  Displaying about menu...");

	wxMessageBox(_("Scope transfer"), _("Welcome to my scope transfer application\n"
                                        "By: Joshua Moore\n"
                                        "License: none yet (maybe GPL or LGPL in the future)"));
}

void ScopeTransferMain::OnConfig( wxCommandEvent& event )
{
    LogAtLevel(1, "Received OnConfig event.  Displaying config page...");

    preferencesDialog preferences( this, -1, _("Preferences"), scopeObject );

    preferences.ShowModal();
}

void ScopeTransferMain::OnRawFile( wxCommandEvent& event )
{
    LogAtLevel(1, "Received OnRawFile event.  Beginning conversion...");

    wxString fileName = wxFileSelector( _("Find raw file to convert"),
                                        _T(""),
                                        _T(""),
                                        _T(""),
                                        _T("*.*"),
                                        0,
                                        this );

    //did the user cancel or select something?
    if( fileName.IsEmpty() )
    {
        LogAtLevel(1, "In OnRawFile event handler: Early cancel");
        return;     //he canceled
    }

    wxFileInputStream userFile( fileName );

    if( !userFile.Ok() )
    {
        wxLogError( _("Could not open file: %s.  Conversion canceled"),
                    fileName.c_str() );

        LogAtLevel(1, "In OnRawFile event handler: Bad file name");
        return;
    }

    //we have a file - try to process it
    if( !scopeObject -> processHeader( &userFile ) )
    {
        //if the header is not right, none of the rest will be either
        wxLogError( _("%s is not a valid file.  Conversion canceled"),
                    fileName.c_str() );

        LogAtLevel(1, "In OnRawFile event handler: Invalid file");
        return;
    }

    LogAtLevel(1, "In OnRawFile event handler: File passes validity check");

    //ok, we have a file and the header processed ok.  Now, do the rest
    if( !scopeObject ->processData( &userFile, OnUpdateProgress ) )
    {
        //set the progress to 100% to make sure it goes away
        OnUpdateProgress( 100 );
        wxLogError( _("Conversion canceled") );

        LogAtLevel(1, "In OnRawFile event handler: Conversion canceled by user");
        return;
    }

    LogAtLevel(1, "In OnRawFile event handler: conversion succeeded or so it seems");

    MDIPictureFrame* frame = new MDIPictureFrame( scopeObject ->getBitmap(),
                                                  this,
                                                  -1,
                                                  fileName );

     LogAtLevel(1, "In OnRawFile event handler: Now saving picture");

     AutoSavePicture( scopeObject ->getBitmap() );

     LogAtLevel(1, "Done with OnRawFile event handler");
}   //end method OnRawFile

void ScopeTransferMain::OnListenButton( wxCommandEvent& event )
{
    LogAtLevel(1, "Received OnListenButton event.  Beginning...");

    if( pollTimer.IsRunning() )
    {
        LogAtLevel(1, "In OnListenButton event handler: stopping poll timer...");

        pollTimer.Stop();
        delete inputStream;
        inputStream = NULL;
    }
    else
    {
        LogAtLevel(1, "In OnListenButton event handler: Starting poll timer...");
        pollTimer.Start( pollTime );

        if( inputStream == NULL )
        {
            //get the config object
            wxConfigBase* config = wxConfigBase::Get();

            //save the existing path to be nice
            wxString previousPath = config ->GetPath();

            //move to where our config stuff is
            config ->SetPath( _T("/Main/Serial/") );

            int dataBits = 8;
            int stopBits = 1;
            wxString comPort = _T(wxCOM4);
            wxParity parity = wxPARITY_NONE;
            wxBaud baudRate = wxBAUD_19200;
            bool xonxoff = false;

            config ->Read( _T("ComPort"), &comPort, _("Com 4") );
            config ->Read( _T("BaudRate"), (int*)&baudRate, wxBAUD_19200 );
            config ->Read( _T("Parity"), (int*)&parity, (wxPARITY_NONE ) );
            config ->Read( _T("DataBits"), &dataBits, 8 );
            config ->Read( _T("StopBits"), &stopBits, 1 );
            config ->Read( _T("xonxoff"), &xonxoff, false);

            //restore the path now that we are done with the config
            config ->SetPath( previousPath );

            //tables to allow us to convert from serial port labels to paths
            //what we do is we find the com port in serial port labels, then we
            //access that item in serialPortPaths
            #define numberOfItemsForComPortLabels 9
            const wxChar* serialPortLabels[numberOfItemsForComPortLabels] =
                                                { _("Com 1"),  _("Com 2"),  _("Com 3"),
                                                  _("Com 4"),  _("Com 5"),  _("Com 6"),
                                                  _("Com 7"),  _("Com 8"),  _("Com 9") };
                                                  /*
                                                  removed because Linux only has up to com 9 defined
                                                  ,
                                                  _("Com 10"), _("Com 11"), _("Com 12"),
                                                  _("Com 13"), _("Com 14"), _("Com 15"),
                                                  _("Com 16") };
                                                  */


            const char* serialPortPaths[numberOfItemsForComPortLabels] =
                                                { wxCOM1,  wxCOM2,  wxCOM3,
                                                  wxCOM4,  wxCOM5,  wxCOM6,
                                                  wxCOM7,  wxCOM8,  wxCOM9 };

                                                  /*
                                                  removed becuause Linux only has up to com 9 defined
                                                  ,
                                                  _T(wxCOM10), _T(wxCOM11), _T(wxCOM12),
                                                  _T(wxCOM13), _T(wxCOM14), _T(wxCOM15),
                                                  _T(wxCOM16) };
                                                  */

            wxArrayString serialPortLabelsAsArrayString( numberOfItemsForComPortLabels, serialPortLabels );

            LogAtLevel(1, "In OnListenButton event handler: Trying to open serial port...");
            inputStream = new SerialInputStream( serialPortPaths[serialPortLabelsAsArrayString.Index(comPort)],
                                                 baudRate,
                                                 parity,
                                                 (unsigned char)dataBits,
                                                 (unsigned char)stopBits,
                                                 xonxoff );



            //of course, we can't just open the serial port.  We need to
            //see if it opened ok and do something about it if it didn't
            if( inputStream ->GetLastError() != wxSTREAM_NO_ERROR )//||
                //inputStream ->GetLastError() != wxSTREAM_EOF )
            {
                //delete the object (closes the serial port)
                delete inputStream;
                inputStream = NULL;

                //no reason for the polling to continue
                pollTimer.Stop();

                //Since the toolbar button for this is
                //a toggle button, we need to untoggle it.
                toolBar -> ToggleTool( event.GetId(), false );

                wxLogError( _T("Error opening serial port.\n"
                            "Try checking serial port setup.") );

                LogAtLevel(1, "In OnListenButton event handler: Error opening serial port.  Error code: %i", inputStream ->GetLastError() );

            }   //end if for error checking

            LogAtLevel(1, "In OnListenButton event handler: Succeeded in opening serial port");

        }   //end if for setting up serial port
    }   //end if ... else for turning polling on and off

    LogAtLevel(1, "Done with OnListenButton event handler");
}   //end method OnListenButton

void ScopeTransferMain::OnPoll( wxTimerEvent& event )
{
    //first some error checking
    if( inputStream ->IsOk() == false )
    {
        //try resetting
        inputStream ->Reset();

        return; //big error maybe we need to do more here?
    }

    //see if we received anything
    if( !((SerialInputStream*)inputStream) ->isByteWaiting() )
        return; //nope, nothing received, so just exit

/*
    char tempChar = inputStream ->GetC();

    if( inputStream ->IsOk() == false )
    {
        //reset
        inputStream ->Reset();

        return; //no data - yet
    }

    //put back that character we just got.  It was just to see if there is
    //anything there.
    inputStream ->Ungetch( tempChar );
*/

    //we must have something
    //lets start processing
    //first the header
    pollTimer.Stop();

    LogAtLevel(1, "In OnPoll event handler: Got a byte, beginning processing");

    if( scopeObject -> processHeader( inputStream ) == false )
    {
        pollTimer.Start( pollTime );

        return;  //invalid header.  We could show an error here, but I won't for now
    }

    LogAtLevel(1, "In OnPoll event handler: Valid header");

    if( scopeObject ->processData( inputStream, OnUpdateProgress ) == false )
    {
        LogAtLevel(1, "In OnPoll event handler: Bad data. Exiting");
        pollTimer.Start( pollTime );

        //clean up the progress bar - make sure it is 100 percent so it goes away
        OnUpdateProgress( 100 );
        return;  //error occured.  I could show an error here, but I won't for now
    }

    LogAtLevel(1, "In OnPoll event handler: Valid data (or so it seems)");

    //we must have it.  Everything seems to check out.
    MDIPictureFrame* frame = new MDIPictureFrame( scopeObject ->getBitmap(),
                                                  this,
                                                  -1,
                                                  scopeObject ->getHeader() );


    //autosave the picture
    AutoSavePicture( scopeObject ->getBitmap() );

    inputStream ->Reset();
    LogAtLevel(1, "Done with OnPoll event handler");
    pollTimer.Start( pollTime );

}   //end method OnPoll

void ScopeTransferMain::OnStartTransfer( wxCommandEvent& event )
{
    wxMessageBox( _("Feature not implemented yet") );
}

void ScopeTransferMain::OnSaveAs( wxCommandEvent& event )
{
    LogAtLevel(1, "Received OnSaveAs event.  Beginning...");
    //todo: do as a dynamic cast with type checking
    wxMDIChildFrame* activeFrame = this ->GetActiveChild();
    wxImage currentPicture;
    wxString fileName;

    //is the frame OK?
    if( activeFrame != NULL )
    {
        //its OK.  Get the picture.  I really should use RTTI and check here,
        //but this will work for now
        currentPicture = ((MDIPictureFrame*)activeFrame) -> GetBitmap() -> ConvertToImage();

        fileName = wxFileSelector( _("Save as"),    //message
                                   _T(""),          //default path
                                   _T(""),          //default filename
                                   _T(""),          //default extension
                                   _("PNG file (*.png)|*.png|"
                                     "BMP file (*.bmp)|*.bmp|"
                                     "JPEG file (*.jpg)|*jpg|"
                                     "TIFF file (*.tif)|*.tif|"
                                     "PCX file (*.pcx)|*.pcx"), //file types
                                   wxSAVE | wxOVERWRITE_PROMPT,  //flag
                                   this );                       //parent
        //did the user cancel?
        if ( fileName.IsEmpty() )
        {
            LogAtLevel(1, "In OnSaveAs event handler: Canceled.  Exiting Handler.");
            return;
        }

        LogAtLevel(1, "In OnSaveAs event handler: Got filename");

        //The user didn't cancel.  Lets try saving.
        if( !currentPicture.SaveFile( fileName ) )
        {
            wxLogError( _("Could not save file.") );
            LogAtLevel(1, "In OnSaveAs event handler: Error saving");
        }
        else
            LogAtLevel(1, "In OnSaveAs event handler: Saved picture");
    }
    else
    {
        //nope, there was no frame.  Show an error.  May not be necessary thougn
        wxMessageBox( _("No Pictures To Save"),
                      _("No picture"),
                      wxOK | wxICON_INFORMATION,
                      this );

        LogAtLevel(1, "In OnSaveAs event handler: No frame - couldn't save picture.");
    }
    LogAtLevel(1, "Done with OnSaveAs event handler");
}   //end method OnSaveAs

void ScopeTransferMain::OnOpenPicture( wxCommandEvent& event )
{
    LogAtLevel(1, "Received OnOpenPicture event");

    wxImage openedImage;
    MDIPictureFrame* pictureFrame;
    wxString fileName = wxFileSelector( _("Choose a picture to open"), //message
                                        _(""),                         //default path
                                        _(""),                         //default filename
                                        _(""),                         //default extension
                                        _("PNG file (*.png)|*.png|"    //wildcards
                                          "BMP file (*.bmp)|*.bmp|"    //for file types
                                          "JPEG file (*.jpg)|*jpg|"
                                          "TIFF file (*.tif)|*.tif|"
                                          "PCX file (*.pcx)|*.pcx|"
                                          "All supported image types ") +
                                          wxImage::GetImageExtWildcard(),
                                        wxOPEN | wxFILE_MUST_EXIST,    //flags
                                        this );                        //parent
    //did the user cancel?
    if( fileName.IsEmpty() )
    {
        LogAtLevel(1, "In OnOpenPicture event handler: User cancelled");
        return;
    }

    LogAtLevel(1, "In OnOpenPicture event handler: User picked file");

    //so, the file name should be good.  Lets try opening the file now
    if( !openedImage.LoadFile( fileName ) )
    {
        wxLogError( _("Could not open file") );
        LogAtLevel(1, "In OnOpenPicture event handler: Bad file.  Filename: %s", fileName.c_str() );
    }

    //ok, now we should have the image.  Lets show it to the user.
    pictureFrame = new MDIPictureFrame( wxBitmap( openedImage ),
                                        this,
                                        -1,
                                        fileName );

    LogAtLevel(1, "Done with OnOpenPicture event handler");
}   //end method OnOpenPicture

void ScopeTransferMain::OnOpenHelp( wxCommandEvent& event )
{
    LogAtLevel(1, "Received OnOpenHelp event");
    this ->helpSystem ->DisplayContents();
}
