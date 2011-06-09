#ifndef _BOOKDEVICE_
#define _BOOKDEVICE_

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

    BookDevice(Mobi * m)
    {
        mobi = m;
    }

    virtual bool open(OpenMode);
    virtual bool isSequential() const
    { return false; }
    virtual bool reset();
    virtual bool seek(qint64);
    
  protected:

    virtual qint64 readData(char * data, qint64 maxSize);
    virtual qint64 writeData(const char * data, qint64 maxSize);
    
    Mobi * mobi;
    qint64 pos;
    qint64 offset;
    int current_block;
    QByteArray block_data;
    
};


#endif
