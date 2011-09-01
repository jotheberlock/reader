#include <stdio.h>

#include <QtCore/QFile>
#include <QtGui/QPainter>

#include "page.h"
#include "mobi.h"
#include "parser.h"
#include "shelfscreen.h"
#include "pagebuttonbar.h"
#include "bookdevice.h"

#define DEBUG_LAYOUT

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
    
    buttonbar = new PageButtonBar(this);
    buttonbar->hide();
}

void Page::paintEvent(QPaintEvent *)
{
    layoutElements();
}

void Page::resizeEvent(QResizeEvent *)
{
    settings->setValue("width", width());
    settings->setValue("height", height());
    findElements();
    update();
}

void Page::mousePressEvent(QMouseEvent * event)
{
    int sensitive_zone_x = width() / 10;
    int sensitive_zone_y = height() / 10;
    if (event->x() < sensitive_zone_x)
    {
        previousPage();
    }
    else if (event->x() > width() - sensitive_zone_x)
    {
        nextPage();
    }
    else if (event->y() > height() - sensitive_zone_y)
    {
        if (buttonbar->isVisible())
        {
            buttonbar->hide();
        }
        else
        {
            buttonbar->setGeometry(0, height() - sensitive_zone_y,
                                   width(), sensitive_zone_y);
            buttonbar->show();
        }
    }
    else if (buttonbar->isVisible())
    {
        buttonbar->hide();
    }
}

void Page::layoutElements()
{
    qint64 top_y = -(current_page * height());
    
    qDebug("Layout out elements, top_y %lld", top_y);
    
    int dropout = 0;
    for (int loopc=0; loopc<elements.size(); loopc++)
    {
        Element * e = elements[loopc];
        qDebug("Rendering %lld %lld (%lld) %lld", e->number(), e->position(),
               top_y + e->position(), e->height());

        QPainter p(this);
        p.fillRect(0, top_y + e->position(), width(), e->height(),
                   loopc % 2 ? QColor(255,200,200) : QColor(200,255,200));
        e->render(this, 0, top_y + e->position(), width(), height(), dropout);
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
    qDebug("\nFinding elements, page %lld", current_page);
        
    qint64 top_y = current_page * height();

    bool keep_going = false;

    qDebug("Top y is %lld height %d", top_y, height());

    do 
    {
        keep_going = false;
            // Remove anything offscreen
        for (int loopc=0; loopc<elements.size(); loopc++)
        {
            Element * e = elements[loopc];
            if (e->position() + e->height() < top_y)
            {
                qDebug("Purging before element at %lld %lld",
                       e->position(), e->height());
                delete e;
                elements.removeAt(loopc);
                keep_going = true;
                break;
            }
            else if (e->position() > top_y + height())
            {
                qDebug("Purging after element at %lld %lld",
                       e->position(), e->height());
                delete e;
                elements.removeAt(loopc);
                keep_going = true;
                break;
            }
        }
    }
    while (keep_going);
    
        // Verify that first element stored is before the page
    if (elements.size() == 0 || elements[0]->position() > top_y)
    {
        qDebug("Resetting and finding first element");
        clearElements();
        
            // Need to read up to that point
        parser->reset();
        qint64 track_y = 0;
        while (true)
        {
            Element * tmp = parser->next();
            if (tmp == 0)
            {
                qDebug("Ran out of elements finding first element");
                return;
            }

            QRect size = tmp->size(width(), track_y % height(), height());
            
            qDebug("Element is %lld %d, top_y %lld", track_y, size.height(),
                   top_y);
            
            if(track_y+size.height() > top_y)
            {
                qDebug("Found first element %lld %d", track_y, size.height());
                    // Reached first visible element, time to bail
                tmp->setPosition(track_y);
                tmp->setHeight(size.height());
                next_y = track_y + size.height();
                elements.push_back(tmp);
                break;
            }
            else
            {
                track_y += size.height();
                delete tmp;
            }
        }
    }

        // Now keep reading elements til we go offscreen
    
    qint64 track_y = top_y;
    while (true)
    {
        Element * tmp = parser->next();
        if (tmp == 0)
        {
            qDebug("Ran out of elements");
            return;
        }
        
        QRect size = tmp->size(width(), track_y % height(), height());
        tmp->setPosition(track_y);
        tmp->setHeight(size.height());
        track_y += size.height();

        elements.push_back(tmp);
        qDebug("Read new element %lld %lld", tmp->position(), tmp->height());
        
        if(track_y > top_y + height())
        {
            qDebug("Bailing, %lld > %lld + %d", track_y, top_y, height());
                // Reached last visible element, time to bail
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

void Page::setPage(int p)
{   
    current_page = p;
    findElements();
    settings->setValue("currentpage", current_page);
    settings->sync();
    update();
}

void Page::backPushed()
{
    settings->setValue("currentbook", "");
    settings->setValue("currentpage", 0);
    settings->sync();
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
