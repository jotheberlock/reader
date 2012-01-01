#ifndef _WORDPLUGIN_
#define _WORDPLUGIN_

#include <QtCore/QString>
#include "page.h"

class WordPlugin
{
  public:

    virtual void handleWord(QString, Page *) = 0;
    
};

class WhitakerPlugin
{
  public:

    virtual void handleWord(QString, Page *);

  protected:

    
};

#endif
