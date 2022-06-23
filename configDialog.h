#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include "ScopeTransferApp.h"
#include "DSA_11k.h"
#include "SerialInputStream.h"

#include "MainConfigPage.h"

class preferencesDialog: public wxDialog
{
    public:
        preferencesDialog( wxFrame* parent,
                       wxWindowID id,
                       const wxString& title,
                       DSA_11k* scopeObject );

        ~preferencesDialog();

    protected:
        void initializeControlVariables();
        void saveControlVariables();

        wxPanel* setupMainConfigPage( wxWindow* parent );
        wxPanel* mainPage;
        DECLARE_EVENT_TABLE()

        //variables representing data from the controls
        wxString serialPortBaudRate;
        wxString serialPortSelection;
        wxString serialPortParity;
        wxString serialPortDataBits;
        wxString serialPortStopBits;
        bool serialPortXonXoff;

        DSA_11k* scopeObject;

        wxPanel* scopePage;

        bool AutoSavePictures;
        wxString AutoSavenameSpecifier;
        wxString AutoSavePath;
        wxString AutoSaveFileType;

        int LogLevel;
};


#endif //_CONFIGDIALOG_H_
