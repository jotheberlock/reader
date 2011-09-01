#include <QtCore/QCoreApplication>

#include "shelfscreen.h"
#include "page.h"
#include "bookdevice.h"
#include "parser.h"

Shelfscreen::Shelfscreen()
{
    quit_button = new QPushButton("Quit");
    connect(quit_button, SIGNAL(clicked()), this, SLOT(quitSlot()));
    layout = new QGridLayout(this);
    current_page = 0;
}

void Shelfscreen::quitSlot()
{
    QCoreApplication::quit();
}

void Shelfscreen::readSlot()
{
    for (int loopc=0; loopc<elements.size(); loopc++)
    {
        ShelfElement & se = elements[loopc];
        if(se.text == sender())
        {
            se.book->open();
            settings->setValue("currentbook", se.book->getFullName());
            settings->setValue("currentpage", 0);
            settings->sync();
            BookDevice * bd = new BookDevice(se.book);
            bd->open(QIODevice::ReadOnly);
            Parser * parser = new Parser(bd, se.book);
            Page * page = new Page(se.book, parser);
            top_level->addWidget(page);
            page->setPage(0);
            top_level->setCurrentWidget(page);
            current_page = page;
        }
    }   
}

bool Shelfscreen::readBook(QString book)
{
    for (int loopc=0; loopc<elements.size(); loopc++)
    {
        ShelfElement & se = elements[loopc];
        if(se.book->getFullName() == book)
        {
            se.book->open();
            settings->setValue("currentbook", book);
            settings->setValue("currentpage", 0);
            settings->sync();
            BookDevice * bd = new BookDevice(se.book);
            bd->open(QIODevice::ReadOnly);
            Parser * parser = new Parser(bd, se.book);
            Page * page = new Page(se.book, parser);
            top_level->addWidget(page);
            page->setPage(0);
            top_level->setCurrentWidget(page);
            current_page = page;
            return true;
        }
    }
    
    return false;  
}

void Shelfscreen::update()
{
    for (int loopc=0; loopc<bookshelf->numBooks(); loopc++)
    {
        Mobi * mobi = bookshelf->getBook(loopc);
        
        ShelfElement element;
        
        element.text = new QPushButton(mobi->getFullName());
        connect(element.text, SIGNAL(clicked()), this, SLOT(readSlot()));
        element.image = new QLabel();
        QImage qi = mobi->bookCover();
        qi = qi.scaled(120, 120, Qt::KeepAspectRatio);
        element.image->setPixmap(QPixmap::fromImage(qi));
        layout->addWidget(element.image, loopc, 0);
        layout->addWidget(element.text, loopc, 1);
        layout->setRowStretch(loopc, 1);
        element.book = mobi;
        elements.push_back(element);
    }

    layout->addWidget(quit_button, bookshelf->numBooks(), 0, 1, 2);
}
