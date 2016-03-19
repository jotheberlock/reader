#ifndef _PARSER_
#define _PARSER_

#include <QStack>
#include <QTextStream>
#include <QIODevice>
#include <QHash>

#include "element.h"
#include "mobi.h"

class TagAttribute
{
  public:

    QString name;
    QString value;
    
};


class Tag
{
  public:

    QString name;
    QList<TagAttribute> attributes;
    QString contents;
    
};

class Parser 
{
  public:

        // Note that the parser owns the device (probably a BookDevice)
        // and will delete it when deleted, but not the Mobi
    Parser(QIODevice * d, Mobi * m);
    ~Parser();
    
    Element * next();

    void dumpTag(Tag *);
    void dumpStack();

    void reset();
    
  protected:

    void handleTag(QString);
    void handleContent(QString);
    QString handleSpecialEntity(QString);
    
    QIODevice * device;
    QStack<Tag *> tags;

    bool parsing_tag;
    bool continuing;
    bool in_paragraph;
    bool in_special_entity;
    
    QTextStream * stream;

    QHash<QString, QString> void_tags;
    QHash<QString, QString> special_entities;
    QHash<QString, QString> block_tags;

    Element * element;
    Mobi * mobi;
    qint64 next_element;
    
};

#endif
