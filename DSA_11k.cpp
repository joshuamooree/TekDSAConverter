#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include "DSA_11k.h"


DSA_11k::DSA_11k()
{
    width = 0;
    height = 0;
    header = _T("");

    bitmap.Create( 1, 1, -1 );

    //default values for the color map - may be
    //changed later by the config system
    colorMap[0].red = 255; colorMap[0].green = 255; colorMap[0].blue = 255;

    colorMap[1].red = 150; colorMap[1].green = 2; colorMap[1].blue = 140;

    colorMap[2].red = 0;   colorMap[2].green = 255; colorMap[2].blue = 0;

    colorMap[3].red = 255; colorMap[3].green = 255; colorMap[3].blue = 0;

    colorMap[4].red = 0;   colorMap[4].green = 0;   colorMap[4].blue = 255;

    colorMap[5].red = 70;  colorMap[5].green = 210; colorMap[5].blue = 160;

    colorMap[6].red = 90;  colorMap[6].green = 100; colorMap[6].blue = 180;

    colorMap[7].red = 255; colorMap[7].green = 255; colorMap[7].blue = 255;

    //lets see what the config system has to say
    doConfig();

    //initialize the color button ids
    colorButtonIDs[0] = 100;
    colorButtonIDs[1] = 101;
    colorButtonIDs[2] = 102;
    colorButtonIDs[3] = 103;
    colorButtonIDs[4] = 104;
    colorButtonIDs[5] = 105;
    colorButtonIDs[6] = 106;
    colorButtonIDs[7] = 107;
}

void DSA_11k::doConfig()
{
    //get the config object
    wxConfigBase* config = wxConfigBase::Get();

    //save the existing path to be nice
    wxString previousPath = config ->GetPath();

    //we are a type of scope and the DSA/11k series
    config ->SetPath( _T("/Scopes/DSA_11k/") );

    //a place to store the path
    wxString key;

    for( int colorMapNumber = 0; colorMapNumber < colorMapSize; colorMapNumber++ )
    {
        //red map entries
        key = wxString(_T("colorMap")) << colorMapNumber << _T("red");

        colorMap[colorMapNumber].red =
                            config -> Read( key, colorMap[colorMapNumber].red );

        //green map entries
        key = wxString(_T("colorMap")) << colorMapNumber << _T("green");

        colorMap[colorMapNumber].green =
                          config -> Read( key, colorMap[colorMapNumber].green );

        //blue map entries
        key = wxString(_T("colorMap")) << colorMapNumber << _T("blue");

        colorMap[colorMapNumber].blue =
                           config -> Read( key, colorMap[colorMapNumber].blue );
    }   //end for loop

    //now config stuff for the formats
    config -> Read( _T("autodetectBinary"), &this ->autodetectBinary, true );
    config -> Read( _T("autodetectCompressed"), &this ->autodetectCompressed, true );
    config -> Read( _T("binaryData"), &this ->binary, true );
    config -> Read( _T("compressedData"), &this ->compacted, true );

    //restore the path
    config ->SetPath( previousPath );
}   //end method doConfig

DSA_11k::~DSA_11k()
{
    for( int pos = 0; pos < colorMapSize; pos++ )
    {
        if( colorPreviews[pos] != NULL )
            delete colorPreviews[pos];
    }
}

wxBitmap DSA_11k::getBitmap()
{
    return bitmap;
}

wxString DSA_11k::getHeader()
{
    return this ->header;
}

long DSA_11k::getPictureWidth()
{
    return this ->width;
}

long DSA_11k::getPictureHeight()
{
    return this ->height;
}


