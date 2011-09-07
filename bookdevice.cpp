#include "bookdevice.h"
#include "mobi.h"
#include <stdio.h>

// #define DEBUG_DEVICE

// This class subclasses QIODevice, providing a similar interface
// to classes such as QFile and QBuffer for the book text of the Mobipocket
// file (which is typically compressed). It's read-only; providing write
// access to the data is both unnecessary and complicated.

// A tradeoff is that while the device gives the appearance of full random
// access, seek()ing actually throws away all current state and re-reads
// the file from the beginning. It would be possible to make this more
// efficient by indexing each Mobi section with its position within the
// uncompressed stream, but this would require reading through the entire
// book at startup. Since seek()ing is a rare event, I thought it better to
// skip the startup delay; it should only impact performance when going to
// the previous page. It would also be possible to optimise seeking forward
// from the current position, but currently there is no need.

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

    reset_internal();
    
    QIODevice::open(om);

#ifdef DEBUG_DEVICE    
    qDebug("Allowing open");
#endif
    
    return true;
}

// Mobipocket's usual compression format is documented here -
// http://wiki.mobileread.com/wiki/PalmDOC#Reading_PalmDOC
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

bool BookDevice::seek(qint64 seekTo)
{
    QIODevice::seek(seekTo);
    reset_internal();

        // To seek we re-read from the beginning - it's not a common
        // operation and to do otherwise would require indexing the book
        // on startup.
        // This is a modified form of readData and should probably be
        // refactored into a single internal method

    qint64 have_read = 0;
    
    while (seekTo >= 0)
    {
        if (seekTo < (block_data.size() - offset))
        {
            offset += seekTo;
            have_read += seekTo;
            pos += seekTo;
#ifdef DEBUG_DEVICE    
            qDebug("Seek %lld bytes", have_read);
#endif
            
            return true;
        }
        else
        {
            int partial_read = (block_data.size() - offset);
            current_block++;
            
            have_read += partial_read;
            pos += partial_read;

#ifdef DEBUG_DEVICE    
            qDebug("Seek block increment, %d %d", current_block,
                   mobi->numBlocks());
#endif
            
            if (current_block > mobi->numBlocks())
            {
#ifdef DEBUG_DEVICE    
                qDebug("Short seek %lld", have_read);
#endif
                return true;
            }
            else
            {
                block_data = mobi->readBlock(current_block);
                offset = 0;
            } 

            seekTo -= partial_read;
            pos += partial_read;
        }
    }

#ifdef DEBUG_DEVICE    
    qDebug("Bailing at end");
#endif
    return true;
}

void BookDevice::reset_internal()
{
    pos = 0;
    offset = 0;
    current_block = 1;
    block_data = mobi->readBlock(1);
}

bool BookDevice::atEnd()
{
#ifdef DEBUG_DEVICE    
    qDebug("atEnd queried");
#endif
    return (current_block == mobi->numBlocks());
}

qint64 BookDevice::bytesAvailable () const
{
    qint64 ret = size-pos + QIODevice::bytesAvailable();
#ifdef DEBUG_DEVICE    
    qDebug("Returning %lld bytes available, size %lld pos %lld iodevice %lld",
           ret, size, pos, QIODevice::bytesAvailable());
#endif
    return (ret > 0) ? ret : 0;
}



