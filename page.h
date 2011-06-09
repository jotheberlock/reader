#ifndef _PAGE_
#define _PAGE_

#include <QtGui/QWidget>

#include "element.h"

class Page : public QWidget
{
  public:

    Page();

    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    
  protected:

    void layoutElements();
    QList<Element *> elements;

    int start_y;
    int end_y;
    
};

#endif