bool DSA_11k::processHeader( wxInputStream* data )
{
    #define headerBufferSize 1000
    #define searchLength 200
    //assume we don't succeed
    wxString headerData ( (wxChar)' ', headerBufferSize );
    bool returnValue = false;

    //first, the regex to detect the format of the header
    wxRegEx findHeader (_T("([\\w[:blank:]:-]+date[\\w[:blank:]:-]+)[[:cntrl:]]+"
                           "(\\d+)[[:cntrl:]]+"
                           "(\\d+)[[:cntrl:]]+"),
                           wxRE_ADVANCED );

    //we should probably check the regex.
    //an error here would be very surprising!
    if( !findHeader.IsValid() )
    {
        wxLogError(   _("Error in regex in DSA_11k.cpp"
                        "How did that happen?!"
                        "Program may be corrupt.") );
        return false;
    }

    //ok, start looking for the header data
    int count;
    for( count = 0; !findHeader.Matches( headerData ) &&
                    count < searchLength &&
                    data->IsOk();
                                  count++ )
        headerData.SetChar( count, data -> GetC() );

    //well, did we find it?  is the stream still OK?
    //if so, we have our stream!  If not, we have an error
    if ( count < searchLength && data ->IsOk() )
    {
        this ->header = findHeader.GetMatch( headerData, 1 );
        findHeader.GetMatch( headerData, 2 ).ToLong( &width );
        findHeader.GetMatch( headerData, 3 ).ToLong( &height );

        returnValue = true;
    }
    else
        returnValue = false;

    //maked a nice new empty bitmap for the picture to go into
    bitmap.Create( width, height, -1 );

    return returnValue;
}


/**
 *
 *
 *
 */

