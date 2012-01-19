#include <stdio.h>

#include <QtCore/QFile>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include "page.h"
#include "mobi.h"
#include "parser.h"
#include "shelfscreen.h"
#include "bookdevice.h"
#include "filterpicker.h"
#include "settings.h"
#include "settingsscreen.h"

// #define DEBUG_LAYOUT

Page::Page(Mobi * m, Parser * p)
{
    mobi = m;
    parser = p;
    
    QImage image;
    image = QImage::fromData(mobi->readBlock(mobi->firstImage()),"GIF");
    if (!image.isNull())
    {
        qDebug("Got image");
    }
    else
    {
        qDebug("No image");
    }

    current_page = 0;
    next_y = 0;
    
    buttonbar = new QToolBar(this);
    buttonbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QAction * back_action = new QAction(QIcon(":/images/back.png"), "Back", this);
    QAction * dump_action = new QAction(QIcon(":/images/dump.png"), "Dump", this);
    QAction * bigger_action = new QAction(QIcon(":/images/bigger.png"), "Bigger", this);
    QAction * smaller_action = new QAction(QIcon(":/images/smaller.png"), "Smaller", this);
    QAction * filters_action = new QAction(QIcon(":/images/filters.png"), "Filters", this);
    QAction * settings_action = new QAction(QIcon(":/images/settings.png"), "Settings", this);
    QAction * quit_action = new QAction(QIcon(":/images/quit.png"), "Quit", this);

        // Add in order of usefulness
    buttonbar->addAction(back_action);
    buttonbar->addAction(quit_action);
    buttonbar->addAction(settings_action);
    buttonbar->addAction(filters_action);
    buttonbar->addAction(bigger_action);
    buttonbar->addAction(smaller_action);
    buttonbar->addAction(dump_action);
    
#if !defined(Q_OS_ANDROID)
    menubar = new QMenuBar(this);
    QMenu * menu = menubar->addMenu("&Menu");
    menu->addAction(back_action);
    menu->addAction(dump_action);
    menu->addAction(bigger_action);
    menu->addAction(smaller_action);
    menu->addAction(filters_action);
    menu->addAction(settings_action);
    menu->addAction(quit_action);
    menubar->setAutoFillBackground(true);
#else
    menubar = 0;
#endif
    connect(back_action, SIGNAL(triggered()), this, SLOT(backPushed()));
    connect(dump_action, SIGNAL(triggered()), this, SLOT(dumpPushed()));
    connect(bigger_action, SIGNAL(triggered()), this, SLOT(biggerPushed()));
    connect(smaller_action, SIGNAL(triggered()), this, SLOT(smallerPushed()));
    connect(filters_action, SIGNAL(triggered()), this, SLOT(filtersPushed()));
    connect(settings_action, SIGNAL(triggered()), this, SLOT(settingsPushed()));
    connect(quit_action, SIGNAL(triggered()), this, SLOT(quitPushed()));
    buttonbar->setAutoFillBackground(true);
#if defined(Q_OS_ANDROID)
    buttonbar->hide();
#else    
    buttonbar->show();
    buttonbar->setGeometry(0,menubar->height(),width(),buttonbar->height());
#endif
    getSettings();
 
    QList<Filter *> filters = filter_manager->getFilters();
    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        if (filters[loopc]->getActive())
        {
            filters[loopc]->newBook(this);
        }
    }   
}

Page::~Page()
{
    for (int loopc=0; loopc<elements.size(); loopc++)
    {
        delete elements[loopc];
    }

    mobi->close();
    delete parser;
}

void Page::paintEvent(QPaintEvent *)
{
    layoutElements();
}

void Page::displayMessage(QString caption, QString message)
{
    QMessageBox qbm;
    qbm.setWindowTitle(caption);
    qbm.setText(message);
    qbm.setStandardButtons(QMessageBox::Ok);
    qbm.setDefaultButton(QMessageBox::Ok);
    top_level->addWidget(&qbm);
    top_level->setCurrentWidget(&qbm);
    qbm.exec();
}

void Page::reflow()
{
    clearElements();
    findElements();
    update();
}

