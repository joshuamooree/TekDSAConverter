#include "wx_pch.h"
#include "MainConfigPage.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(MainConfigPage)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(MainConfigPage)
//*)

//(*IdInit(MainConfigPage)
const long MainConfigPage::ID_CHOICE1 = wxNewId();
const long MainConfigPage::ID_CHOICE2 = wxNewId();
const long MainConfigPage::ID_CHOICE4 = wxNewId();
const long MainConfigPage::ID_CHOICE5 = wxNewId();
const long MainConfigPage::ID_CHOICE3 = wxNewId();
const long MainConfigPage::ID_CHECKBOX2 = wxNewId();
const long MainConfigPage::ID_CHECKBOX1 = wxNewId();
const long MainConfigPage::ID_AutoSaveHelpButton = wxNewId();
const long MainConfigPage::ID_TEXTCTRL1 = wxNewId();
const long MainConfigPage::ID_STATICTEXT2 = wxNewId();
const long MainConfigPage::ID_CHOICE6 = wxNewId();
const long MainConfigPage::ID_TEXTCTRL2 = wxNewId();
const long MainConfigPage::ID_BUTTON1 = wxNewId();
const long MainConfigPage::ID_CHOICE7 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainConfigPage,wxPanel)
	//(*EventTable(MainConfigPage)
	//*)
END_EVENT_TABLE()

