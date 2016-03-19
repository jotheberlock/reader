#ifndef _PAGE_
#define _PAGE_

#include <QFrame>
#include <QMouseEvent>
#include <QToolBar>
#include <QMenuBar>
#include <QAction>

#include "element.h"

class Mobi;
class Parser;
class MainWindow;

class Page : public QFrame
{
    Q_OBJECT
        
  public:

    Page(Mobi *, Parser *);
    ~Page();
    
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void resizeEvent(QResizeEvent *);
     
    void addElement(Element * e)
    {
        elements.push_back(e);
    }
    
    void setPage(qint64 p);
    void setFontSize(int p) { fontsize=p; }
        
    qint64 getPage() { return current_page; }
    qint64 getPageWidth() { return width(); }
    qint64 getPageHeight() { return pageHeight(); }
    qint64 getMargin() { return margin; }
    qint64 getIndent() { return indent; }
    qint64 getLeading() { return leading; }
    qint64 getFontSize() { return fontsize; }
    QString getFont() { return font; }

    Mobi * getMobi() { return mobi; }
    Parser * getParser() { return parser; }
    
    void mouseFindElement(qint64 x, qint64 y, bool is_press);
    void displayMessage(QString caption, QString message);

    void reflow();
    void getSettings();
    
  public slots:

    void backPushed();
    void dumpPushed();
    void biggerPushed();
    void smallerPushed();
    void filtersPushed();
    void settingsPushed();
    void quitPushed();
    void menuPushed();
    
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
    qint64 margin;
    qint64 indent;
    qint64 fontsize;
    qint64 leading;
    QString font;

    int press_x;
    int press_y;
    
    Mobi * mobi;
    Parser * parser;
    QToolBar * buttonbar;
    QMenuBar * menubar;
    
};

#endif