void Page::getSettings()
{
    fontsize = settings->getFontSize();
    margin = settings->getMargin();
    indent = settings->getIndent();
    font = settings->getFont();
    leading = settings->getLeading();
}

void Page::resizeEvent(QResizeEvent *)
{
    if (buttonbar->isVisible())
    {
#if defined(Q_OS_ANDROID)        
        int h = buttonbar->sizeHint().height();
        buttonbar->setGeometry(0, height() - h, width(), h);
#else
        buttonbar->setGeometry(0,menubar->height(),width(),buttonbar->height());
#endif
    }

    if (menubar)
    {
        menubar->setGeometry(0, 0, width(), menubar->height());
    }

    reflow();
}

void Page::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->x() < margin)
    {
        previousPage();
    }
    else if (event->x() > width() - margin)
    {
        nextPage();
    }
    else
    {
        mouseFindElement(event->x(), event->y());
    } 
}

void Page::mouseFindElement(qint64 x, qint64 y)
{
        // Convert y to logical units
    y -= pageStart();
    y += (current_page * pageHeight());
    for (int loopc=0; loopc<elements.size(); loopc++)
    {
        ParagraphElement * e = dynamic_cast<ParagraphElement *>(elements[loopc]);
        if (!e)
        {
            continue;
        }
        
        if (y >= e->position() && y <= (e->position() + e->height()))
        {
            Filter * f = filter_manager->getActiveTouchFilter();
            if (f)
            {
                f->onRelease(e, this, x, y);
            }
        } 
    }
}

void Page::layoutElements()
{
    qint64 top_y = -(current_page * pageHeight());
    top_y += pageStart();

#ifdef DEBUG_LAYOUT    
    qDebug("Layout out elements, top_y %lld", top_y);
#endif

    for (int loopc=0; loopc<elements.size(); loopc++)
    {
        Element * e = elements[loopc];
#ifdef DEBUG_LAYOUT
        qDebug("Rendering %lld %lld (%lld) %lld", e->number(), e->position(),
               top_y + e->position(), e->height());

        QPainter p(this);

        QColor col = loopc % 2 ? QColor(255,200,200) : QColor(200,255,200);
        p.fillRect(0, top_y + e->position(), width(), e->height(), col);
#endif
            // Render happens in real coordinates
        e->render(top_y);
    }
}

void Page::clearElements()
{
    for (int loopc=0; loopc<elements.size(); loopc++)
    {
        delete elements[loopc];
    }
    elements.clear();
    next_y = 0;
}

void Page::findElements()
{    
#ifdef DEBUG_LAYOUT
    qDebug("\nFinding elements, page %lld", current_page);
#endif
    
    qint64 top_y = current_page * pageHeight();
    
    bool keep_going = false;
    
#ifdef DEBUG_LAYOUT
    qDebug("Top y is %lld height %d", top_y, pageHeight());
#endif
    
    do 
    {
        keep_going = false;
            // Remove anything offscreen
        for (int loopc=0; loopc<elements.size(); loopc++)
        {
            Element * e = elements[loopc];
            if (e->position() + e->height() < top_y)
            {
#ifdef DEBUG_LAYOUT
                qDebug("Purging before element at %lld %lld",
                       e->position(), e->height());
#endif
                delete e;
                elements.removeAt(loopc);
                keep_going = true;
                break;
            }
            else if (e->position() > top_y + pageHeight())
            {
#ifdef DEBUG_LAYOUT
                qDebug("Purging after element at %lld %lld",
                       e->position(), e->height());
#endif
                    /*
                delete e;
                elements.removeAt(loopc);
                keep_going = true;
                break;
                    */
            }
        }
    }
    while (keep_going);

        // Verify that first element stored is before the page
    if (elements.size() == 0 || elements[0]->position() > top_y)
    {
#ifdef DEBUG_LAYOUT
        qDebug("Resetting and finding first element");
#endif
        clearElements();
        
            // Need to read up to that point
        parser->reset();
        qint64 track_y = 0;
        while (true)
        {
            Element * tmp = parser->next();
            if (tmp == 0)
            {
#ifdef DEBUG_LAYOUT
                qDebug("Ran out of elements finding first element");
#endif
                return;
            }

            tmp->configure(track_y, this);
            
            qint64 e_height = tmp->height();
            
#ifdef DEBUG_LAYOUT
            qDebug("Element is %lld %lld, top_y %lld", track_y, e_height,
                   top_y);
#endif
            
            if(track_y+e_height > top_y)
            {
#ifdef DEBUG_LAYOUT
                qDebug("Found first element %lld %lld", track_y, e_height);
#endif
                    // Reached first visible element, time to bail
                next_y = track_y + e_height;
                elements.push_back(tmp);
                break;
            }
            else
            {
                track_y += e_height;
                delete tmp;
            }
        }
    }

        // Now keep reading elements til we go offscreen
    
    qint64 track_y = next_y;
    while (true)
    {
        Element * tmp = parser->next();
        if (tmp == 0)
        {
            qDebug("Ran out of elements");
            next_y = track_y;
            return;
        }

        tmp->configure(track_y, this);
        qint64 e_height = tmp->height();
        track_y += e_height;

        elements.push_back(tmp);
#ifdef DEBUG_LAYOUT
        qDebug("Read new element %lld %lld", tmp->position(), tmp->height());
        tmp->dump();
#endif
        
        if(track_y > top_y + pageHeight())
        {
#ifdef DEBUG_LAYOUT
            qDebug("Bailing, %lld > %lld + %d, with element %lld", track_y, top_y, height(), tmp->number());
#endif
                // Reached last visible element, time to bail
            next_y = track_y;
            return;
        }
    }
}