MainConfigPage::MainConfigPage(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(MainConfigPage)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxFlexGridSizer* FlexGridSizer4;
	wxStaticBoxSizer* StaticBoxSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticBoxSizer* StaticBoxSizer7;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxStaticBoxSizer* StaticBoxSizer6;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxStaticBoxSizer* StaticBoxSizer5;
	wxGridSizer* GridSizer2;
	
	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Serial port"));
	GridSizer2 = new wxGridSizer(2, 3, 0, 0);
	serialPortSpeeds = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	serialPortSpeeds->Append(_("150"));
	serialPortSpeeds->Append(_("300"));
	serialPortSpeeds->Append(_("600"));
	serialPortSpeeds->Append(_("1200"));
	serialPortSpeeds->Append(_("2400"));
	serialPortSpeeds->Append(_("4800"));
	serialPortSpeeds->Append(_("9600"));
	serialPortSpeeds->Append(_("19200"));
	serialPortSpeeds->Append(_("38400"));
	serialPortSpeeds->Append(_("57600"));
	serialPortSpeeds->Append(_("115200"));
	serialPortSpeeds->Append(_("230400"));
	serialPortSpeeds->Append(_("460800"));
	serialPortSpeeds->Append(_("9321600"));
	GridSizer2->Add(serialPortSpeeds, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	serialPortSelection = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	serialPortSelection->Append(_("Com 1"));
	serialPortSelection->Append(_("Com 2"));
	serialPortSelection->Append(_("Com 3"));
	serialPortSelection->Append(_("Com 4"));
	serialPortSelection->Append(_("Com 5"));
	serialPortSelection->Append(_("Com 6"));
	serialPortSelection->Append(_("Com 7"));
	serialPortSelection->Append(_("Com 8"));
	serialPortSelection->Append(_("Com 9"));
	serialPortSelection->Append(_("Com 10"));
	serialPortSelection->Append(_("Com 11"));
	serialPortSelection->Append(_("Com 12"));
	serialPortSelection->Append(_("Com 13"));
	serialPortSelection->Append(_("Com 14"));
	serialPortSelection->Append(_("Com 15"));
	serialPortSelection->Append(_("Com 16"));
	GridSizer2->Add(serialPortSelection, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Data Bits"));
	dataBitsSelection = new wxChoice(this, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
	dataBitsSelection->Append(_("5"));
	dataBitsSelection->Append(_("6"));
	dataBitsSelection->Append(_("7"));
	dataBitsSelection->Append(_("8"));
	dataBitsSelection->Append(_("16"));
	StaticBoxSizer3->Add(dataBitsSelection, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer2->Add(StaticBoxSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Stop Bits"));
	stopBitsSelection = new wxChoice(this, ID_CHOICE5, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
	stopBitsSelection->Append(_("0"));
	stopBitsSelection->Append(_("1"));
	StaticBoxSizer2->Add(stopBitsSelection, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer2->Add(StaticBoxSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	parityChoiceBox = new wxChoice(this, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
	parityChoiceBox->Append(_("No Parity"));
	parityChoiceBox->Append(_("Even Parity"));
	parityChoiceBox->Append(_("Odd Parity"));
	GridSizer2->Add(parityChoiceBox, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	selectXonXoff = new wxCheckBox(this, ID_CHECKBOX2, _("XonXoff"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	selectXonXoff->SetValue(false);
	GridSizer2->Add(selectXonXoff, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(GridSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Autosave Settings"));
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	AutosaveCheckbox = new wxCheckBox(this, ID_CHECKBOX1, _("Autosave Pictures"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	AutosaveCheckbox->SetValue(false);
	FlexGridSizer2->Add(AutosaveCheckbox, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Picture name format"));
	FlexGridSizer3 = new wxFlexGridSizer(0, 4, 0, 0);
	AutosaveHelpButton = new wxButton(this, ID_AutoSaveHelpButton, _("\?"), wxDefaultPosition, wxSize(20,-1), 0, wxDefaultValidator, _T("ID_AutoSaveHelpButton"));
	AutosaveHelpButton->SetToolTip(_("Format string may be any combination\nof legal characters for the platform.\nInvalid characters will be replaced with _.\nValue substitutions are used.\nValid substitutions are:\n\n%% for the % character\n%d for the current date\n%t for the current time\n%y for the current year\n%M for the current month\n%h for the current hour\n%m for the current minute\n%s for the current second\n\nAll autosave file names will have\na number appended (the first file\nwill be yourName 01, the second\nyourName 02, etc)"));
	FlexGridSizer3->Add(AutosaveHelpButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FormatTextBox = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxSize(150,-1), wxTE_LEFT, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FormatTextBox->SetMaxLength(50);
	FormatTextBox->SetToolTip(_("Format string may be any combination\nof legal characters for the platform.\nInvalid characters will be replaced with _.\nValue substitutions are used.\nValid substitutions are:\n\n%% for the % character\n%d for the current date\n%t for the current time\n%y for the current year\n%M for the current month\n%h for the current hour\n%m for the current minute\n%s for the current second\n\nAll autosave file names will have\na number appended (the first file\nwill be yourName 01, the second\nyourName 02, etc)"));
	FlexGridSizer3->Add(FormatTextBox, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("##"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	StaticText2->SetMinSize(wxSize(0,0));
	FlexGridSizer3->Add(StaticText2, 1, wxALL|wxFIXED_MINSIZE|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	AutoSaveExtension = new wxChoice(this, ID_CHOICE6, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
	AutoSaveExtension->Append(_(".png"));
	AutoSaveExtension->Append(_(".bmp"));
	AutoSaveExtension->Append(_(".jpg"));
	AutoSaveExtension->Append(_(".tiff"));
	AutoSaveExtension->Append(_(".pcx"));
	FlexGridSizer3->Add(AutoSaveExtension, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer5->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2->Add(StaticBoxSizer5, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer6 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Autosave location"));
	FlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	LocationTextBox = new wxTextCtrl(this, ID_TEXTCTRL2, _("Text"), wxDefaultPosition, wxSize(250,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	FlexGridSizer4->Add(LocationTextBox, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	AutoSaveChooseLocation = new wxButton(this, ID_BUTTON1, _(".."), wxDefaultPosition, wxSize(20,-1), 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer4->Add(AutoSaveChooseLocation, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer6->Add(FlexGridSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2->Add(StaticBoxSizer6, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer4->Add(FlexGridSizer2, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer7 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Logging"));
	LogChoice = new wxChoice(this, ID_CHOICE7, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE7"));
	LogChoice->SetSelection( LogChoice->Append(_("Normal (errors messages only)")) );
	LogChoice->Append(_("Log to File"));
	LogChoice->Append(_("Advanced log (verbose with data)"));
	StaticBoxSizer7->Add(LogChoice, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer7, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	
	Connect(ID_AutoSaveHelpButton,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainConfigPage::OnAutosaveHelpButtonClick);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MainConfigPage::OnAutoSaveChooseLocationClick);
	//*)
}

MainConfigPage::~MainConfigPage()
{
	//(*Destroy(MainConfigPage)
	//*)
}


void MainConfigPage::OnAutosaveHelpButtonClick(wxCommandEvent& event)
{
    wxMessageBox ( _("Format string may be any combination\n"
                     "of legal characters for the platform.\n"
                     "Invalid characters will be replaced with _.\n"
                     "Value substitutions are used.\n"
                     "Valid substitutions are:\n\n"
                     "%% for the % character\n"
                     "%d for the current date\n"
                     "%t for the current time\n"
                     "%y for the current year\n"
                     "%M for the current month\n"
                     "%h for the current hour\n"
                     "%m for the current minute\n"
                     "%s for the current second\n\n"
                     "All autosave file names will have\n"
                     "a number appended (the first file\n"
                     "will be yourName 01, the second\n"
                     "yourName 02, etc)") );
}

void MainConfigPage::OnAutoSaveChooseLocationClick(wxCommandEvent& event)
{
    wxString userDirectory;
    userDirectory = wxDirSelector( _("Choose autosave folder"), LocationTextBox ->GetValue() );

    if( userDirectory.length() > 0 )
        LocationTextBox -> ChangeValue( userDirectory );
}
