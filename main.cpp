#include <QtCore/QFile>
#include <QtCore/QBuffer>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QScrollArea>

#include "mobi.h"
#include "page.h"
#include "parser.h"
#include "bookdevice.h"
#include "bookshelf.h"
#include "shelfscreen.h"

Bookshelf * bookshelf;
QStackedWidget * top_level;

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

        /*
    QFile file(argc > 1 ? argv[1] : "../Behemoth.prc");
    file.open(QIODevice::ReadOnly);
    
    Mobi mobi;
    if (!mobi.sniff(&file))
    {
        mobi.open();
    }

    BookDevice bd(&mobi);
    bd.open(QIODevice::ReadOnly);
    
    Parser parser(&bd, mobi.encoding());
        */
        /*
    char buf[4096];
    strcpy(buf, "<p>Here is some <i>italic</i> text and some <b>bold</b></p>");
    QByteArray qba(buf, strlen(buf));
    QBuffer buffer(&qba);
    buffer.open(QIODevice::ReadOnly);
    Parser parser(&buffer, 1);
        */
    
        //Page page(&mobi, &parser);

        /*
    ParagraphElement pe;
    pe.addFragment(StringFragment("Hi here is some text hello"));
    pe.addFragment(StringFragment(" and here is more", true, true));
    page.addElement(&pe);
        */
    
        //page.show();

    bookshelf = new Bookshelf;

    QString docs_path;
    docs_path = QDir::homePath() + QDir::separator() + "Documents";
    bookshelf->scanDirectory(docs_path);
    bookshelf->scanDirectory("/sdcard/kindle");
    
    Shelfscreen * shelfscreen = new Shelfscreen;
    shelfscreen->update();
    shelfscreen->resize(shelfscreen->minimumSize());
    QScrollArea * qsa = new QScrollArea;
    qsa->setWidget(shelfscreen);

    app.setStyleSheet("QPushButton { text-align: left; }");
     
    top_level = new QStackedWidget;
    top_level->addWidget(qsa);
    top_level->show();

    app.exec();
}
