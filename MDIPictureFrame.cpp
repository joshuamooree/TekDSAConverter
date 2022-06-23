#include "MDIPictureFrame.h"

MDIPictureFrame::MDIPictureFrame( wxBitmap picture,
                                  wxMDIParentFrame* parent,
                                  wxWindowID id,
                                  const wxString& title)
    : wxMDIChildFrame( parent, id, title )
{
    this -> picture = new wxBitmap ( picture );

/*
// doesn't work for some reason
    wxIcon frameIcon;
    frameIcon.CopyFromBitmap( *this ->picture );

    this ->SetIcon( frameIcon );
*/
    //important, or the frame doesn't show up.  Can't hurt anything anyway
    this -> Show();
}


MDIPictureFrame::~MDIPictureFrame()
{

}

void MDIPictureFrame::OnPaint( wxPaintEvent& WXUNUSED( event ) )
{
    //prepare the DC
    wxPaintDC dc( this );

    //setup some variables we will use to keep track of things
    int newWidth = GetClientSize().GetWidth();
    int newHeight = GetClientSize().GetHeight();

    int originalWidth = picture ->GetWidth();
    int originalHeight = picture ->GetHeight();

    static double lastRescaleFactor = 1;

    double rescaleFactor = 0;

    //now, we don't want to rescale the height and the width by different
    //amounts (otherwise we distort the picture).  The scale factor we want is
    //the smaller one (otherwise the picture might grow off the screen in one
    //direction
    if( ((double)newWidth / (double)originalWidth) < ((double)newHeight / (double)originalHeight) )
        rescaleFactor = (double)newWidth / (double)originalWidth;
    else
        rescaleFactor = (double)newHeight / (double)originalHeight;

    //got the scale factor.  Now tell the dc about it
    dc.SetUserScale( rescaleFactor, rescaleFactor );

    //draw the picture
    dc.DrawBitmap( picture ->ConvertToImage(), 0, 0, false );

    //wxLogError("Rescale factor is: %f", rescaleFactor);
    //wxLogError("Picture width is: %d, height is %d", originalWidth, originalHeight);
    //wxLogError("Window width is: %d, height is %d", newWidth, newHeight);

    //this keeps us from getting in a lovely infinite loop
    //Refresh is necessary to tell the OS to redraw the entire picture (not just
    //the changed parts).  However, it results in use being called again.  So,
    //we need to make sure we don't keep calling ourselves indirectly (makes
    //application very unusable when its locked up like that).
    if( lastRescaleFactor != rescaleFactor )
    {
        //reset the entire picture
        Refresh();

        //tell ourselves what we did last time, so we know
        //whether or not we need to call refresh again.
        lastRescaleFactor = rescaleFactor;

        //wxLogError("Resizing picture...");
    }   //end if

}   //end method OnPaint

wxBitmap* MDIPictureFrame::GetBitmap()
{
    return this ->picture;
}

BEGIN_EVENT_TABLE( MDIPictureFrame, wxMDIChildFrame )
    EVT_PAINT ( MDIPictureFrame::OnPaint )
END_EVENT_TABLE()
