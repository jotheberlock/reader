#ifndef _PARSER_
#define _PARSER_

#include <QtCore/QStack>
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>
#include <QtCore/QHash>

#include "element.h"

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

    Parser(QIODevice * d, int encoding);
    Element * next();

    void dumpTag(Tag *);
    void dumpStack();
    
  protected:
    
    void handleTag(QString);
    void handleContent(QString);
    
    QIODevice * device;
    QStack<Tag *> tags;

    bool parsing_tag;
    bool continuing;
    
    QTextStream * stream;

    QHash<QString, QString> void_tags;

    ParagraphElement * para;
    
};

#endif
