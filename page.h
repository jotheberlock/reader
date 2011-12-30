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
    
    void setPage(qint64 p);
    qint64 getPage() { return current_page; }
    qint64 getPageWidth() { return width(); }
    qint64 getPageHeight() { return pageHeight(); }
    qint64 getMargin() { return margin; }
    qint64 getIndent() { return indent; }
    qint64 getFontSize() { return fontsize; }
    Element * getHighlighted() { return highlight; }
    
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
    Element * highlight;
    
    qint64 current_page;
    qint64 next_y;
    qint64 margin;
    qint64 indent;
    qint64 fontsize;
    
    Mobi * mobi;
    Parser * parser;
    QToolBar * buttonbar;
    QMenuBar * menubar;

};

#endif
