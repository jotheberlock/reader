#include "parser.h"

// #define DEBUG_PARSER

Parser::Parser(QIODevice * d, Mobi * m)
{
    device = d;
    mobi = m;
    
    stream = new QTextStream(d);
    element = 0;
    in_paragraph = false;
    in_special_entity = false;
    
    if (mobi->encoding() == 1252)
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

    special_entities["quot"] = "\"";
    special_entities["apos"] = "'";
    special_entities["amp"] = "&";
    special_entities["lt"] = "<";
    special_entities["gt"] = ">";
    special_entities["nbsp"] = " ";
    special_entities["iexcl"] = QChar(0xa1);
    special_entities["euro"] = QChar(0x20ac);
    special_entities["cent"] = QChar(0x161);
    special_entities["pound"] = QChar(0x163);
    special_entities["curren"] = QChar(0x164);
    special_entities["yen"] = QChar(0x165);
    special_entities["brvbar"] = QChar(0xa6);
    special_entities["sect"] = QChar(0xa7);
    special_entities["uml"] = QChar(0xa8);
    special_entities["copy"] = QChar(0xa9);
    special_entities["ordf"] = QChar(0xaa);
    special_entities["laquo"] = QChar(0xa8);
    special_entities["not"] = QChar(0xac);
    special_entities["reg"] = QChar(0xae);
    special_entities["macr"] = QChar(0xaf);
    special_entities["deg"] = QChar(0xb0);
    special_entities["plusmn"] = QChar(0xb1);
    special_entities["sup2"] = QChar(0xb2);
    special_entities["sup3"] = QChar(0xb3);
    special_entities["acute"] = QChar(0xb4);
    special_entities["micro"] = QChar(0xb5);
    special_entities["para"] = QChar(0xb6);
    special_entities["middot"] = QChar(0xb7);
    special_entities["cedil"] = QChar(0xb8);
    special_entities["sup1"] = QChar(0xb9);
    special_entities["ordm"] = QChar(0xba);
    special_entities["raquo"] = QChar(0xbb);
    special_entities["frac14"] = QChar(0xbc);
    special_entities["frac12"] = QChar(0xbd);
    special_entities["frac34"] = QChar(0xbe);
    special_entities["iquest"] = QChar(0xbf);
    special_entities["times"] = QChar(0xd7);
    special_entities["divide"] = QChar(0xf7);
    special_entities["lsquo"] = QChar(0x2018);
    special_entities["rsquo"] = QChar(0x2019);
    special_entities["ldquo"] = QChar(0x201c);
    special_entities["rdquo"] = QChar(0x201d);
    special_entities["bullet"] = QChar(0x2022);
    special_entities["endash"] = QChar(0x2013);
    special_entities["emdash"] = QChar(0x2014);
    special_entities["ndash"] = QChar(0x2013);
    special_entities["mdash"] = QChar(0x2014);
    special_entities["AElig"] = QChar(0x00c6);
    special_entities["aelig"] = QChar(0x00e6);
    special_entities["OElig"] = QChar(0x0152);
    special_entities["oelig"] = QChar(0x0153);

    block_tags["p"] = "p";
    block_tags["h1"] = "h1";
    block_tags["h2"] = "h2";
    block_tags["h3"] = "h3";
    block_tags["h4"] = "h4";
    block_tags["h5"] = "h5";
    block_tags["h6"] = "h6";
    block_tags["dl"] = "dl";
    block_tags["ol"] = "ol";
    block_tags["ul"] = "ul";
    block_tags["address"] = "address";
        //block_tags["blockquote"] = "blockquote";
    block_tags["center"] = "center";
    block_tags["del"] = "del";
    block_tags["div"] = "div";
    block_tags["hr"] = "hr";
    block_tags["ins"] = "ins";
    block_tags["noscript"] = "noscript";
    block_tags["pre"] = "pre";
    block_tags["script"] = "script";
}

Parser::~Parser()
{
    while (tags.size() > 0)
    {
        delete tags.pop();
    }
    
    delete device;
}

