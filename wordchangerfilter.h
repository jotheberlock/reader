#ifndef _WORDCHANGERFILTER_
#define _WORDCHANGERFILTER_

#include "filter.h"

class ParagraphRecord;

class WordChangerFilter : public Filter
{
  public:

    virtual bool handlesTouch() { return true; }
    virtual QString name() { return "Word changer"; }
    
    virtual void newBook(Page *);
        // Called when book is closed
    virtual void endBook(Page *);
        // Called before StringFragments are laid out
    virtual void beforeLayout(ParagraphElement *);
    virtual void onRelease(ParagraphElement *, Page *, qint64 x, qint64 y);
    
  protected:

    QHash<qint64, ParagraphRecord *> changes;

    QString bookname;
    void save();
    void load();
    
};

#endif
