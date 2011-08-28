#ifndef _MOBI_
#define _MOBI_

#include <QtCore/QDataStream>
#include <QtCore/QVector>
#include <QtGui/QImage>

class PDBHeader
{
  public:

    char name[32];
    quint16 attributes;
    quint16 version;
    quint32 create_date;
    quint32 mod_date;
    quint32 backup_date;
    quint32 mod_number;
    quint32 appInfoID;
    quint32 sortInfoID;
    quint32 type;
    quint32 creator;
    quint32 uniqueIDseed;
    quint32 dummy;
    quint16 num_records;

    void fromCC(quint32, char *);
    void swap();
    void dump();
    QString makeDate(quint32);
    
};

class RecordData
{
  public:

    void swap();
    
    quint32 offset;
    quint32 bits;
};

class PalmdocHeader
{
  public:

    void swap();
    void dump();
    
    quint16 compression;
    quint16 dummy;
    quint32 text_length;
    quint16 record_count;
    quint16 record_size;
    quint16 encryption;
    quint16 dummy2;
    
};

class MobiHeader
{
  public:

    void swap();
    void dump();

    quint32 header_length;
    quint32 type;
    quint32 encoding;
    quint32 uid;
    quint32 version;
    char reserved[40];
    quint32 first_non_book;
    quint32 full_name_offset;
    quint32 full_name_length;
    quint32 book_locale;
    quint32 input_language;
    quint32 output_language;
    quint32 min_version;
    quint32 first_image;
    quint32 huffman_record_offset;
    quint32 huffman_record_count;
    char dummy[8];
    quint32 exth_flags;
    char dummy2[32];
    quint32 drm_offset;
    quint32 drm_count;
    quint32 drm_size;
    quint32 drm_flags;
    
};

class ExthRecord
{
  public:

    void swap();
    void dump();
    
    quint32 type;
    quint32 length;
    char * data;
    
};


class Mobi
{
  public:

    bool sniff(QIODevice *);
    void open();

    int numBlocks()
    {
        return palmdoc_header->record_count;
    }
    
    QByteArray readBlock(int);
    int firstImage()
    {
        return mobi_header ? mobi_header->first_image : -1;
    }
    
    int encoding()
    {
        return mobi_header ? mobi_header->encoding : -1;
    }

    qint64 size()
    {
        return text_size;
    }

    QIODevice * getDevice()
    {
        return device;
    }

    QString getFullName()
    {
        return fullname;
    }

    QImage bookCover() { return book_cover; }
    
  protected:
    
    quint16 read16();
    quint32 read32();

    QString fullname;
    QIODevice * device;
    PDBHeader * header;
    RecordData * records;
    PalmdocHeader * palmdoc_header;
    MobiHeader * mobi_header;
    QList <ExthRecord *> exth_records;
    qint64 text_size;
    QImage book_cover;
    
};

#endif