bool DSA_11k::processData( wxInputStream* data,  bool (*progressCallback)(int precent) )
{
    #define bufferSize 20

    //a place to put our temporary data for autodetection
    char buffer [bufferSize];

    //if we are going to autodetect anything, first we need data
    if( autodetectBinary || autodetectCompressed )
    {
        //now to figure out just what kind of file this is.

        //get the data - all 20 bytes worth
        for( int pos = 0; pos < bufferSize && data ->IsOk(); pos++ )
            buffer[pos] = data -> GetC();

        //a little error checking - no sense in continuing if the stream already
        //has problems
        if( ! data ->IsOk() )
        {
            wxLogError( _("Data error: stream ended early. Data may be corrupt. \n"
                        "Canceling conversion.") );
            return false;
        }

        //ok, now put those characters back - they will be needed later!
        data ->Ungetch( buffer, bufferSize );
    }   //end if for autodetectBinary and autodetectCompressed

    //should we autodetect binary?
    if( autodetectBinary )
    {
        bool foundBinaryCharacters = false;
        //first, is this a binary or a binhex file?
        //binary files have all kinds of values.  binhex has only ascii 0-9, a-b
        //and newlines.
        //So, lets do our test by seeing if any are not regular letters or
        //newlines.  If there are, this must be a binary file.  Otherwise, it
        //must be a binhex
        for( int pos = 0; pos < bufferSize; pos++ )
            //if the character is not a letter, number, or a newline, the data must
            //be binary
            if( (buffer[pos] < '0' ||
                    ( buffer[pos] > '9' && buffer[pos] < 'A' ) ||
                    ( buffer[pos] > 'F' && buffer[pos] < 'a' ) ||
                    buffer[pos] > 'f') &&
                    //buffer[pos] != '\0' ||  //should null be excluded?
                    buffer[pos] != '\n' &&
                    buffer[pos] != '\f' &&
                    buffer[pos] != '\r')
            {
                foundBinaryCharacters = true;
            }

        //ok, so is it binary?  If so, note that
        if( foundBinaryCharacters )
            binary = true;
        else
            binary = false;
    }   //end if for autodetectBinary


    //should we autodetect compressed?
    if( autodetectCompressed )
    {
        //next, we need to figure out whether it is compressed or not.  Since
        //the compression is a run length encoding, it reduces repeated
        //symbols.  So, lets count the repeated symbols and find how often
        //the most repeated one is repeated.
        //an alternative implementation for this could be an entropy measurement
        //that is, count how many different numbers (characters) are in the
        //data sample - the uncompressed has lots of the same

        //we already got the data above, so we'll work with that.
        int highestCharacterCount = 0;
        int currentCharacterCount = 0;

        //loop through all the characters and then count how many times each one
        //occurs in the array.
        for( char currentCharacter = '\0'; currentCharacter < 127; currentCharacter++ )
        {
            //reset the character count
            currentCharacterCount = 0;
            //loop through the array and see how many times this character occurs
            for( int pos = 0; pos < bufferSize; pos++ )
                //is the character here?
                if( buffer[pos] == currentCharacter )
                    //yes, increment the count
                    currentCharacterCount++;

            //was that a record count?
            if( currentCharacterCount > highestCharacterCount )
                highestCharacterCount = currentCharacterCount;
        }   //end for

        //ok, so now a bit of magic, statictics, and testing.
        //if the highest character count is higher than a preset threshold,
        //this must be uncompressed.  Otherwise, it must be uncompressed

        //since binary and binhex look different, the thresholds are different
        //for each
        if( binary )
            //data is binary
            if( highestCharacterCount < 7 )
                compacted = true;
            else
                compacted = false;
        else
            //data is binhex
            if( highestCharacterCount < 10 )
                compacted = true;
            else
                compacted = false;
    }   //end if for autodetectCompressed


    //now, lets let the user know about the data type we are using
    wxString outputMessage ( _("Raw data is ") );
    if( binary )
        outputMessage << _("binary ");
    else
        outputMessage << _("binHex ");

    if( compacted )
        outputMessage << _("compacted");
    else
        outputMessage << _("uncompacted");

    //output it to the status bar
    wxLogStatus( outputMessage );

    //finally, process the data.  Of course which function we use depends on
    //which whether the data is compressed or not

    //process according to whether it is compacted or not
    if( compacted )
        return processCompressed( data, progressCallback );
    else
        return processUncompressed( data, progressCallback );

    //how exactly do we get here?  Must be an error - a very strange one
    return false;
}   //end method process data

 /**
  * The 11k series stores its screen data in 3 bit "pixels".  The mapping to
  * RGB or some other color scheme is not contained in the data from the scope
  * - the pixels actually only tell us the source.
  *
  * For the compressed format, after converting from binhex/binary, it starts
  * with 2 pixels packed into 1 byte.  So the bits are
  * bbbaaa
  * That leaves 2 bits left from the first byte.  These bytes and possibly the
  * next 16 describe how many times the first two "pixels" should be in this
  * sequence of pixels.  The actual number of pixels represented by this
  * sequence is 2*r, where r is the value described below.
  *
  * If there are no repeats, the last two digits of the first byte will be 01.
  * So, for no repeats, the first byte will be 01bbbaaa, or 2*1 = 2 pixels (the
  * first 2 transmitted, no repeats)
  *
  * The remaining bits can command up to 2*3 = 6 repeats.  If these last two
  * digits are both 0, the repeats are described by the next bytes.
  *
  * So, to get 2*4 = 8 repeats, up to 2*(2^8 - 1) = 2*255 = 510 repeats, the
  * next byte is used and the remaining digits for the previous byte are left at
  * 0.  So, for this case, the sequence looks like:
  *      00bbbaaa rrrrrrrr
  *
  * Since the lowest value this second byte would represent is 4, if the value
  * of the previous byte is less than 4, this byte must be important.  So for
  * 2*256 = 512 up to 2*(2^10 - 1) = 2*1023 = 2047 repeats, the sequence looks like:
  *      00bbbaaa 000000rr rrrrrrrr
  *
  * It is important to remember that the number obtained from these bits is not
  * the actual number of repeats.  2 times this number is the number of pixels
  * represented by this 1 - 3 byte sequence.  Also, as a consequence, the number
  * of pixels represented by a sequence will always be a multiple of 2.
  *
  * | value from byte
  * |    1    |    2   |   3   |
  * | pixel number
  * |  1   2     3   4   5   6
  * |
  * | aaa bbb | aaa bbb aaa bbb ...
  *           |
  *  from 1st | repeats described -->
  *   part    |   by second part
  */
