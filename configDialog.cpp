#include "configDialog.h"

#define numberOfItemsForComPortLabels 16

//int idSerialPortSpeedConfig = wxNewId();
//int idSerialPortChoose = wxNewId();
//int idSerialPortParityChoose = wxNewId();

BEGIN_EVENT_TABLE(preferencesDialog, wxDialog)
	//EVT_MENU(idMenuQuit, MyFrame::OnQuit)
	//EVT_CHOICE( idSerialPortSpeedConfig, preferencesDialog::OnChooseSpeed )

END_EVENT_TABLE()

preferencesDialog::preferencesDialog( wxFrame* parent,
                                      wxWindowID id,
                                      const wxString& title,
                                      DSA_11k* scopeObject )

    : wxDialog( parent, id, title, wxDefaultPosition, wxSize( 500, 600 ) )
{
    this ->scopeObject = scopeObject;

    initializeControlVariables();

    wxBoxSizer* topLevel = new wxBoxSizer( wxVERTICAL );

    this ->SetSizer( topLevel );

    wxNotebook* pages = new wxNotebook( this, -1 );

    topLevel ->Add( pages, 1, wxALIGN_TOP | wxEXPAND );

    topLevel ->Add( CreateButtonSizer( wxOK ), 0, wxALIGN_BOTTOM | wxFIXED_MINSIZE );

    //mainPage = new wxPanel( pages );
    //pages ->AddPage( mainPage, _("Main Settings") );
    //setupMainConfigPage();
    mainPage = setupMainConfigPage( pages );
    pages ->AddPage( mainPage, _("Main Settings") );

    this ->scopePage = scopeObject ->getConfigPage( pages );
    pages ->AddPage( scopePage, _("Scope Settings") );

    //let the objects that have their own pages do their own event handling
    this ->PushEventHandler( scopeObject );

}


void preferencesDialog::initializeControlVariables()
{
    //get the config object
    wxConfigBase* config = wxConfigBase::Get();

    //save the existing path to be nice
    wxString previousPath = config ->GetPath();

    //move to where our serial port config stuff is
    config ->SetPath( _T("/Main/Serial/") );


    //set the baudRate variable to the one in the config
    this ->serialPortBaudRate.Printf( _T("%i"), config ->Read( _T("BaudRate"), ((long)wxBAUD_19200) ) );
    //serialPortSpeeds ->SetSelection( serialPortSpeedStrings.Index( tempString ) );

    //set the serialPort variable to the one in the config
    this ->serialPortSelection.Printf( _T("%s"), config ->Read( _T("ComPort"), _("Com 4") ).c_str() );

    //set the parity variable to the one in the config
    long configParity;
    config->Read( _T("Parity"), &configParity, wxPARITY_NONE );

    if( configParity == wxPARITY_NONE )
        this ->serialPortParity.Printf( _("No Parity") );
    else if ( configParity == wxPARITY_EVEN )
        this ->serialPortParity.Printf( _("Even Parity") );
    else if( configParity == wxPARITY_ODD )
        this ->serialPortParity.Printf( _("Odd Parity") );
    else    //error
        this ->serialPortParity.Printf( _("No Parity") );

    //next the data bits
    this ->serialPortDataBits.Printf( _T("%i"), config->Read( _T("DataBits"), 8L ) );

    //next the stop bits
    this ->serialPortStopBits.Printf( _T("%i"), config ->Read( _T("StopBits"), 0L ) );

    //next the xonxoff
    config ->Read( _T("xonxoff"), &this ->serialPortXonXoff, false );

    //now for the autosave settings
    config ->SetPath( _T("/Autosave/") );

    config ->Read( _T("/Main/Autosave"), &this ->AutoSavePictures, true );

    this ->AutoSavenameSpecifier = config ->Read( _T("Name Specifier"), _T("screenshot %d ") );

    this ->AutoSavePath = config ->Read( _T("Path"), _T("./"));

    this ->AutoSaveFileType = config ->Read( _T("File type"), _T(".png") );


    //now for the log settings
    config ->SetPath( _T("/Log/") );
    this ->LogLevel = config ->Read(_T("Level"), 0L);

    //restore the path now that we are done
    config ->SetPath( previousPath );
}

