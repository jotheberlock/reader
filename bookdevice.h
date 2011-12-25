#ifndef _BOOKDEVICE_
#define _BOOKDEVICE_

#include <stdio.h>
#include <QtCore/QIODevice>

class Mobi;

class BookDevice : public QIODevice
{
  public:

    BookDevice()
        : QIODevice()
    {
        mobi = 0;
    }

    BookDevice(Mobi * m);

    virtual qint64 	bytesAvailable () const;
    virtual bool open(OpenMode);
    virtual bool isSequential() const
    { return false; }
    virtual bool seek(qint64);
    virtual bool atEnd();
    
  protected:

    virtual qint64 readData(char * data, qint64 maxSize);
    virtual qint64 writeData(const char * data, qint64 maxSize);

    void reset_internal();
    
    qint64 size;
    Mobi * mobi;
    qint64 pos;
    qint64 offset;
    int current_block;
    QByteArray block_data;
    
};


#endif
