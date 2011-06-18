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

        int state = 0;
        TagAttribute ta;
        bool in_commas = false;
        
        for (int loopc=0; loopc<s.length(); loopc++)
        {
            QChar qc = s[loopc];
            
            if (state == 0)  // Name 
            {
                if (qc == ' ')
                {
                    state = 1;
                }
                else
                {
                    top->name += qc;
                } 
            }
            else if (state == 1) // Whitespace
            {
                if (qc != ' ')
                {
                    state = 2;
                    ta.name = qc;
                    ta.value = "";
                }
            }
            else if (state == 2) // attribute name
            {
                if (qc == '=')
                {
                    state = 3;
                }
                else
                {
                    ta.name += qc;
                }    
            }
            else if (state == 3) // attribute value
            {
                if (qc == '"')
                {
                    in_commas = !in_commas;
                }
                else if (qc ==' ' && !in_commas)
                {
                    state = 1;
                    top->attributes.push_back(ta);
                }
                else
                {
                    ta.value += qc;
                }
            }
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