wxPanel* preferencesDialog::setupMainConfigPage( wxWindow* parent )
{
    /*
    //a throwaway variable - used to convert numbers to strings
    wxString tempString;

    wxFlexGridSizer* mainSizer = new wxFlexGridSizer( 5, 2, 10, 10 );

    mainPage ->SetSizer(mainSizer);

    //add a button for autosaves
    wxCheckBox* checkBox = new wxCheckBox( mainPage,
                                           -1,
                                           _("Autosave pictures") );
    mainSizer ->Add( checkBox, wxALIGN_CENTER );

    //stuff for serial port config

    wxStaticBoxSizer* serialPortStuff = new wxStaticBoxSizer( wxHORIZONTAL, mainPage, _("Serial Port")  );
    mainSizer ->Add (serialPortStuff);

    //serial port speed
    #define numberOfItemsForSerialPortSpeeds 14
    wxArrayString serialPortSpeedStrings;

    //the list of serial port speeds - in number format
    long serialPortSpeedNumbers[] = {wxBAUD_150,    wxBAUD_300,    wxBAUD_600,
                                     wxBAUD_1200,   wxBAUD_2400,   wxBAUD_4800,
                                     wxBAUD_9600,   wxBAUD_19200,  wxBAUD_38400,
                                     wxBAUD_57600,  wxBAUD_115200, wxBAUD_230400,
                                     wxBAUD_460800, wxBAUD_921600 };

    //convert that to a wxArrayString
    for( int pos = 0; pos < numberOfItemsForSerialPortSpeeds; pos++ )
    {
        tempString.Printf( _T("%i"), serialPortSpeedNumbers[pos] );
        serialPortSpeedStrings.Add( tempString );
    }

    //now we finally have a wxArrayString we can use with the choice control
    //make the control
    wxChoice* serialPortSpeeds = new wxChoice( mainPage,
                                               -1,
                                               wxDefaultPosition,
                                               wxDefaultSize,
                                               serialPortSpeedStrings );
    serialPortStuff ->Add( serialPortSpeeds );

    serialPortSpeeds ->SetValidator( wxGenericValidator( &this ->serialPortBaudRate ) );


    //add the choice box for the com ports
    //first some strings though
    const wxChar* serialPortLabels[numberOfItemsForComPortLabels] =
                                                { _("Com 1"),  _("Com 2"),  _("Com 3"),
                                                  _("Com 4"),  _("Com 5"),  _("Com 6"),
                                                  _("Com 7"),  _("Com 8"),  _("Com 9"),
                                                  _("Com 10"), _("Com 11"), _("Com 12"),
                                                  _("Com 13"), _("Com 14"), _("Com 15"),
                                                  _("Com 16") };


    //make the labels into a wxArrayString for easy handling
    wxArrayString serialPortLabelsAsArrayString( numberOfItemsForComPortLabels, serialPortLabels );

    //make the choice box
    wxChoice* serialPortSelection = new wxChoice( mainPage,
                                                 -1,
                                                  wxDefaultPosition,
                                                  wxDefaultSize,
                                                  serialPortLabelsAsArrayString );
    serialPortStuff ->Add( serialPortSelection );

    serialPortSelection ->SetValidator( wxGenericValidator( &this ->serialPortSelection ) );


    //add a choice box for parity
    //labels first
    const wxChar* parityLabels [3] = {_("No Parity"),
                                      _("Even Parity"),
                                      _("Odd Parity") };
    wxArrayString parityChoices( 3, parityLabels );

    //now the control
    wxChoice* parityChoiceBox = new wxChoice( mainPage,
                                              -1,
                                              wxDefaultPosition,
                                              wxDefaultSize,
                                              parityChoices );
    serialPortStuff ->Add( parityChoiceBox );

    parityChoiceBox ->SetValidator( wxGenericValidator( &this ->serialPortParity ) );


    //add a choice box for data bits
    #define numberOfItemsForDataBitsBox 5

    int dataBitsValues[] = { 5, 6, 7, 8, 16 };

    wxArrayString dataBitsBoxValues;

    for( int pos = 0; pos < numberOfItemsForDataBitsBox; pos++ )
    {
        tempString.Printf( _T("%i"), dataBitsValues[pos] );
        dataBitsBoxValues.Add( tempString );
    }

    wxChoice* dataBitsSelection = new wxChoice( mainPage,
                                                -1,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                dataBitsBoxValues);
    serialPortStuff->Add( dataBitsSelection );

    dataBitsSelection ->SetValidator( wxGenericValidator( &this ->serialPortDataBits ) );

    #define numberOfItemsForStopBits 2
    int stopBitsValues[] = { 0, 1 };

    wxArrayString stopBitsBoxValues;
    for( int pos = 0; pos < numberOfItemsForStopBits; pos++ )
    {
        tempString.Printf( _T("%i"), stopBitsValues[ pos ] );
        stopBitsBoxValues.Add( tempString );
    }

    wxChoice* stopBitsSelection = new wxChoice( mainPage,
                                                -1,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                stopBitsBoxValues);
    serialPortStuff ->Add( stopBitsSelection );
    stopBitsSelection ->SetValidator( wxGenericValidator( &this ->serialPortStopBits ) );

    wxCheckBox* selectXonXoff = new wxCheckBox( mainPage,
                                                -1,
                                                _("XonXoff") );
    serialPortStuff ->Add( selectXonXoff );
    selectXonXoff ->SetValidator( wxGenericValidator( &this ->serialPortXonXoff ) );
    */

    //MainConfigPage configPage

    //mainPage ->InitDialog();
    MainConfigPage* ConfigPage = new MainConfigPage( parent, -1, wxDefaultPosition, wxDefaultSize );

    ConfigPage ->AutosaveCheckbox ->SetValidator( wxGenericValidator( &this ->AutoSavePictures ) );

    ConfigPage -> serialPortSpeeds ->SetValidator( wxGenericValidator( &this ->serialPortBaudRate ) );

    ConfigPage -> serialPortSelection ->SetValidator( wxGenericValidator( &this ->serialPortSelection ) );

    ConfigPage -> parityChoiceBox ->SetValidator( wxGenericValidator( &this ->serialPortParity ) );

    ConfigPage -> dataBitsSelection ->SetValidator( wxGenericValidator( &this ->serialPortDataBits ) );

    ConfigPage -> stopBitsSelection ->SetValidator( wxGenericValidator( &this ->serialPortStopBits ) );

    ConfigPage -> selectXonXoff ->SetValidator( wxGenericValidator( &this ->serialPortXonXoff ) );

    ConfigPage -> FormatTextBox ->SetValidator( wxGenericValidator( &this ->AutoSavenameSpecifier ) );

    ConfigPage -> LocationTextBox ->SetValidator( wxGenericValidator( &this ->AutoSavePath ) );

    ConfigPage -> AutoSaveExtension ->SetValidator( wxGenericValidator( &this ->AutoSaveFileType ) );

    ConfigPage -> LogChoice ->SetValidator( wxGenericValidator( &this ->LogLevel ) );

    ConfigPage ->InitDialog();

    ConfigPage ->TransferDataToWindow();

    return ConfigPage;
}


