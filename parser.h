#ifndef _PARSER_
#define _PARSER_

#include <QtCore/QStack>
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>
#include <QtCore/QHash>

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

    Parser(QIODevice * d, Mobi * m);
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

    Element * element;
    Mobi * mobi;
    qint64 next_element;
    
};

#endif
