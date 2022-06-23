#ifndef _MDIPICTUREFRAME_H_
#define _MDIPICTUREFRAME_H_

#include "scopeTransferApp.h"


class MDIPictureFrame: public wxMDIChildFrame
{
    public:
        MDIPictureFrame( wxBitmap picture,
                         wxMDIParentFrame* parent,
                         wxWindowID id,
                         const wxString& title);

        ~MDIPictureFrame();

        void OnPaint( wxPaintEvent& WXUNUSED( event ) );

        wxBitmap* GetBitmap();

    private:
        //MDIPictureFrameCanvas* canvas;
        wxBitmap *picture;

        DECLARE_EVENT_TABLE()


};


#endif //_MDIPICTUREFRAME_H_
