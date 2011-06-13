#include "parser.h"

Parser::Parser(QIODevice * d, int encoding)
{
    device = d;
    state = 0;

    stream = new QTextStream(d);

    if (encoding == 1252)
    {
        stream->setCodec("iso8859-1");
    }
    else
    {
        stream->setCodec("UTF-8");
    }
    
    Tag root;
    tags.push_back(root);
}

Element * Parser::next()
{
    QString accum;

    if (stream->atEnd())
    {
        printf("Stream at end\n");
    }
    
    while (!stream->atEnd())
    {
        QChar ch;
        *stream >> ch;

            //printf("[%c]\n", ch.toAscii());

        Tag dummy;
        Tag & tag = (tags.size() > 0) ? tags.top() : dummy;
        
        if (state == 0)
        {
            if (ch == '<')
            {
                printf("Entering tag state\n");
                state = 1;
                Tag t;
                tags.push_back(t);
            }
            else
            {
                    // Do nothing
            } 
        }
        else if (state == 1)
        {
            if (ch == '/')
            {
                state = 0;
                dumpTag(tag);

                if (tags.size() > 0)
                {
                    tags.pop();
                }
                else
                {
                    printf("Rogue backslash\n");
                }
            }
            else if (ch == '>')
            {
                printf("Leaving tag state for content\n");
                
                state = 2;
            }
            else if (ch == '<')
            {
            }
            else
            {
                tag.name += ch;
            }
        }
        else if (state == 2)
        {
            if (ch == '<')
            {
                printf("Reentering tag state\n");
                state = 1;

                QChar ch2;
                *stream >> ch2;

                if (ch2 != '/')
                {
                    Tag t;
                    t.name = ch2;
                    tags.push_back(t);
                    printf("Is new tag name so far [%s]\n", t.name.toAscii().data());
                }
                else
                {   
                    dumpTag(tag);
                    if (tags.size() > 0)
                    {
                        tags.pop();
                    }
                    while (ch2 != '>')
                    {
                        *stream >> ch2;
                    }
                }
            }
            else
            {
                tag.contents += ch;
            } 
        }
    }
}

void Parser::dumpTag(Tag & tag)
{
    printf("Name [%s] Contents [%s]\n",
           tag.name.toAscii().data(), tag.contents.toAscii().data());
}
