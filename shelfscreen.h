#ifndef _SHELFSCREEN_
#define _SHELFSCREEN_

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>

#include "bookshelf.h"

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
    void update();

    public slots:

    void quitSlot();
    void readSlot();
    
    QSize minimumSize() const
    {
        return layout->minimumSize();
    }
    
  protected:

    QGridLayout * layout;
    QList<ShelfElement> elements;
    QPushButton * quit_button;
    
};

extern QStackedWidget * top_level;

#endif
