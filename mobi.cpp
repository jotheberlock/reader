#include "mobi.h"

#include <QtCore/QtEndian>
#include <QtCore/QFile>
#include <QtGui/QTextEdit>
#include <QtCore/QDateTime>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include "bookdevice.h"

QString PDBHeader::makeDate(quint32 date)
{
    QDateTime qds;
    qds = QDateTime::fromString("1970-01-01", Qt::ISODate);    
    qds = qds.addSecs(date);

    return qds.toString();
}

void PDBHeader::fromCC(quint32 from, char * to)
{
    to[4] = 0;
    memcpy(to, (char *)&from, 4);
}

void PDBHeader::swap()
{
    attributes = qFromBigEndian(attributes);
    version = qFromBigEndian(version);
    create_date = qFromBigEndian(create_date);
    mod_date = qFromBigEndian(mod_date);
    backup_date = qFromBigEndian(backup_date);
    mod_number = qFromBigEndian(mod_number);
    appInfoID = qFromBigEndian(appInfoID);
    sortInfoID = qFromBigEndian(sortInfoID);
    uniqueIDseed = qFromBigEndian(uniqueIDseed);
    num_records = qFromBigEndian(num_records);
}

void PDBHeader::dump()
{
    char buf[5];
    
    qDebug("PDB Header:\n");
    qDebug("Name [%s]", name);
    qDebug("Attributes %x", attributes);
    qDebug("Version %x", version);
    qDebug("Create date %s", makeDate(create_date).toAscii().data());
    qDebug("Mod date %s", makeDate(mod_date).toAscii().data());
    qDebug("Backup date %s", makeDate(backup_date).toAscii().data());
    qDebug("Mod number %d", mod_number);
    qDebug("appInfoID %d", appInfoID);
    qDebug("sortInfoID %d", sortInfoID);
    fromCC(type, buf);
    qDebug("Type %s", buf);
    fromCC(creator, buf);
    qDebug("Creator %s", buf);
    qDebug("Unique ID seed %d", uniqueIDseed);
    qDebug("Number of records %d", num_records);
}

void RecordData::swap()
{
    offset = qFromBigEndian(offset);
}    

void PalmdocHeader::swap()
{
    compression = qFromBigEndian(compression);
    text_length = qFromBigEndian(text_length);
    record_count = qFromBigEndian(record_count);
    record_size = qFromBigEndian(record_size);
    encryption = qFromBigEndian(encryption);
}

void PalmdocHeader::dump()
{
    qDebug("Palmdoc header");
    qDebug("Compression %d", compression);
    qDebug("Text length %d", text_length);
    qDebug("Record count %d", record_count);
    qDebug("Record size %d", record_size);
    qDebug("Encryption %d", encryption);
}


void MobiHeader::swap()
{
    header_length = qFromBigEndian(header_length);
    type = qFromBigEndian(type);
    encoding = qFromBigEndian(encoding);
    uid = qFromBigEndian(uid);
    version = qFromBigEndian(version);
    first_non_book = qFromBigEndian(first_non_book);
    full_name_offset = qFromBigEndian(full_name_offset);
    full_name_length = qFromBigEndian(full_name_length);
    book_locale = qFromBigEndian(book_locale);
    input_language = qFromBigEndian(input_language);
    output_language = qFromBigEndian(output_language);
    min_version = qFromBigEndian(min_version);
    first_image = qFromBigEndian(first_image);
    huffman_record_offset = qFromBigEndian(huffman_record_offset);
    huffman_record_count = qFromBigEndian(huffman_record_count);
    exth_flags = qFromBigEndian(exth_flags);
    drm_offset = qFromBigEndian(drm_offset);
    drm_count = qFromBigEndian(drm_count);
    drm_size = qFromBigEndian(drm_size);
    drm_flags = qFromBigEndian(drm_flags);
}

void MobiHeader::dump()
{
    qDebug("Mobi header");
    qDebug("Header length %d", header_length);
    qDebug("Type %d", type);
    qDebug("Encoding %d", encoding);
    qDebug("UID %d", uid);
    qDebug("Version %d", version);
    qDebug("First non book %d", first_non_book);
    qDebug("Full name offset %d", full_name_offset);
    qDebug("Full name length %d", full_name_length);
    qDebug("Book locale %x", book_locale);
    qDebug("Input language %d", input_language);
    qDebug("Output language %d", output_language);
    qDebug("Min version %d", min_version);
    qDebug("First image %d", first_image);
    qDebug("Huffman record offset %d", huffman_record_offset);
    qDebug("Huffman record count %d", huffman_record_count);
    qDebug("EXTH flags %x", exth_flags);
    qDebug("DRM offset %d", drm_offset);
    qDebug("DRM count %d", drm_count);
    qDebug("DRM size %d", drm_size);
    qDebug("DRM flags %x", drm_flags);
}

void ExthRecord::swap()
{
    type = qFromBigEndian(type);
    length = qFromBigEndian(length);
}

void ExthRecord::dump()
{
    qDebug("Record %d length %d ", type, length);

    bool is_text = false;
    if (type >= 100 && type <= 114)
    {
        is_text = true;
    }
    else if (type >= 118 && type <=119)
    {
        is_text = true;
    }

    if (is_text)
    {
        qDebug("[%s]", data);
    }
}

