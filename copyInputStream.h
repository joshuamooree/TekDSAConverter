#ifndef COPYINPUTSTREAM_H
#define COPYINPUTSTREAM_H

#include <wx/stream.h>

class copyInputStream : public wxInputStream
{
    public:
        copyInputStream();
        copyInputStream( wxInputStream& readStream, wxOutputStream& writeStream );

        void setInputStream( wxInputStream& readStream );
        void setOutputStream( wxOutputStream& writeStream );

        bool CanRead();

        char GetC();

        bool Eof();

        char Peek();

        wxInputStream& Read(void* buffer, size_t buffer);
        //wxInputStream& Read(wxOutputStream& stream_out);

        off_t SeekI(off_t pos, wxSeekMode mode = wxFromStart);

        off_t TellI();

        size_t Ungetch(const char* buffer, size_t size);

        wxFileOffset GetLength() const;

        wxStreamError GetLastError() const;

        size_t GetSize() const;

        bool IsOk() const;

        bool IsSeekable() const;

    private:
        wxInputStream* readStream;
        wxOutputStream* writeStream;
};


#endif // COPYINPUTSTREAM_H
