#include "parser.h"

Parser::Parser(QIODevice * d, int encoding)
{
    device = d;

    stream = new QTextStream(d);
    para = 0;
    in_paragraph = false;
    
    if (encoding == 1252)
    {
        stream->setCodec("iso8859-1");
    }
    else
    {
        stream->setCodec("UTF-8");
    }

        // http://www.w3.org/TR/2009/WD-html5-20090423/syntax.html#void-elements
    
    void_tags["area"] = "area";
    void_tags["base"] = "base";
    void_tags["br"] = "br";
    void_tags["col"] = "col";
    void_tags["command"] = "command";
    void_tags["embed"] = "embed";
    void_tags["hr"] = "hr";
    void_tags["img"] = "img";
    void_tags["input"] = "input";
    void_tags["keygen"] = "keygen";
    void_tags["link"] = "link";
    void_tags["meta"] = "meta";
    void_tags["param"] = "param";
    void_tags["source"] = "source";
}

void Parser::handleTag(QString s)
{
    if (s[0] == '/')
    {
        s = s.toLower();
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

        bool self_closing = (s.right(1) == "/");
        
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
                else if (qc == '/')
                {
                    self_closing = true;
                    break;
                }       
                else
                {
                    top->name += qc.toLower();
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
                    ta.name += qc.toLower();
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

        if (ta.name != "")
        {
            top->attributes.push_back(ta);
        }

        if (top->name == "p")
        {
            para = new ParagraphElement;
            in_paragraph = true;
        }
        
        if (void_tags.contains(top->name) || self_closing)
        {
            dumpTag(top);
            tags.pop();
            delete top;
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

    tags.top()->contents += s;
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

    return para;
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

    if (tag->contents != "")
    {
        printf("Processing [%s] [%s]\n",
               tag->name.toAscii().data(),
               tag->contents.toAscii().data());
        if (tag->name == "p")
        {
            StringFragment sf;
        
            sf.text = tag->contents.split(' ');
            para->addFragment(sf);
            continuing = false;
            in_paragraph = false;
        }
        else if (in_paragraph)
        {
            StringFragment sf;
            sf.text = tag->contents.split(' ');
            for (int loopc=0; loopc<tags.size(); loopc++)
            {
                Tag * tag = tags.at(loopc);
                if (tag->name == "b")
                {
                    sf.is_bold = true;
                }
                else if (tag->name == "i")
                {
                    sf.is_italic = true;
                }
            }
            para->addFragment(sf);
        }
    }
}

void Parser::dumpStack()
{
    for (int loopc=0; loopc<tags.size(); loopc++)
    {
        printf("%d: [%s]\n", loopc, tags[loopc]->name.toAscii().data());
    }
}