bool DSA_11k::processCompressed( wxInputStream* data, bool (*progressCallback)(int precent) )
{

    //*********************************//
    //begin setup
    //*********************************//

    //where to put the picture once ceonversion is done
    wxImage convertedPicture( (int)width, (int)height, false );
    unsigned char *rawData = convertedPicture.GetData();
    //unsigned char rawData [height][width*3];

    //setup the stream that does our conversion
    binHexTextInputStream convData( *data );

    //now setup the mode for that stream
    if( binary )
        convData.setMode( binHexTextStreamBinaryFlag );
    else
        convData.setMode( binHexTextStreamHexFlag );

    uint16_t currentByte;

    //*********************************//
    //begin algorithim
    //*********************************//

    currentByte = convData.Read8();

    int row = 0, column = 0;
    for( row = 0; (row < height) && ( data->IsOk() ); row++ )
    {
        //we do interate through the columns.  However, increasing column is done
        //in the body of the loop since each pass generates at least 2 entries
        for( column = 0; (column < width) && ( data->IsOk() ); )
        {
            //error checking
            if( (currentByte & 0x07) < colorMapSize )
            {
                //ok, first the first pixel that is packed into the byte
                rawData[ row*width*3 + column*3 ] = colorMap[ currentByte & 0x07 ].red;
                rawData[ row*width*3 + column*3 + 1 ] = colorMap[ currentByte & 0x07 ].green;
                rawData[ row*width*3 + column*3 + 2 ] = colorMap[ currentByte & 0x07 ].blue;

                column++;
                //if( row == 0 && column == 1 )
                    //wxLogError( "Processed first pixel.  Data is: %d", currentByte & 0x07  );
            }
            else
                //error.  Yes, I know gotos are evil, but we are not going far
                goto incorrectDataError;

            //now that we have recorded the first pixel, shift the bits out
            currentByte = currentByte >> 3;

            //error checking
            if( (currentByte & 0x07) < colorMapSize )
            {
                //ok, now the second pixel that is packed into the byte
                rawData[ row*width*3 + column*3 ] = colorMap[ currentByte & 0x07 ].red;
                rawData[ row*width*3 + column*3 + 1 ] = colorMap[ currentByte & 0x07 ].green;
                rawData[ row*width*3 + column*3 + 2 ] = colorMap[ currentByte & 0x07 ].blue;

                column++;
            }
            else
                //error.  Yes, I know gotos are evil, but we are not going far
                goto incorrectDataError;

            //discard the bits for this pixel
            currentByte = currentByte >> 3;

            //now we have taken care of the first 2 pixels in the byte.  Next,
            //we need to see about repeats
            if( currentByte == 0 )
            {
                //ah, so there are repeats!

                currentByte = convData.Read8();

                if( !data -> IsOk() )
                    //error: end of stream encoutered early  Yes, I know gotos
                    //are evil, but we are not going far
                    goto earlyEndOfStream;

                //are there more repeats than what we initially think?
                if( currentByte < 4 )
                {
                    //there are!
                    //first, shift the bits we do have over so we can accomodate
                    //the new ones
                    currentByte = currentByte << 8;
                    currentByte |= convData.Read8();

                    if( !data ->IsOk() )
                        //error: end of stream encoutered early  Yes, I know
                        //gotos are evil, but we are not going far
                        goto earlyEndOfStream;

                }   //end of if for getting additional repeats
                //ok, now we know hom many times to repeat, lets repeat

            }   //end of if for repeats

                //note on the initialization: the number of times to repeat is
                //2 less than what currentbyte has in it
                for( int repeat = (currentByte*2 - 2); repeat > 0 && column < width; repeat--, column++ )
                {
                    //note on the indixes:
                    //this is rawData:
                    // r1g1b1 r2g2b2 (spaces have no significance)
                    //we want to repeat the first one first, the second one second,
                    //the first one third, etc
                    //so, rawData should look like
                    // r1g1b1 r2g2b2 r1g1b1 r2g2b2 ...

                    //copy over the red
                    rawData[ row*width*3 + column*3 ] = rawData[ row*width*3 + column*3 - 6 ];
                    //copy over the green
                    rawData[ row*width*3 + column*3 + 1 ] = rawData[ row*width*3 + column*3 - 6 + 1 ];
                    //copy over the blue
                    rawData[ row*width*3 + column*3 + 2 ] = rawData[ row*width*3 + column*3 - 6 + 2 ];

                }   //end of for loop for repeats

            //ok, now we finished processing that bit of data, now move onto
            //the next byte
            currentByte = convData.Read8();

        }   //end inner for

        //update the progress.
        if( progressCallback != NULL )
            if ( !progressCallback( (row * 100) / height ) )
                return false; //abort if the user clicked abort

    }   //end outer for

    //some more error checking
    if( row < height || column < width )
        //error: end of stream encoutered early  Yes, I know gotos
        //are evil, but we are not going far
        goto earlyEndOfStream;

    //set the progress to 100 since we are finished
    if( progressCallback != NULL )
            progressCallback( 100 );

    //create the actual bitmap
    bitmap = wxBitmap( convertedPicture );

    LogAtLevel(1, "DSA_11k.processCompressed: Data good (or so it seems).  Done, exiting.");

    //the only time we get here is if all went well
    return true;

    //error stuff - the target of those gotos above
    incorrectDataError:
        wxLogError(   _("Data error in DSA_11k.cpp::processCompressed: \n"
                        "Received pixel with value greater "
                        "than size of colormap. \n"
                        "Either the data mode is not correctly set or \n"
                        "the scope is not supported \n"
                        "Canceling conversion.") );

        //set the progress to 100 since we are finished
        if( progressCallback != NULL )
            progressCallback( 100 );

        LogAtLevel(1, "DSA_11k.processCompressed: Data error.  Exiting.");
        return false;

    earlyEndOfStream:
        wxLogError(   _("Error in DSA_11k.cpp::processCompressed: \n"
                        "End of stream encountered early \n"
                        "This is probably either a programming error, \n"
                        "an unsupported scope, a corrupted file, \n"
                        "or a canceled transfer "
                        "Canceling conversion.") );

        //set the progress to 100 since we are finished
        if( progressCallback != NULL )
            progressCallback( 100 );

        LogAtLevel(1, "DSA_11k.processCompressed: Early end of stream.  Exiting.");
        return false;


}   //end method processCompressed

