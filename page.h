#ifndef _PAGE_
#define _PAGE_

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>

#include "element.h"

class Mobi;
class Parser;
class PageButtonBar;

class Page : public QWidget
{
    Q_OBJECT
        
  public:

    Page(Mobi *, Parser *);

    virtual void mousePressEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

    void addElement(Element * e)
    {
        elements.push_back(e);
    }
    
    void setPage(int p);
    
  public slots:

    void backPushed();
    void dumpPushed();
    
  protected:

    void nextPage();
    void previousPage();
    
    void layoutElements();
    QList<Element *> elements;

    qint64 current_page;
    qint64 highest_y;
    
    Mobi * mobi;
    Parser * parser;
    PageButtonBar * buttonbar;
    
};

#endif
