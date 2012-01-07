#include "dictionaryfilter.h"

DictionaryFilter::DictionaryFilter(Dictionary * d)
{
    dictionary = d;
}

void DictionaryFilter::onRelease(ParagraphElement * element, Page * page, qint64 x, qint64 y)
{
    QList<Word> & words = element->getWords();
    
    for (int loopc=0; loopc<words.size(); loopc++)
    {
        Word & word = words[loopc];
        if (x>=word.lx && y>=word.ly &&
            x<=(word.lx+word.w) && y<=(word.ly+word.h))
        {
            QString t = word.text;
            t.remove(":");
            t.remove(";");
            t.remove(",");
            t.remove(".");
            t.remove(":");
            t.remove("'");
            t.remove("\"");
            t.remove(">");
            t.remove("<");
            t.remove("[");
            t.remove("]");
            t.remove(".");
            t.remove("(");
            t.remove(")");
            t.remove("!");
            t=t.toLower();
            QString definition = dictionary->lookup(t);
            page->displayMessage(word.text, definition);
        }
    }
}
