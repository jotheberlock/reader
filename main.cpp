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
QSettings * settings;

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    QFont font("Droid Sans");
    font.setBold(true);
    app.setFont(font);
    
    settings = new QSettings("Joel Dillon", "Calliope eReader");
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
     
    top_level = new QStackedWidget;
    top_level->addWidget(qsa);
    top_level->show();

    QString cbook = settings->value("currentbook").toString();
    int cpage = settings->value("currentpage").toInt();
    int w = settings->value("width").toInt();
    int h = settings->value("height").toInt();
    if (shelfscreen->readBook(cbook))
    {
        shelfscreen->currentPage()->setPage(cpage);
    }

    if (w > 0 && h > 0)
    {
        top_level->resize(w,h);
    }
    
    app.exec();
}
