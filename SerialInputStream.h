#ifndef _SERIALINPUTSTREAM_H_
#define _SERIALINPUTSTREAM_H_

#include "ScopeTransferApp.h"
#include "wx/ctb/iobase.h"
#include "wx/ctb/serport.h"
#ifdef __WINDOWS__
    #include "wx/ctb/win32/serport.h"
#else
    #include "wx/ctb/linux/serport.h"
#endif

#define SerialInputStreamDataBufferSize 256

class SerialInputStream : public wxInputStream
{
    public:
        /**
         * Default constructor
         */
        SerialInputStream();

        /**
         * \param deviceName a string (defined in serport.h) describing the
         * com port
         * \param baudRate a value (defined in serportx.h) describing the baud
         * rate to use
         * \param wordLength how many bits is each word?  Usually 7 or 8
         * \param stopBits how many stop bits to use.  Most often 0
         * \param xonxoff use xonxoff flow control.  Usually false
         * Constructor
         */
        SerialInputStream( const wxChar* deviceName,
                           wxBaud baudRate,
                           wxParity parity = wxPARITY_NONE,
                           unsigned char wordLength = 8,
                           unsigned char stopBits = 0,
                           bool xonxoff = false );
        /**
         * Descructor
         */
        ~SerialInputStream();

        /**
         * \param deviceName a string (defined in serport.h) describing the
         * com port
         * \param baudRate a value (defined in serportx.h) describing the baud
         * rate to use
         * \param wordLength how many bits is each word?  Usually 7 or 8
         * \param stopBits how many stop bits to use.  Most often 0
         * \param xonxoff use xonxoff flow control.  Usually false
         * \return true if the port is successfully opened, false otherwise
         * Opens a serial port with the specified parameters.
         * Useful for two step contuction or when the serial port had previously
         * been closed
         */
        bool openSerialPort( const char* deviceName,
                             wxBaud baudRate,
                             wxParity parity = wxPARITY_NONE,
                             unsigned char wordLength = 8,
                             unsigned char stopBits = 0,
                             bool xonxoff = false  );

        /**
         * \param baudRate a value (defined in serportx.h) describing the baud
         * rate to use
         * \return true if the set succeeds, false otherwise
         * Sets the baud rate.  Serial port must already be open
         */
        bool changeBaudRate( wxBaud baudRate );

        /**
         * \return true if the port is successfully closed, false otherwise
         * Close the serial port
         */
        bool closeSerialPort();

        /**
         * \return true if the serial port is open, false otherwise
         * Checks whether or not the serial port is open
         */
        bool isPortOpen();

        /**
         * \return true if there is at least 1 byte waiting in the buffer
         * A quick test to see if there are bytes available
         */
         bool isByteWaiting();

    protected:
        size_t OnSysRead( void *buffer, size_t size );

        wxFileOffset SeekI(wxFileOffset pos, wxSeekMode mode);
        wxFileOffset TellI() const;

        wxSerialPort* device;

        wxSerialPort_DCS deviceDescriptor;
};



#endif //_SERIALINPUTSTREAM_H_
