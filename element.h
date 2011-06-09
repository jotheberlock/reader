#ifndef _ELEMENT_
#define _ELEMENT_

#include <QtCore/QStringList>
#include <QtGui/QPaintDevice>
#include <QtGui/QFont>

class Element
{
  public:

    virtual bool render(QPaintDevice *, QFont font, int x,int y, int w, int h,
                        int & rendered_h) = 0;
    
};

class ParagraphElement : public Element
{
  public:

    ParagraphElement(QString t)
    {
        text=t.split(' ');
    }
    
    void setText(QString t) 
    {
        text=t.split(' ');
    }
    
    virtual bool render(QPaintDevice *, QFont font, int x,int y, int w, int h,
                        int & rendered_h);

  protected:

    QStringList text;
    
};

#endif
