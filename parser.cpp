#include "parser.h"

Parser::Parser(BookDevice * d)
{
    device = d;
    reader = new QXmlSimpleReader;
    input = new QXmlInputSource(d);
    reader->setContentHandler(this);
    reader->setErrorHandler(this);
    in_para = false;
}

bool Parser::startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts )
{
    printf("Start [%s]\n", qName.toAscii().data());
    if (qName == "p")
    {
        in_para = true;
    }

    return true;
}

bool Parser::endElement ( const QString & namespaceURI, const QString & localName, const QString & qName )
{
    printf("End [%s]\n", qName.toAscii().data());
    if (qName == "p")
    {
        printf("Pausing with text [%s]\n", para_text.toAscii().data());

        in_para = false;
            //device->pause(true);
    }
    return true;
}

bool Parser::characters ( const QString & ch )
{
    printf("Characters [%s]\n", ch.toAscii().data());
    
    if (in_para)
    {
        para_text = ch;
    }
    
    return true;
}

Element * Parser::next()
{
    printf("Doing parse\n");

    device->pause(false);
    if (reader->parse(input, false))
    {
        printf("Parse successful\n");
    }
    else
    {
        printf("Parse failure\n");
    }    
}

bool Parser::warning ( const QXmlParseException & exception )
{
    printf("WARNING: Column %d line %d %s\n", exception.columnNumber(),
           exception.lineNumber(), exception.message().toAscii().data());
    return true;
}

bool Parser::error ( const QXmlParseException & exception )
{
    printf("ERROR: Column %d line %d %s\n", exception.columnNumber(),
           exception.lineNumber(), exception.message().toAscii().data());
    return true;
}

bool Parser::fatalError ( const QXmlParseException & exception )
{
    printf("FATAL: Column %d line %d %s\n", exception.columnNumber(),
           exception.lineNumber(), exception.message().toAscii().data());
    return true;
}

        