void Page::nextPage()
{
    setPage(current_page+1);
}

void Page::previousPage()
{
    if (current_page > 0)
    {
        setPage(current_page-1);
    }
}

void Page::setPage(qint64 p)
{
#ifdef DEBUG_LAYOUT
    printf("Setting page to %lld\n", p);
#endif
    current_page = p;
    findElements();
    settings->setCurrentPage(current_page);
    settings->save();
    update();
}

void Page::backPushed()
{
    QList<Filter *> filters = filter_manager->getFilters();
    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        if (filters[loopc]->getActive())
        {
            filters[loopc]->endBook(this);
        }
    }
    
    settings->setCurrentBook("");
    settings->setCurrentPage(0);
    top_level->setCurrentIndex(0);
    top_level->removeWidget(this);
    delete this;
}

void Page::dumpPushed()
{
    QFile dump_file("bookdump.html");
    if (!dump_file.open(QIODevice::WriteOnly))
    {
        qWarning("Could not open dump file");
        return;
    }
    BookDevice bd(mobi);
    bd.open(QIODevice::ReadOnly);

    QByteArray bytes = bd.readAll();
    dump_file.write(bytes);
    dump_file.close();
}

void Page::biggerPushed()
{
    fontsize += 2;
    settings->setFontSize(fontsize);
    reflow();
}

void Page::smallerPushed()
{
    if (fontsize > 2)
    {
        fontsize -= 2;
    }
    settings->setFontSize(fontsize);
    reflow();
}

void Page::filtersPushed()
{
    FilterPicker * fp = new FilterPicker(this);
    top_level->addWidget(fp);
    top_level->setCurrentWidget(fp);
}

void Page::settingsPushed()
{
    SettingsScreen * ss = new SettingsScreen(this);
    top_level->addWidget(ss);
    top_level->setCurrentWidget(ss);
}

void Page::quitPushed()
{
    QApplication::quit();
}

void Page::menuPushed()
{
    if (buttonbar->isVisible())
    {
        buttonbar->hide();
    }
    else
    {
        int h = buttonbar->sizeHint().height();
        buttonbar->setGeometry(0, height() - h, width(), h);
        buttonbar->show();
        buttonbar->raise();
    }
}

int Page::pageStart()
{
    int ret = menubar ? menubar->height() : 0;
#if !defined(Q_OS_ANDROID)
    ret += buttonbar->height();
#endif
    return ret;
    return menubar ? menubar->height() : 0;
}

int Page::pageHeight()
{
    int ret = height();
    ret -= menubar ? menubar->height() : 0;
#if !defined(Q_OS_ANDROID)
    ret -= buttonbar->height();
#endif
    return ret;
}
