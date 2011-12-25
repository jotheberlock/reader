#ifndef _PAGE_
#define _PAGE_

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

#include "element.h"

class Mobi;
class Parser;

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

    void mouseFindElement(qint64 x, qint64 y);
    
  public slots:

    void backPushed();
    void dumpPushed();
    
  protected:

    void nextPage();
    void previousPage();
    
    void layoutElements();
    void findElements();
    void clearElements();
    
    int pageStart();
    int pageHeight();

    QList<Element *> elements;

    qint64 current_page;
    qint64 next_y;
    
    Mobi * mobi;
    Parser * parser;
    QToolBar * buttonbar;
    QMenuBar * menubar;

};

#endif
