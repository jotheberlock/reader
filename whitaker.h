#ifndef _WHITAKER_
#define _WHITAKER

#include "dictionary.h"

class WhitakerDictionary : public Dictionary
{
  public:
    
    virtual QString name() { return "Whitakers Words"; }
    
        // return "" if not found
    virtual QString lookup(QString word);

};

#endif