bool DSA_11k::processUncompressed( wxInputStream* data, bool (*progressCallback)(int precent) )
{
    //*********************************//
    //begin setup
    //*********************************//

    //where to put the picture once ceonversion is done
    wxImage convertedPicture( (int)width, (int)height, false );
    unsigned char *rawData = convertedPicture.GetData();


    //setup the stream that does our conversion
    binHexTextInputStream convData( *data );

    //now setup the mode for that stream
    if( binary )
        convData.setMode( binHexTextStreamBinaryFlag );
    else
        convData.setMode( binHexTextStreamHexFlag );

    uint8_t currentByte;

    //*********************************//
    //begin algorithim
    //*********************************//

    //get the first byte
    currentByte = convData.Read8();

    for( int row = 0; (row < height) && ( data->IsOk() ); row++ )
    {
        for( int column = 0; column < width && ( data->IsOk() ); column++ )
        {
            if( currentByte < colorMapSize )
            {
                rawData[ row*width*3 + column*3 ] = colorMap[currentByte].red;
                rawData[ row*width*3 + column*3 + 1 ] = colorMap[currentByte].green;
                rawData[ row*width*3 + column*3 + 2 ] = colorMap[ currentByte ].blue;
            }
            else
            {
                wxLogError(   _("Data error in DSA_11k.cpp::processUncompressed: \n"
                                "Received pixel with value greater "
                                "than size of colormap. \n"
                                "Either the data mode is not correctly set or \n"
                                "the scope is not supported \n"
                                "Canceling conversion.") );

                //dismiss the progress dialog
                progressCallback( 100 );

                //break out of the loops with a failure - the data is
                //not at all what it should be, so we cannot continue
                return false;
            }

            //get the next byte
            currentByte = convData.Read8();
        }   //end inner for

        //update the progress
        if( progressCallback != NULL )
            if( !progressCallback( (row * 100) / height ) )
                return false;
    }   //end outer for

    //get rid of the progress dialog since we must be done
    progressCallback( 100 );

    //create the actual bitmap
    bitmap = wxBitmap( convertedPicture );

    return true;
}   //end method processUncompressed

