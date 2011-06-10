#ifndef _PARSER_
#define _PARSER_

#include "bookdevice.h"
#include "element.h"

#include <QtXml/qxml.h>

class Parser : public QXmlDefaultHandler
{
  public:

    Parser(BookDevice * d);
    Element * next();
    
    virtual bool startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
    virtual bool endElement ( const QString & namespaceURI, const QString & localName, const QString & qName );
    virtual bool characters ( const QString & ch );
    
    virtual bool warning ( const QXmlParseException & exception );
    virtual bool error ( const QXmlParseException & exception );
    virtual bool fatalError ( const QXmlParseException & exception );
        
  protected:

    BookDevice * device;
    QXmlSimpleReader * reader;
    QXmlInputSource * input;
    bool in_para;
    QString para_text;
    
};

#endif