preferencesDialog::~preferencesDialog()
{
    int previousLogLevel = this ->LogLevel;

    mainPage ->TransferDataFromWindow();

    if(previousLogLevel != this ->LogLevel)
        if(previousLogLevel > 0)
            if(this -> LogLevel == 0)
                LogAtLevel(0, _("Log Disabled"));


    this ->scopeObject ->destroyConfigPage( this ->scopePage );

    saveControlVariables();

    if(previousLogLevel != this ->LogLevel )
        if(this ->LogLevel > 0)
            LogAtLevel(1, _("Log Enabled, Level: %i"), this ->LogLevel);

    //wxLogError( this ->serialPortParity );
}

void preferencesDialog::saveControlVariables()
{
        //get the config object
        wxConfigBase* config = wxConfigBase::Get();

        //save the existing path to be nice
        wxString previousPath = config ->GetPath();
        //move to where our serial port config stuff is
        config ->SetPath( _T("/Main/Serial/") );

        //first the baudRate
        long baudRate;
        this ->serialPortBaudRate.ToLong( &baudRate );
        config ->Write( _T("BaudRate"), baudRate );

        //next the serial port
        config ->Write( _T("ComPort"), this ->serialPortSelection );

        //next the parity
        if( this ->serialPortParity == _("No Parity") )
            config ->Write( _T("Parity"), wxPARITY_NONE );
        else if( this ->serialPortParity == _("Even Parity") )
            config ->Write( _T("Parity"), wxPARITY_EVEN );
        else if( this ->serialPortParity == _("Odd Parity") )
            config ->Write( _T("Parity"), wxPARITY_ODD );
        else    //error, unhandled for now - set to something safe
            config ->Write( _T("Parity"), wxPARITY_NONE );

        //next the data bits
        long dataBits;
        this ->serialPortDataBits.ToLong( &dataBits );
        config ->Write( _T("DataBits"), dataBits );

        //next the stop bits
        long stopBits;
        this ->serialPortStopBits.ToLong( &stopBits );
        config ->Write( _T("StopBits"), stopBits );

        //next xon xoff
        config ->Write( _T("xonxoff"), this ->serialPortXonXoff );

        //autosave stuff
        config ->Write( _("/Main/Autosave"), this ->AutoSavePictures );
        config ->SetPath( _T("/Autosave/") );

        config ->Write( _T("Name Specifier"), this ->AutoSavenameSpecifier );
        config ->Write( _T("Path"), this ->AutoSavePath );
        config ->Write( _T("File type"), this ->AutoSaveFileType );

        //now for the log settings
        config ->SetPath( _T("/Log/") );
        config ->Write(_T("Level"), this ->LogLevel);

        //restore the path now that we are done
        config ->SetPath( previousPath );
}