/////////////////////GUI stuff/////////////////////
wxPanel* DSA_11k::getConfigPage( wxWindow* parent )
{
    wxMemoryDC dc;
    wxBitmap previewBitmaps [ colorMapSize ];

    wxStaticText* labels[colorMapSize];
    wxPanel* configPage = new wxPanel( parent );

    //make an array for our strings
    wxArrayString labelText;
    labelText.Alloc( colorMapSize );
    labelText.Add( _("Background color") );
    labelText.Add( _("Waveform 1, selectable text, and icons color") );
    labelText.Add( _("Waveform 2 color") );
    labelText.Add( _("Waveform 3 color") );
    labelText.Add( _("Waveform 4 color") );
    labelText.Add( _("Window Waveforms and TEK logo color") );
    labelText.Add( _("Graticule, unselected text, and icon background color") );
    labelText.Add( _("Cursor and measurement zone color") );

    //make a top level sizer and set it up
    wxFlexGridSizer* topSizer = new wxFlexGridSizer( 1 );
    configPage ->SetSizer( topSizer );

    //now make a sizer for the color butons and add it
    wxFlexGridSizer* colorSizer = new wxFlexGridSizer( colorMapSize, 2, 5, 5 );
    //configPage ->SetSizer( colorSizer );
    topSizer ->Add( colorSizer );

    //now, lets loop through the arrays and and make our controls
    for( int currentColor = 0; currentColor < colorMapSize; currentColor++ )
    {
        //add the labels
        labels[currentColor] = new wxStaticText( configPage,
                                                -1,
                                                labelText[ currentColor ] );
        colorSizer ->Add( labels[currentColor], wxALIGN_RIGHT );

        //add the bitmap buttons
        //make the bitmap (they are all 20 x 20)
        previewBitmaps[currentColor].Create( 20, 20 );
        //select the bitmap into the dc
        dc.SelectObject( previewBitmaps[currentColor] );
        //make the brush the same color as the corresponding entry into the
        //color map
        dc.SetBrush( wxBrush( wxColour( colorMap[currentColor].red,
                                        colorMap[currentColor].green,
                                        colorMap[currentColor].blue ) ) );
        //fill the bitmap
        dc.DrawRectangle( 0, 0, 20, 20 );

        //put the bitmap into a button
        colorPreviews[currentColor] = new wxBitmapButton( configPage,
                                                 colorButtonIDs[currentColor],
                                                 previewBitmaps[currentColor] );
        //add the button to the panel
        colorSizer ->Add( colorPreviews[currentColor] );

        //a bit of magic here - dynamic events, ooh scary
        Connect( colorButtonIDs[ currentColor ],
                 wxEVT_COMMAND_BUTTON_CLICKED,
                 wxCommandEventHandler(DSA_11k::onSelectColor) );

    }   //end for

    //now controls for the binary, compressed, and autodetects
    wxStaticBoxSizer* binarySizer = new wxStaticBoxSizer( wxHORIZONTAL,
                                                          configPage,
                                                          _("Binary") );
    topSizer ->Add( binarySizer );

    wxCheckBox* binaryOrNot = new wxCheckBox( configPage,
                                              -1,
                                              _("Binary format"));
    binaryOrNot ->SetValidator( wxGenericValidator(&this ->binary) );
    binarySizer ->Add(binaryOrNot);

    wxCheckBox* autodetectBinaryOrNot = new wxCheckBox( configPage,
                                                        -1,
                                                        _("Autodetect binary format"));
    autodetectBinaryOrNot ->SetValidator( wxGenericValidator(&this ->autodetectBinary) );
    binarySizer ->Add(autodetectBinaryOrNot);

    wxStaticBoxSizer* compressedSizer = new wxStaticBoxSizer( wxHORIZONTAL,
                                                          configPage,
                                                          _("Compressed") );
    topSizer ->Add( compressedSizer );

    wxCheckBox* compressedOrNot = new wxCheckBox( configPage,
                                                  -1,
                                                  _("Compressed data"));
    compressedOrNot ->SetValidator( wxGenericValidator(&this ->compacted) );
    compressedSizer ->Add(compressedOrNot);

    wxCheckBox* autodetectCompressedOrNot = new wxCheckBox( configPage,
                                                            -1,
                                                            _("Autodetect compressed data"));
    autodetectCompressedOrNot ->SetValidator( wxGenericValidator(&this ->autodetectCompressed) );
    compressedSizer ->Add(autodetectCompressedOrNot);

    //transfer data to the controls by way of the validators
    configPage ->TransferDataToWindow();

    //give the page
    return configPage;
}