QByteArray Mobi::readBlock(int b)
{
    QByteArray outbuf;

    int size = records[b+1].offset-records[b].offset;
    
    char * buf = new char[size];

    if (palmdoc_header->compression == 1 ||
        (unsigned int)b >= mobi_header->first_non_book)
    {
        device->seek(records[b].offset);
        device->read(buf, size);
        outbuf = QByteArray(buf, size);
    }
    else if (palmdoc_header->compression == 2)
    {
        device->seek(records[b].offset);
        device->read(buf, size);
        
        unsigned char * inptr = (unsigned char *)buf;
        unsigned char * endptr = (unsigned char *)buf+size;
        
        while (inptr < endptr)
        {
            if ( (*inptr == 0x0) ||
                 ( (*inptr >= 0x09) && (*inptr <= 0x7f) ) )
            {
                outbuf += *inptr;
                inptr++;
            }
            else if ( *inptr >= 0x1 && *inptr <= 0x08 )
            {
                int count = *inptr;
                
                inptr++;
                
                for (int loopc2=0; loopc2<count; loopc2++)
                {
                    outbuf += *inptr;
                    inptr++;
                }
            }
            else if ( *inptr >= 0x80 && *inptr <= 0xbf )
            {
                unsigned int ld = (*inptr & 0x3f) << 8;
                inptr++;
                ld |= *inptr;
                inptr++;

                unsigned int distance = (ld & ~0x7) >> 3;
                unsigned int len = ld & 0x7;

                unsigned int refptr = outbuf.size();

                for (unsigned int loopc=0; loopc<len+3; loopc++)
                {
                    outbuf += outbuf[refptr - distance + loopc];
                }
            }
            else   // 0xc0 to 0xff
            {
                outbuf += ' ';
                outbuf += (*inptr) ^ 0x80;
                inptr++;
            }
        }
    }

    return outbuf;
}

bool Mobi::sniff(QIODevice * d)
{
    device = d;
    header = new PDBHeader;
    device->read((char *)header, 78);
    header->swap();

    records = new RecordData[header->num_records];

    for (int loopc=0; loopc<header->num_records; loopc++)
    {
        device->read((char *)(&records[loopc]), 8);
        records[loopc].swap();
    }

    palmdoc_header = new PalmdocHeader;
    device->seek(records[0].offset);
    device->read((char *)palmdoc_header, 16);
    palmdoc_header->swap();

    char mobisig[5];
    mobisig[4] = 0;
    device->read((char *)mobisig, 4);
    if (strcmp(mobisig, "MOBI") != 0)
    {
        return false;
    }

    if (palmdoc_header->encryption != 0)
    {
        return false;
    }

    if (palmdoc_header->compression != 1 && palmdoc_header->compression != 2)
    {
        return false;
    }
    
    header->dump();
    palmdoc_header->dump();
    
    mobi_header = new MobiHeader;
    device->read((char *)mobi_header, sizeof(MobiHeader));
    mobi_header->swap();
    mobi_header->dump();

    if (mobi_header->drm_offset != 0xffffffff)
    {
        return false;
    }
    
    if (!mobi_header->exth_flags & 0x40)
    {
        return true;
    }
    
    char buf[4096];
    device->read(buf, mobi_header->header_length - (sizeof(MobiHeader)+4));
    
    char extsig[5];
    extsig[4] = 0;
    device->read((char *)extsig, 4);
    if (strcmp(extsig, "EXTH") != 0)
    {
        qDebug("Expected EXTH didn't find it [%s]", extsig);
        return true;
    }
    
    device->read((char *)extsig, 4);
    quint32 num_records;
    device->read((char *)&num_records, 4);
    num_records = qFromBigEndian(num_records);

    qDebug("%d EXTH records", num_records);
    
    for (unsigned int loopc=0; loopc<num_records; loopc++)
    {
        ExthRecord * etr = new ExthRecord;
        device->read((char *)etr, 8);
        etr->swap();

        int left_to_read = etr->length-8;

        etr->data = new char[left_to_read+1];
        memset(etr->data, 0, left_to_read+1);
        device->read(etr->data, left_to_read);
        etr->dump();
        exth_records.push_back(etr);
    }

    char * fname = new char[mobi_header->full_name_length+1];
    memset(fname, 0, mobi_header->full_name_length+1);
    device->seek(records[0].offset + mobi_header->full_name_offset);
    device->read(fname, mobi_header->full_name_length);
    qDebug("Full name [%s]", fname);
    fullname = QString(fname);
    delete[] fname;

    return true;
}

void Mobi::open()
{
    unsigned int current_offset = 0;
    
    for (unsigned int loopc=1; loopc<mobi_header->first_non_book; loopc++)
    {
        QByteArray qba = readBlock(loopc);
        block_offsets.push_back(current_offset);
        current_offset += qba.size();
    }

    text_size = current_offset;
    
        /*
    QTextEdit * qte = new QTextEdit();

    QPalette pal;
    QColor fgColor("blue");
    pal.setColor(QPalette::Text, fgColor);
    qte->setPalette(pal);

    BookDevice bd(this);
    if (bd.open(QIODevice::ReadOnly))
    {
        qDebug("Opened device");
    }
    QTextStream str(&bd);

    if (mobi_header->encoding == 1252)
    {
        str.setCodec("iso8859-1");
    }
    else
    {
        str.setCodec("UTF-8");
    }

    QString text = str.readAll();
    qte->setHtml(text);
    qte->show();
        */

        /*
    BookDevice bd(this);
    if (bd.open(QIODevice::ReadOnly))
    {
        qDebug("Opened device");
    }
    
    QTextStream str(&bd);

    if (mobi_header->encoding == 1252)
    {
        str.setCodec("iso8859-1");
    }
    else
    {
        str.setCodec("UTF-8");
    }

    QString text = str.readAll();
    
    QFile file("../book.html");
    file.open(QIODevice::WriteOnly);
    QTextStream qts(&file);
    qts << text;
    file.close();
        */
}


