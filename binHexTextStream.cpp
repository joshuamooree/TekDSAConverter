#include "binHexTextStream.h"

binHexTextInputStream::binHexTextInputStream( wxInputStream& stream,
                                              int binHex )
    : wxTextInputStream( stream )
{
    this ->binHex = binHex;
}

binHexTextInputStream::~binHexTextInputStream()
{

}

void binHexTextInputStream::setMode( int binHex )
{
    this ->binHex = binHex;
}

wxUint8 binHexTextInputStream::Read8()
{
    wxUint8 value = 0;

    if( binHex == binHexTextStreamBinaryFlag )
        //binary type - no conversion necessary
        value = (wxUint8)NextChar();
    else if( binHex == binHexTextStreamHexFlag )
    {
        //hex type - conversion necessary
        wxString buffer;
        wxChar tempChar;
        long tempInt;

        //get the first digit
        tempChar = NextChar();
        while( (tempChar == '\n' || tempChar == '\r' || tempChar == '\f')&& tempChar != '\0' )
            tempChar = NextChar();
        buffer << tempChar;

        //get the next digit
        tempChar = NextChar();
        while( (tempChar == '\n' || tempChar == '\r' || tempChar == '\f')&& tempChar != '\0' )
            tempChar = NextChar();
        buffer << tempChar;

        //convert to binary
        buffer.ToLong( &tempInt, 16 );
        value = (wxUint8)tempInt;
    }
    else
        ;   //unknown type, leave the return value as the default

    return value;
}

binHexTextInputStream& binHexTextInputStream::operator>>( wxUint8& i )
{
    i = Read8();
    return *this;
}





