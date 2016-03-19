#ifndef _SHELFSCREEN_
#define _SHELFSCREEN_

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>

#include "bookshelf.h"

class Page;

class ShelfElement
{
  public:

    QLabel * image;
    QPushButton * text;
    Mobi * book;
    
};

class Shelfscreen : public QWidget
{
    Q_OBJECT
        
  public:

    Shelfscreen();

    public slots:

    void updateSlot();
    void quitSlot();
    void readSlot();
    bool readBook(QString);

    Page * currentPage()  { return current_page; }
        
    QSize minimumSize() const
    {
        return layout->minimumSize();
    }
    
  protected:

    QGridLayout * layout;
    QList<ShelfElement> elements;
    QPushButton * quit_button;
    Page * current_page;
    
};

extern QStackedWidget * top_level;

#endif
