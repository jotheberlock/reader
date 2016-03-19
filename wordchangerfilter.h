#ifndef _WORDCHANGERFILTER_
#define _WORDCHANGERFILTER_

#include <QTime>
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
    virtual void onPress(ParagraphElement *, Page *, qint64 x, qint64 y);
        // Touch to change word, hold to undo changes in paragraph
    virtual void onRelease(ParagraphElement *, Page *, qint64 x, qint64 y);
    
  protected:

    QHash<qint64, ParagraphRecord *> changes;
    QTime press_time;
    
    QString bookname;
    void save();
    void load();
    
};

#endif
