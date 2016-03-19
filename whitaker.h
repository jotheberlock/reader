#ifndef _WHITAKER_
#define _WHITAKER

#include <QMap>
#include "dictionary.h"

class WhitLocation
{
  public:

    qint64 pos;
    qint64 len;
    
};


class WhitakerDictionary : public Dictionary
{
  public:

    WhitakerDictionary()
    {
        loaded=false;
    }
    
    virtual QString name() { return "Whitakers Words"; }
    
        // return "" if not found
    virtual QString lookup(QString word);

  protected:

    void doLoad();
    QMap<QString,WhitLocation> words;
    bool loaded;
    
};

#endif
