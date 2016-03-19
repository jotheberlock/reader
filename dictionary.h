#ifndef _DICTIONARY_
#define _DICTIONARY_

#include <QString>

class Dictionary
{
  public:

    virtual QString name() = 0;
        // return "" if not found
    virtual QString lookup(QString word) = 0;

};

#endif
