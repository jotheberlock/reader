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

    bookshelf = new Bookshelf;

    QString docs_path;
    docs_path = QDir::homePath() + QDir::separator() + "Documents";
    bookshelf->scanDirectory(docs_path);
    bookshelf->scanDirectory("/sdcard/kindle");
    
    Shelfscreen * shelfscreen = new Shelfscreen;
    shelfscreen->update();
    shelfscreen->resize(480,640);
        //shelfscreen->resize(shelfscreen->minimumSize());
    QScrollArea * qsa = new QScrollArea;
    qsa->setWidget(shelfscreen);

    app.setStyleSheet("QPushButton { text-align: left; }");
     
    top_level = new QStackedWidget;
    top_level->addWidget(qsa);
    top_level->show();

    app.exec();
}
