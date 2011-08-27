#include "bookdevice.h"
#include "mobi.h"
#include <stdio.h>

#define DEBUG_DEVICE

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

#ifdef DEBUG_DEVICE    
    qDebug("Allowing open");
#endif
    
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
#ifdef DEBUG_DEVICE    
    qDebug("atEnd queried");
#endif
    return (current_block == mobi->numBlocks());
}

qint64 	BookDevice::bytesAvailable () const
{
    qint64 ret = size-pos + QIODevice::bytesAvailable();
#ifdef DEBUG_DEVICE    
    qDebug("Returning %lld bytes available, size %lld pos %lld iodevice %lld",
           ret, size, pos, QIODevice::bytesAvailable());
#endif
    return (ret > 0) ? ret : 0;
}

qint64 BookDevice::readData(char * data, qint64 maxSize)
{
#ifdef DEBUG_DEVICE    
    qDebug("Read attempt %lld current block %d numblocks %d size %lld pos %lld",
           maxSize, current_block, mobi->numBlocks(), size, pos);
#endif
    
    if (current_block > mobi->numBlocks())
    {
#ifdef DEBUG_DEVICE    
        qDebug("Bailing because of out of data");
#endif
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
#ifdef DEBUG_DEVICE    
            qDebug("Read %lld bytes", have_read);
#endif
            
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

#ifdef DEBUG_DEVICE    
            qDebug("Block increment, %d %d", current_block,
                   mobi->numBlocks());
#endif
            
            if (current_block > mobi->numBlocks())
            {
#ifdef DEBUG_DEVICE    
                qDebug("Short read %lld", have_read);
#endif
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

#ifdef DEBUG_DEVICE    
    qDebug("Bailing at end");
#endif
    return 0;
}

qint64 BookDevice::writeData(const char *, qint64)
{
    return -1;
}


