#ifndef MAINCONFIGPAGE_H
#define MAINCONFIGPAGE_H

#ifndef WX_PRECOMP
	//(*HeadersPCH(MainConfigPage)
	#include <wx/sizer.h>
	#include <wx/checkbox.h>
	#include <wx/panel.h>
	#include <wx/choice.h>
	//*)
#endif
//(*Headers(MainConfigPage)
//*)

class MainConfigPage: public wxPanel
{
	public:

		MainConfigPage(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~MainConfigPage();

		//(*Declarations(MainConfigPage)
		wxChoice* Choice5;
		wxChoice* Choice3;
		wxCheckBox* CheckBox1;
		wxChoice* Choice4;
		wxChoice* Choice1;
		wxChoice* Choice2;
		//*)

	protected:

		//(*Identifiers(MainConfigPage)
		static const long SavePicturesCheck;
		static const long serialPortSpeeds;
		static const long ID_CHOICE2;
		static const long ID_CHOICE3;
		static const long ID_CHOICE4;
		static const long ID_CHOICE5;
		//*)

	private:

		//(*Handlers(MainConfigPage)
		void OnCheckBox1Click(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