void Parser::reset()
{
    device->reset();
    for (int loopc=0; loopc<tags.size(); loopc++)
    {
        delete tags[loopc];
    }
    tags.clear();

    delete stream;
    stream = new QTextStream(device);

    if (mobi->encoding() == 1252)
    {
        stream->setCodec("iso8859-1");
    }
    else
    {
        stream->setCodec("UTF-8");
    }
    
    element = 0;
    in_paragraph = false;    
    in_special_entity = false;
    next_element = 0;
}

void Parser::handleTag(QString s)
{
    if (s[0] == '/')
    {
        s = s.toLower();
        if (tags.size() == 0)
        {
#ifdef DEBUG_PARSER
            qDebug("Got close tag [%s] without opening tag",
                   s.toUtf8().data());
#endif
            return;
        }
        
        Tag * top = tags.top();
        
#ifdef DEBUG_PARSER
        if (s != "/" + top->name)
        {
            qDebug("Mismatched tag. [%s] [%s]",
                   s.toUtf8().data(), top->name.toUtf8().data());
        }
#endif
        
        dumpTag(top);
        tags.pop();
        delete top;
    }
    else
    {
#ifdef DEBUG_PARSER
        if (s.toLower() == "br")
        {
            qDebug("Br found in para %s", in_paragraph ? "yes" : "no");
        }
#endif
            // Handles docs which are formatted as <br>text<br>text<br>
            // We consider if this para was started by a br, it is also
            // ended by one.
        bool is_nested = false;
        if ( (s.toLower() == "br") &&
             (tags.size() > 0) &&
             (tags.top()->name == "br") )
        {
            is_nested = true;
        }
        
        if (in_paragraph && (block_tags.contains(s.toLower())))
        {
            is_nested = true;
        }
            
        if (is_nested)
        {
#ifdef DEBUG_PARSER
            qDebug("Nested block tag [%s]! %d",
                   s.toLower().toUtf8().data(), tags.size());
#endif
                // Nested block tag
            if (tags.size() > 0)
            {
                Tag * top = tags.top();
                dumpTag(top);
                tags.pop();
#ifdef DEBUG_PARSER
                qDebug("Tags stack popped to %d", tags.size());
                dumpStack();
#endif
                delete top;
                in_paragraph = false;
            }
            return;
        }
            
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

#ifdef DEBUG_PARSER        
        qDebug("Name is [%s]", top->name.toUtf8().data());
#endif
        if (top->name == "p" || top->name == "br")
        {
#ifdef DEBUG_PARSER 
            qDebug("Entering new paragraph");
#endif
            element = new ParagraphElement;
            element->setNumber(next_element++);
            
            in_paragraph = true;
        }
        else if (void_tags.contains(top->name) || self_closing)
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
#ifdef DEBUG_PARSER 
        qDebug("Content [%s] with no tag", s.toUtf8().data());
#endif
        return;
    }

    if (tags.top()->name == "blockquote")
    {
        qDebug("Blockquote with [%s], %s",
               s.toUtf8().data(), in_paragraph ? "yes" : "no");
    }
    
    if (in_paragraph)
    {
#ifdef DEBUG_PARSER 
        qDebug("Adding text [%s] to %s", s.toUtf8().data(),
               tags.top()->name.toUtf8().data());
#endif
        StringFragment sf;
        sf.text = s.split(' ');
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
        ((ParagraphElement *)element)->addFragment(sf);
    }
}

QString Parser::handleSpecialEntity(QString s)
{
#ifdef DEBUG_PARSER    
    qDebug("Looking for entity [%s]", s.toUtf8().data());
#endif
    
    if (s[0] == '#')
    {
        bool ok;
        ushort ret;
        
        if (s[1] == 'x')
        {
            s = s.remove(0,2);
            ret = s.toUShort(&ok, 16);
        }
        else
        {
            s = s.remove(0,1);
            ret = s.toUShort(&ok, 10);
        }
        
        if (ok)
        {
            QString sret = QChar(ret);
            return sret;
        }
        else
        {
            return "";
        }
    }
    
    if (special_entities.contains(s))
    {
        return special_entities[s];
    }
    
    return "";
}

