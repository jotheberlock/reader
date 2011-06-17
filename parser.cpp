#include "parser.h"

Parser::Parser(QIODevice * d, int encoding)
{
    device = d;

    stream = new QTextStream(d);

    if (encoding == 1252)
    {
        stream->setCodec("iso8859-1");
    }
    else
    {
        stream->setCodec("UTF-8");
    }
}

void Parser::handleTag(QString s)
{
    s = s.toLower();
    if (s[0] == '/')
    {
        if (tags.size() == 0)
        {
            printf("Got close tag [%s] without opening tag\n",
                   s.toAscii().data());
        }
        
        Tag * top = tags.top();
    
        if (s != "/" + top->name)
        {
            printf("Mismatched tag. [%s] [%s]\n",
                   s.toAscii().data(), top->name.toAscii().data());
        }

        dumpTag(top);
        tags.pop();
        delete top;
    }
    else
    {
        tags.push(new Tag);
        
        Tag * top = tags.top();
                
        QStringList qsl = s.split(' ');
        top->name = qsl[0];
        for (int loopc=1; loopc<qsl.size(); loopc++)
        {
            QStringList al = qsl[loopc].split('=');
            if (al.size() != 2)
            {
                printf("Malformed attribute [%s]\n",
                       qsl[loopc].toAscii().data());
                continue;
            }
            
            TagAttribute attr;
            attr.name = al[0];
            attr.value = al[1];
            top->attributes.push_back(attr);
        }
    }    
}

void Parser::handleContent(QString s) 
{
    if (tags.size() == 0)
    {
        printf("Content [%s] with no tag\n", s.toAscii().data()); 
        return;
    }

    tags.top()->contents = s;
}

Element * Parser::next()
{
    QString accum;

    if (stream->atEnd())
    {
        printf("Stream at end\n");
    }

    continuing = true;
    parsing_tag = false;
        
    while ((!stream->atEnd()) && continuing)
    {
        QChar ch;
        *stream >> ch;
        
        if (parsing_tag)
        {
            if (ch == '>')
            {
                parsing_tag = false;
                handleTag(accum);
                accum = "";
            }
            else
            {
                accum += ch;
            } 
        }
        else
        {
            if (ch == '<')
            {
                if (accum != "")
                {
                    handleContent(accum);
                }
                accum = "";
                parsing_tag = true;
            }
            else
            {
                accum += ch;
            }
        }
    }
}

void Parser::dumpTag(Tag * tag)
{
    printf("Name [%s] Contents [%s]\n",
           tag->name.toAscii().data(), tag->contents.toAscii().data());
    for (int loopc=0; loopc<tag->attributes.size();loopc++)
    {
        printf("[%s] = [%s]\n",
               tag->attributes[loopc].name.toAscii().data(),
               tag->attributes[loopc].value.toAscii().data());
    }
}

void Parser::dumpStack()
{
    for (int loopc=0; loopc<tags.size(); loopc++)
    {
        printf("%d: [%s]\n", loopc, tags[loopc]->name.toAscii().data());
    }
}
