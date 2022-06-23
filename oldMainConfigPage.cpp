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
const long MainConfigPage::SavePicturesCheck = wxNewId();
const long MainConfigPage::serialPortSpeeds = wxNewId();
const long MainConfigPage::ID_CHOICE2 = wxNewId();
const long MainConfigPage::ID_CHOICE3 = wxNewId();
const long MainConfigPage::ID_CHOICE4 = wxNewId();
const long MainConfigPage::ID_CHOICE5 = wxNewId();
//*)

BEGIN_EVENT_TABLE(MainConfigPage,wxPanel)
	//(*EventTable(MainConfigPage)
	//*)
END_EVENT_TABLE()

MainConfigPage::MainConfigPage(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(MainConfigPage)
	wxGridSizer* GridSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	GridSizer1 = new wxGridSizer(3, 1, 0, 0);
	CheckBox1 = new wxCheckBox(this, SavePicturesCheck, _("Autosave pictures"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("SavePicturesCheck"));
	CheckBox1->SetValue(false);
	GridSizer1->Add(CheckBox1, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Serial port"));
	Choice1 = new wxChoice(this, serialPortSpeeds, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("serialPortSpeeds"));
	Choice1->Append(_("150"));
	Choice1->Append(_("300"));
	Choice1->Append(_("600"));
	Choice1->Append(_("1200"));
	Choice1->Append(_("2400"));
	Choice1->Append(_("4800"));
	Choice1->Append(_("9600"));
	Choice1->SetSelection( Choice1->Append(_("19200")) );
	Choice1->Append(_("38400"));
	Choice1->Append(_("57600"));
	Choice1->Append(_("115200"));
	Choice1->Append(_("230400"));
	Choice1->Append(_("460800"));
	Choice1->Append(_("912600"));
	StaticBoxSizer1->Add(Choice1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Choice2 = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	Choice2->Append(_("Com 1"));
	Choice2->Append(_("Com 2"));
	Choice2->Append(_("Com 3"));
	Choice2->SetSelection( Choice2->Append(_("Com 4")) );
	Choice2->Append(_("Com 5"));
	Choice2->Append(_("Com 6"));
	Choice2->Append(_("Com 7"));
	Choice2->Append(_("Com 8"));
	Choice2->Append(_("Com 9"));
	Choice2->Append(_("Com 10"));
	Choice2->Append(_("Com 11"));
	Choice2->Append(_("Com 12"));
	Choice2->Append(_("Com 13"));
	Choice2->Append(_("Com 14"));
	Choice2->Append(_("Com 15"));
	Choice2->Append(_("Com 16"));
	StaticBoxSizer1->Add(Choice2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Choice3 = new wxChoice(this, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
	Choice3->SetSelection( Choice3->Append(_("No parity")) );
	Choice3->Append(_("Even parity"));
	Choice3->Append(_("Odd parity"));
	StaticBoxSizer1->Add(Choice3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Choice4 = new wxChoice(this, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
	Choice4->Append(_("5"));
	Choice4->Append(_("6"));
	Choice4->Append(_("7"));
	Choice4->SetSelection( Choice4->Append(_("8")) );
	Choice4->Append(_("16"));
	StaticBoxSizer1->Add(Choice4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Choice5 = new wxChoice(this, ID_CHOICE5, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
	Choice5->SetSelection( Choice5->Append(_("0")) );
	Choice5->Append(_("1"));
	StaticBoxSizer1->Add(Choice5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(GridSizer1);
	GridSizer1->Fit(this);
	GridSizer1->SetSizeHints(this);
	//*)
}

MainConfigPage::~MainConfigPage()
{
	//(*Destroy(MainConfigPage)
	//*)
}


void MainConfigPage::OnCheckBox1Click(wxCommandEvent& event)
{
}
