#ifndef _DSA_11K_H_
#define _DSA_11K_H_


#include "ScopeTransferApp.h"
#include "binHexTextStream.h"

#define colorMapSize 8

class DSA_11k : public wxEvtHandler
{
    public:
        DSA_11k();
        ~DSA_11k();

        bool processHeader( wxInputStream* data );

        //if the value for progressCallback is null, it will not be called
        bool processData( wxInputStream* data, bool (*progressCallback)(int precent) );

        wxPanel* getConfigPage( wxWindow* parent );
        void destroyConfigPage( wxPanel* page );

        wxBitmap getBitmap();

        wxString getHeader();

        long getPictureWidth();
        long getPictureHeight();

        void onSelectColor( wxCommandEvent& event );

    protected:
        bool processCompressed( wxInputStream* data, bool (*progressCallback)(int precent) );
        bool processUncompressed( wxInputStream* data, bool (*progressCallback)(int precent) );

        void doConfig();

        long width;
        long height;
        wxString header;

        wxBitmap bitmap;

        bool binary;
        bool compacted;
        bool autodetectCompressed;
        bool autodetectBinary;

        typedef struct
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        } colorStructure;

        colorStructure colorMap[colorMapSize];

        /* from filter.c (the example program that inspired this project and
           documented the format for the 11k and DSA), the indexes from the
           scope are:

           0 - background
           1 - Waveform color 1, selectable text and icons
           2 - Waveform color 2
           3 - waveform color 3
           4 - waveform color 4
           5 - window waveforms, tek logo
           6 - Graticules, unselected text and icon backgrounds
           7 - cursors, measurement zones
        */

        wxBitmapButton* colorPreviews [colorMapSize];

        int colorButtonIDs [colorMapSize];


};



#endif //_DSA_11K_H_
