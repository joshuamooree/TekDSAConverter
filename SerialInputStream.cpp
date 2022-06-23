#include "SerialInputStream.h"

SerialInputStream::SerialInputStream() : wxInputStream()
{
    //make a new blank serial port object
    device = new wxSerialPort();
}

SerialInputStream::SerialInputStream( const char* deviceName,
                                      wxBaud baudRate,
                                      wxParity parity,
                                      unsigned char wordLength,
                                      unsigned char stopBits,
                                      bool xonxoff )
    : wxInputStream()
{
    LogAtLevel(1, "SerialInputStream constructor: Begin");

    //make a new blank serial port object
    device = new wxSerialPort();

    //setup the various parameters
    deviceDescriptor.baud = baudRate;
    deviceDescriptor.parity = parity;
    deviceDescriptor.wordlen = wordLength;
    deviceDescriptor.stopbits = stopBits;
    deviceDescriptor.xonxoff = xonxoff;


    //try to open the serial port
    if( this ->device ->Open( deviceName, &deviceDescriptor ) < 0 )
    {
        m_lasterror = wxSTREAM_READ_ERROR;
        LogAtLevel(1, "SerialInputStream constructor: Error opening serial port.  Port: %s Baud: %i "
                      " Parity: %i Word length: %i Stop bits: %i XonXoff: %i",
                      deviceName, (int)baudRate, (int)parity, (int)wordLength, (int)stopBits, (int)xonxoff);
    }
    else
    {
        m_lasterror = wxSTREAM_NO_ERROR;
        LogAtLevel(1, "SerialInputStream constructor: Succeeded in opening serial port.  Port %s Baud: %i "
                      " Parity: %i Word length: %i Stop bits: %i XonXoff: %i",
                      deviceName, (int)baudRate, (int)parity, (int)wordLength, (int)stopBits, (int)xonxoff);
    }


}   //end constructor

SerialInputStream::~SerialInputStream()
{
    LogAtLevel(1, "SerialInputStream destructor");
    device ->Close();
    delete device;
}   //end destructor

bool SerialInputStream::isPortOpen()
{
    return device ->IsOpen();
}   //end method isPortOpen

bool SerialInputStream::closeSerialPort()
{
    if( device ->Close() == 0 )
        return true;

    m_lasterror = wxSTREAM_READ_ERROR;

    return false;

}   //end method closeSerialPort

bool SerialInputStream::openSerialPort( const char* deviceName,
                                        wxBaud baudRate,
                                        wxParity parity,
                                        unsigned char wordLength,
                                        unsigned char stopBits,
                                        bool xonxoff )
{
    //setup the various parameters
    deviceDescriptor.baud = baudRate;
    deviceDescriptor.parity = parity;
    deviceDescriptor.wordlen = wordLength;
    deviceDescriptor.stopbits = stopBits;
    deviceDescriptor.xonxoff = xonxoff;

    //try to open the serial port
    if( this ->device ->Open( deviceName, &deviceDescriptor ) == -1 )
        return false;  //error

    //ok, so now the serial port should be open and setup properly
    return true;

}   //end method openSerialPort

bool SerialInputStream::changeBaudRate( wxBaud baudRate )
{
    //try to change the baud rate
    if( device ->SetBaudRate( baudRate ) != 0 )
        return false;   //error

    //update our records
    this ->deviceDescriptor.baud = baudRate;

    //baud rate changed successfully
    return true;

}   //end method changeBaudRate

bool SerialInputStream::isByteWaiting()
{
    char characterToRead;

    //is the stream even OK before we start?
    if( IsOk() )
    {
        //ok, now lets try reading a byte
        int bytesRead = device ->Read( &characterToRead, sizeof(characterToRead) );

        //what happened?
        if( bytesRead == -1 )
        {
            //error
            m_lasterror = wxSTREAM_READ_ERROR;
            return false;
        }
        else if( bytesRead == 0 )
        {
            //nothing read.  Everything is ok, just nothing was read
            m_lasterror = wxSTREAM_NO_ERROR;
            return false;
        }
        else if( bytesRead > 0 )
        {
            //we read something.
            //everything is ok
            m_lasterror = wxSTREAM_NO_ERROR;
            //put the char back since this function just test
            this ->Ungetch( characterToRead );
            //tell the caller about it
            return true;
        }
    }   //end outer if
    else
        //error in the stream, so just return false
        return false;

    //catch all - mostly gets rid of compiler warnings
    return false;

}   //end method isByteWaiting

/*
 * Required to make a wxInputStream into a SerialInputStream.  This is where the
 * real work gets done.
 */
size_t SerialInputStream::OnSysRead( void* buffer, size_t size )
{
    int logLevel;
    wxConfigBase::Get() ->Read("/Log/Level", &logLevel, 0 );
    static wxFileOutputStream* dataLogStream = NULL;

    if(logLevel >= 2 && dataLogStream == NULL)
    {
        dataLogStream = new wxFileOutputStream("dataLog.txt");
    }

    int bytesRead = 0;

    //dimensional analysis:
    //  bits / word
    // ------------- milliseconds / second = milliseconds / word
    // bits / second
    double timeoutTime = ((double)deviceDescriptor.wordlen * 1000.0)/
                           (double)deviceDescriptor.baud;

    //timeoutTime should, however, be for more than 1 word.  Lets make it for 100
    timeoutTime *= 10000;

    //let's set up a timer so we can see if there are more bytes or if this is
    //"end of the file"
    wxStopWatch time;

    //try getting something.  And keep trying until we get something, an error
    //occurs, or it becomes obvious that nothing is coming

    do
    {
        bytesRead = device ->Read( (char*)buffer, size );

    } while( bytesRead == 0 &&  //loop while we haven't received anything
             bytesRead != -1 && //break if an error is encountered
             time.Time() < timeoutTime ); //break if timeout

    //set the error code according to what happened
    if( bytesRead == -1 )
    {
        //something bad happened.
        m_lasterror = wxSTREAM_READ_ERROR;
        return 0;
    }
    else if( time.Time() >= timeoutTime )
    {
        //in this case, we say read error if no data for a long time
        m_lasterror = wxSTREAM_READ_ERROR;
    }
    else
    {
        //no error as far as we are concerned
        m_lasterror = wxSTREAM_NO_ERROR;

        if(logLevel >= 2)
            dataLogStream ->Write(buffer, bytesRead);
    }

    //return how much was written to the output
    return bytesRead;

}   //end method OnSysRead

wxFileOffset SerialInputStream::SeekI(wxFileOffset pos, wxSeekMode mode)
{
    //do we actually do anything here?  There is not really any stream
    //to seek (unless we remember what we got - yuck!)
    return -1;
}

wxFileOffset SerialInputStream::TellI() const
{
    return -1;
}

