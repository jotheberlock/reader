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
    
    printf("PDB Header:\n\n");
    printf("Name [%s]\n", name);
    printf("Attributes %x\n", attributes);
    printf("Version %x\n", version);
    printf("Create date %s\n", makeDate(create_date).toAscii().data());
    printf("Mod date %s\n", makeDate(mod_date).toAscii().data());
    printf("Backup date %s\n", makeDate(backup_date).toAscii().data());
    printf("Mod number %d\n", mod_number);
    printf("appInfoID %d\n", appInfoID);
    printf("sortInfoID %d\n", sortInfoID);
    fromCC(type, buf);
    printf("Type %s\n", buf);
    fromCC(creator, buf);
    printf("Creator %s\n", buf);
    printf("Unique ID seed %d\n", uniqueIDseed);
    printf("Number of records %d\n", num_records);
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
    printf("Palmdoc header\n\n");
    printf("Compression %d\n", compression);
    printf("Text length %d\n", text_length);
    printf("Record count %d\n", record_count);
    printf("Record size %d\n", record_size);
    printf("Encryption %d\n", encryption);
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
    printf("Mobi header\n\n");
    printf("Header length %d\n", header_length);
    printf("Type %d\n", type);
    printf("Encoding %d\n", encoding);
    printf("UID %d\n", uid);
    printf("Version %d\n", version);
    printf("First non book %d\n", first_non_book);
    printf("Full name offset %d\n", full_name_offset);
    printf("Full name length %d\n", full_name_length);
    printf("Book locale %x\n", book_locale);
    printf("Input language %d\n", input_language);
    printf("Output language %d\n", output_language);
    printf("Min version %d\n", min_version);
    printf("First image %d\n", first_image);
    printf("Huffman record offset %d\n", huffman_record_offset);
    printf("Huffman record count %d\n", huffman_record_count);
    printf("EXTH flags %x\n", exth_flags);
    printf("DRM offset %d\n", drm_offset);
    printf("DRM count %d\n", drm_count);
    printf("DRM size %d\n", drm_size);
    printf("DRM flags %x\n", drm_flags);
}

void ExthRecord::swap()
{
    type = qFromBigEndian(type);
    length = qFromBigEndian(length);
}

void ExthRecord::dump()
{
    printf("Record %d length %d ", type, length);

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
        printf("[%s]", data);
    }
    
    printf("\n");
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

void Mobi::open(QIODevice * d)
{
    device = d;
    header = new PDBHeader;
    device->read((char *)header, 78);
    header->swap();
    header->dump();

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
    palmdoc_header->dump();

    char mobisig[5];
    mobisig[4] = 0;
    device->read((char *)mobisig, 4);
    if (strcmp(mobisig, "MOBI") != 0)
    {
        return;
    }

    mobi_header = new MobiHeader;
    device->read((char *)mobi_header, sizeof(MobiHeader));
    mobi_header->swap();
    mobi_header->dump();

    if (!mobi_header->exth_flags & 0x40)
    {
        return;
    }
    
    char buf[4096];
    device->read(buf, mobi_header->header_length - (sizeof(MobiHeader)+4));
    
    char extsig[5];
    extsig[4] = 0;
    device->read((char *)extsig, 4);
    if (strcmp(extsig, "EXTH") != 0)
    {
        printf("Expected EXTH didn't find it [%s]\n", extsig);
        return;
    }
    
    device->read((char *)extsig, 4);
    quint32 num_records;
    device->read((char *)&num_records, 4);
    num_records = qFromBigEndian(num_records);

    printf("%d EXTH records\n", num_records);
    
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
    printf("Full name [%s]\n", fname);
    fullname = QString(fname);
    delete[] fname;

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
        printf("Opened device\n");
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
        printf("Opened device\n");
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


