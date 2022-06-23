#include "copyInputStream.h"

//m_lasterror
copyInputStream::copyInputStream()
{

}

copyInputStream::copyInputStream( wxInputStream& readStream, wxOutputStream& writeStream )
{
    this->readStream = &readStream;
    this->writeStream = &writeStream;
}

bool copyInputStream::CanRead()
{
    return this->readStream->CanRead();
}

char copyInputStream::GetC()
{
    char data = this->readStream->GetC();
    this -> writeStream->PutC(data);
    return data;
}

bool copyInputStream::Eof()
{
    return this->readStream->Eof();
}

char copyInputStream::Peek()
{
    return this->readStream->Peek();
}

wxInputStream& copyInputStream::Read(void* buffer, size_t size)
{
    this->readStream->Read(buffer, size);
    this->writeStream->Write(buffer, size);

    return *this;
}

off_t copyInputStream::SeekI(off_t pos, wxSeekMode mode)
{
    if (this->readStream->SeekI(pos, mode) == wxInvalidOffset)
        return wxInvalidOffset;
    if( this->writeStream->SeekO(pos, mode) == wxInvalidOffset)
        return wxInvalidOffset;

    return this->readStream->TellI();
}

off_t copyInputStream::TellI()
{
    return this->readStream->TellI();
}

size_t copyInputStream::Ungetch( const char* buffer, size_t size )
{
    size_t bytesUnread = this->readStream->Ungetch(buffer, size);
    this->writeStream->SeekO(-bytesUnread, wxFromCurrent);

    return bytesUnread;
}

wxFileOffset copyInputStream::GetLength() const
{
    return this->readStream->GetLength();
}

wxStreamError copyInputStream::GetLastError() const
{
    wxStreamError readErr = this->readStream->GetLastError();
    wxStreamError writeErr = this->writeStream->GetLastError();

    if( readErr != wxSTREAM_NO_ERROR )
        if( writeErr != wxSTREAM_NO_ERROR )
            if( this->m_lasterror != wxSTREAM_NO_ERROR )
                return wxSTREAM_NO_ERROR;
            else
                return this->m_lasterror;
        else
            return writeErr;
    else
        return readErr;

}

size_t copyInputStream::GetSize() const
{
    return this->readStream->GetSize();
}

bool copyInputStream::IsOk() const
{
    return this->GetLastError() == wxSTREAM_NO_ERROR;
}

bool copyInputStream::IsSeekable() const
{
    return this->readStream->IsSeekable() && this->writeStream->IsSeekable();
}
