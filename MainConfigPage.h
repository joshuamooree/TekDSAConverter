#ifndef MAINCONFIGPAGE_H
#define MAINCONFIGPAGE_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(MainConfigPage)
	#include <wx/sizer.h>
	#include <wx/stattext.h>
	#include <wx/textctrl.h>
	#include <wx/checkbox.h>
	#include <wx/panel.h>
	#include <wx/choice.h>
	#include <wx/button.h>
	//*)
#endif
//(*Headers(MainConfigPage)
//*)

class MainConfigPage: public wxPanel
{
	public:

		MainConfigPage(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~MainConfigPage();

		//(*Declarations(MainConfigPage)
		wxTextCtrl* FormatTextBox;
		wxChoice* serialPortSpeeds;
		wxStaticText* StaticText2;
		wxChoice* parityChoiceBox;
		wxButton* AutoSaveChooseLocation;
		wxChoice* AutoSaveExtension;
		wxChoice* LogChoice;
		wxChoice* serialPortSelection;
		wxTextCtrl* LocationTextBox;
		wxCheckBox* selectXonXoff;
		wxChoice* stopBitsSelection;
		wxCheckBox* AutosaveCheckbox;
		wxChoice* dataBitsSelection;
		wxButton* AutosaveHelpButton;
		//*)

		//(*Identifiers(MainConfigPage)
		static const long ID_CHOICE1;
		static const long ID_CHOICE2;
		static const long ID_CHOICE4;
		static const long ID_CHOICE5;
		static const long ID_CHOICE3;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX1;
		static const long ID_AutoSaveHelpButton;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE6;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON1;
		static const long ID_CHOICE7;
		//*)

	private:

		//(*Handlers(MainConfigPage)
		void OnAutosaveHelpButtonClick(wxCommandEvent& event);
		void OnAutoSaveChooseLocationClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