void DSA_11k::destroyConfigPage( wxPanel* page )
{
    //first activate the validators and transfer data from the controls
    page ->TransferDataFromWindow();

    //ok, we need to save all the variables that go in the config
    wxConfigBase* config = wxConfigBase::Get();

    //save the existing path to be nice
    wxString previousPath = config ->GetPath();

    wxString key;

    //we are a type of scope and the DSA/11k series
    config ->SetPath( _T("//Scopes//DSA_11k//") );

    for( int colorMapIndex = 0; colorMapIndex < colorMapSize; colorMapIndex++ )
    {
        key = wxString(_T("colorMap")) << colorMapIndex << _T("red");
        config ->Write( key, (long)colorMap[colorMapIndex].red );

        key = wxString(_T("colorMap")) << colorMapIndex << _T("green");
        config ->Write( key, (long)colorMap[colorMapIndex].green );

        key = wxString(_T("colorMap")) << colorMapIndex << _T("blue");
        config ->Write( key, (long)colorMap[colorMapIndex].blue );
    }

    //config stuff for the data formats and detection
    config ->Write( _T("autodetectBinary"), this ->autodetectBinary );
    config ->Write( _T("autodetectCompressed"), this ->autodetectCompressed );
    config ->Write( _T("binaryData"), this ->binary );
    config ->Write( _T("compressedData"), this ->compacted );

    //restore the path
    config ->SetPath( previousPath );

}

void DSA_11k::onSelectColor( wxCommandEvent& event )
{
    //no need to see if we should process this
    //event - the event handler makes sure we don't get ones that
    //aren't ours

    //find the index into the color map.  This assumes
    //we got the right ID, that the IDs for the color preview
    //buttons are sequential, and that the lowest id number is
    //the first one in the array
    int colorMapIndex = event.GetId() - colorButtonIDs[0];

    //do a little checking (wouldn't want overflows now would we)
    if( colorMapIndex >= 0 && colorMapIndex < colorMapSize )
    {
        //get the color from the user
        wxColour userColor = wxGetColourFromUser( NULL,
                                    wxColour( colorMap[colorMapIndex].red,
                                              colorMap[colorMapIndex].green,
                                              colorMap[colorMapIndex].blue) );

        //see if the user clicked yes or cancel
        if( userColor.Ok() )
        {
            //ok, we should have a valid color here
            //make the new bitmap
            wxBitmap surface (20, 20);
            wxMemoryDC dc;
            dc.SelectObject( surface );
            dc.SetBrush( userColor );
            dc.DrawRectangle( 0, 0, 20, 20 );

            //put it into the button
            colorPreviews[colorMapIndex] -> SetBitmapLabel( surface );

            //put it into the color map
            colorMap[colorMapIndex].red = userColor.Red();
            colorMap[colorMapIndex].green = userColor.Green();
            colorMap[colorMapIndex].blue = userColor.Blue();
        }   //end inner if
    }   //end outer if
}   //end method onSelectColor
