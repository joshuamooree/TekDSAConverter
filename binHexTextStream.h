#ifndef _BINHEXTEXTSTREAM_H_
#define _BINHEXTEXTSTREAM_H_

#include "ScopeTransferApp.h"

#define binHexTextStreamBinaryFlag 0
#define binHexTextStreamHexFlag 1

class binHexTextInputStream: public wxTextInputStream
{
    public:
        binHexTextInputStream( wxInputStream& stream,
                               int binHex = binHexTextStreamHexFlag );
        ~binHexTextInputStream();
        wxUint8 Read8();

        void setMode( int binHex );

        binHexTextInputStream& operator>>( wxUint8& i );
    protected:
        int binHex;

};



#endif //_BINHEXTEXTSTREAM_H_
