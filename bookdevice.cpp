#include "bookdevice.h"
#include "mobi.h"
#include <stdio.h>

BookDevice::BookDevice(Mobi * m)
    : QIODevice()
{
    mobi = m;
    size = m->size();
}

bool BookDevice::open(OpenMode om)
{
    if (!mobi)
    {
        return false;
    }

    if (! (om & QIODevice::ReadOnly) )
    {
        return false;
    }

    pos = 0;
    offset = 0;
    current_block = 1;
    block_data = mobi->readBlock(1);

    QIODevice::open(om);

    qDebug("Allowing open");
    
    return true;
}

bool BookDevice::reset()
{
    QIODevice::reset();
    pos = 0;
    return true;
}

bool BookDevice::seek(qint64 pos)
{
    QIODevice::seek(pos);
    return true;
}

bool BookDevice::atEnd()
{
    qDebug("atEnd queried");
    return (current_block == mobi->numBlocks());
}

qint64 	BookDevice::bytesAvailable () const
{
    return size-pos + QIODevice::bytesAvailable();
}

qint64 BookDevice::readData(char * data, qint64 maxSize)
{
    qDebug("Read attempt %lld", maxSize);
    
    if (current_block == mobi->numBlocks())
    {
        qDebug("Bailing because of out of data");
        abort();
        return 0;
    } 

    qint64 have_read = 0;
    
    while (maxSize >= 0)
    {
        if (maxSize < (block_data.size() - offset))
        {
            char * ptr = block_data.data();
            memcpy(data, ptr+offset, maxSize);
            offset += maxSize;
            have_read += maxSize;
            pos += maxSize;
            qDebug("Read %lld bytes", have_read);
            
            return have_read;
        }
        else
        {
            char * ptr = block_data.data();

            int partial_read = (block_data.size() - offset);
            memcpy(data, ptr+offset, partial_read);
            current_block++;
            
            have_read += partial_read;
            pos += partial_read;
                
            if (current_block == mobi->numBlocks())
            {
                return have_read;
            }
            else
            {
                block_data = mobi->readBlock(current_block);
                offset = 0;
            } 
            
            data += partial_read;
            maxSize -= partial_read;
            pos += partial_read;
        }
    }

    qDebug("Bailing at end");
    return 0;
}

qint64 BookDevice::writeData(const char *, qint64)
{
    return -1;
}


