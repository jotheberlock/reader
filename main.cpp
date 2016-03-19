#include <QFile>
#include <QBuffer>
#include <QDir>
#include <QApplication>
#include <QScrollArea>

#include "mobi.h"
#include "page.h"
#include "parser.h"
#include "bookdevice.h"
#include "bookshelf.h"
#include "shelfscreen.h"
#include "filter.h"
#include "dictionaryfilter.h"
#include "whitaker.h"
#include "wordchangerfilter.h"
#include "settings.h"
#include "calliope_application.h"
#include "mainwindow.h"

Bookshelf * bookshelf;
QStackedWidget * top_level;
Settings * settings;

FilterManager * filter_manager;

int main(int argc, char ** argv)
{
    CalliopeApplication app(argc, argv);

        /* For some reason this doesn't work with Android Qt
#if defined(Q_OS_ANDROID
 For usability's sake
 app.setStyleSheet("QScrollBar { width: 50 }");
#endif
        */
    
#if defined(Q_OS_ANDROID)
    app.setGlobalStrut(QSize(50,50));
#endif
    
    settings = new Settings();
    
    filter_manager = new FilterManager();
    filter_manager->addFilter(new WordChangerFilter());
    filter_manager->addFilter(new DictionaryFilter(new WhitakerDictionary()));
    
    bookshelf = new Bookshelf;

    QString docs_path;
    docs_path = QDir::homePath() + QDir::separator();
    bookshelf->addPath(docs_path + "Documents");
    bookshelf->addPath(docs_path + "calliope");
    bookshelf->addPath("/sdcard/kindle");
    bookshelf->addPath("/sdcard/calliope");
    
    Shelfscreen * shelfscreen = new Shelfscreen;
    shelfscreen->updateSlot();
    QObject::connect(bookshelf, SIGNAL(shelfChanged()), shelfscreen, SLOT(updateSlot()));
    
    QScrollArea * qsa = new QScrollArea;
    qsa->setWidgetResizable(true);
    qsa->setWidget(shelfscreen); 
    shelfscreen->update();
    
    top_level = new MainWindow;
    top_level->addWidget(qsa);
    
    settings->setActiveTouch("Word changer");
    Filter * f = filter_manager->getFilter(settings->getActiveTouch());
    filter_manager->setActiveTouchFilter(f);

    qint64 the_page = settings->getCurrentPage();
    if (shelfscreen->readBook(settings->getCurrentBook()))
    {
        shelfscreen->currentPage()->setPage(the_page);
    }
    
    top_level->show();
    
    app.exec();
}
