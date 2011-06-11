#ifndef _PARSER_
#define _PARSER_

#include <QtCore/QStack>

#include "bookdevice.h"
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

    Parser(BookDevice * d);
    Element * next();
    
  protected:
    
    BookDevice * device;
    QStack<Tag> tags;
    
};

#endif
