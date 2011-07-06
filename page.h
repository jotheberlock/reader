#ifndef _PAGE_
#define _PAGE_

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>

#include "element.h"

class Mobi;
class Parser;

class Page : public QWidget
{
  public:

    Page(Mobi *, Parser *);

    virtual void mousePressEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

    void addElement(Element * e)
    {
        elements.push_back(e);
    }
    
  protected:

    void nextPage();
    void previousPage();
    
    void layoutElements();
    QList<Element *> elements;

    int start_y;
    int end_y;
    Mobi * mobi;
    Parser * parser;
    
};

#endif
