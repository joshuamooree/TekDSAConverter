/***************************************************************
 * Name:      wx_pch.h
 * Purpose:   Header to create Pre-Compiled Header (PCH)
 * Author:    Joshua Moore (joshua.moore7@gmail.com)
 * Created:   2007-10-18
 * Copyright: Joshua Moore ()
 * License:
 **************************************************************/

#ifndef WX_PCH_H_INCLUDED
#define WX_PCH_H_INCLUDED

// basic wxWidgets headers
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#ifdef WX_PRECOMP
    // put here all your rarely-changing header files
    #include <wx/file.h>
	#include <wx/regex.h>
	#include <inttypes.h>
	#include <wx/stream.h>
	#include <wx/wfstream.h>
	#include <wx/txtstrm.h>
	#include <wx/gbsizer.h>
	#include <wx/event.h>
	#include <wx/colordlg.h>
	#include <wx/progdlg.h>
	#include <wx/filename.h>
	#include <wx/image.h>
	#include <wx/notebook.h>
	#include <wx/mstream.h>
	#include <wx/html/helpctrl.h>
	#include <wx/fs_arc.h>
	#include <wx/fs_mem.h>

	//stuff for wxConfig
	#include <wx/config.h>
	#include <wx/confbase.h>
	#include <wx/fileconf.h>

	#include <wx/valgen.h>
#endif // WX_PRECOMP

#include "miscellaneousUtilities.h"

#endif // WX_PCH_H_INCLUDED
