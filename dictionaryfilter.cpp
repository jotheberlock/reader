#include "dictionaryfilter.h"

DictionaryFilter::DictionaryFilter(Dictionary * d)
{
    dictionary = d;
}

void DictionaryFilter::onRelease(ParagraphElement * element, Page * page, qint64 x, qint64 y)
{
    Word word = element->getWord(x,y);
    if (word.text == "")
    {
        return;
    }
    
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