Element * Parser::next()
{
    QString accum;
    QString entity_accum;
    
    if (stream->atEnd())
    {
        return 0;
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
#ifdef DEBUG_PARSER
                    if (tags.size() > 0)
                    {
                        qDebug("Handle content for [%s] of [%s]",
                               tags.top()->name.toUtf8().data(),
                               accum.toUtf8().data());
                    }
#endif
                    handleContent(accum);
                }
                accum = "";
                parsing_tag = true;
            }
            else if (ch == '&' && !in_special_entity)
            {
                in_special_entity = true;
                entity_accum = "";
            }
            else if (ch == ';' && in_special_entity)
            {
                in_special_entity = false;
                accum += handleSpecialEntity(entity_accum);
            }
            else if (in_special_entity)
            {
                entity_accum += ch;
            }
            else
            {
                accum += ch;
            }
        }
    }

    Element * ret = element;
    element = 0;
    in_paragraph = false;
    return ret;
}

void Parser::dumpTag(Tag * tag)
{
#ifdef DEBUG_PARSER
    qDebug("Name [%s] Contents [%s]",
           tag->name.toUtf8().data(), tag->contents.toUtf8().data());
    for (int loopc=0; loopc<tag->attributes.size();loopc++)
    {
        qDebug("[%s] = [%s]",
               tag->attributes[loopc].name.toUtf8().data(),
               tag->attributes[loopc].value.toUtf8().data());
    }
#endif
    
    if (tag->name == "p" || tag->name == "br")
    {   
        if (element)
        {
            if (((ParagraphElement *)element)->numFragments() > 0)
            {
#ifdef DEBUG_PARSER
                qDebug("Have para for [%s], %d fragments",
                       tag->contents.toUtf8().data(),
                       ((ParagraphElement *)element)->numFragments());
                ((ParagraphElement *)element)->dump();
#endif
                continuing = false;
            }
            else
            {
#ifdef DEBUG_PARSER
                qDebug("Zero-fragment paragraph");
#endif
            } 
        }
        else
        {
#ifdef DEBUG_PARSER
            qDebug("Null paragraph element");
#endif
        } 
        
        in_paragraph = false;
    }
    else if(tag->name == "mbp:pagebreak")
    {
#ifdef DEBUG_PARSER
        qDebug("Making pagebreak element");
#endif
        PagebreakElement * pe = new PagebreakElement;
        element = pe;
        element->setNumber(next_element++);
        continuing = false;
    } 
    else if (tag->name == "img")
    {    
        for (int loopc=0; loopc<tag->attributes.size();loopc++)
        {
            if (tag->attributes[loopc].name == "recindex")
            {
                bool ok;
                int pnum = tag->attributes[loopc].value.toInt(&ok);
                if (!ok)
                {
#ifdef DEBUG_PARSER
                    qDebug("Can't parse image [%s]",
                           tag->attributes[loopc].value.toUtf8().data());
#endif
                    break;
                }
                
#ifdef DEBUG_PARSER
                qDebug("Making image [%d]", pnum);
#endif
                QByteArray qba = mobi->readBlock(mobi->firstImage()+pnum-1);
                QImage qi = QImage::fromData(qba, "GIF");
                if (!qi.isNull())
                {
                    PictureElement * pe = new PictureElement(qi);
                    element = pe;
                    element->setNumber(next_element++);
                    continuing = false;
                }
                else
                {
#ifdef DEBUG_PARSER
                    qDebug("Is null");
#endif
                }
            }
        }
    }
}

void Parser::dumpStack()
{
    for (int loopc=0; loopc<tags.size(); loopc++)
    {
        qDebug("%d: [%s]", loopc, tags[loopc]->name.toUtf8().data());
    }
}
