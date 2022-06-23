/***************************************************************
 * Name:      ScopeTransferApp.h
 * Purpose:   Defines Application Class
 * Author:    Joshua Moore (joshua.moore7@gmail.com)
 * Created:   2007-10-18
 * Copyright: Joshua Moore ()
 * License:
 **************************************************************/

#ifndef SCOPETRANSFERAPP_H
#define SCOPETRANSFERAPP_H

#include <wx/app.h>


class MyApp : public wxApp
{
	public:
		virtual bool OnInit();
		virtual int OnExit();

    protected:
        wxConfigBase* config;
};



#endif // SCOPETRANSFERAPP_H
