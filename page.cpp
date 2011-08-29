#include <stdio.h>

#include <QtCore/QFile>

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
    highest_y = 0;
    
    buttonbar = new PageButtonBar(this);
    buttonbar->hide();
}

void Page::paintEvent(QPaintEvent *)
{
    layoutElements();
}

void Page::resizeEvent(QResizeEvent *)
{
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
    int y = -(current_page * height())+highest_y;
    int current_y = y;
    
#ifdef DEBUG_LAYOUT    
    qDebug("Start y %d", y);
#endif
    
    int loopc = 0;
    
    while (true)
    {
        while (loopc >= elements.size())
        {
            Element * tmp = parser->next();
            if (tmp == 0)
            {
                return;
            }

            elements.push_back(tmp);
        }
        
        Element * e = elements.at(loopc);
        
        QRect size = e->size(width(), current_y, height());
        
        if (current_y + size.height() < 0)
        {
                // Not visible on screen
            elements.removeFirst();
            current_y += size.height();
            highest_y = e->position();
            delete e;
            continue;
        }
        int dropout = 0;

#ifdef DEBUG_LAYOUT
        qDebug("Render %d %d %d", current_y, width(), height());
#endif
        
        e->render(this, 0, current_y, width(), height(), dropout);
        e->setPosition(current_y);
            
#ifdef DEBUG_LAYOUT    
        qDebug("At %d widget height %d item height %d dropout %d\n", current_y,
               height(), size.height(), dropout);
#endif        
        if (current_y+size.height() > height())
        {
            int dropdiff = (height()-y) - dropout;
            
#ifdef DEBUG_LAYOUT    
            qDebug("Stopped rendering at %d, height was %d, dropout %d, dropdiff %d",
                   y, size.height(), dropout, dropdiff);
#endif
            break;
        }
        else
        {
            current_y += size.height();
        }

        loopc++;
    }
}

void Page::nextPage()
{
#ifdef DEBUG_LAYOUT
    qDebug("\nNext page\n");
#endif
    current_page++;
    settings->setValue("currentpage", current_page);
    settings->sync();
    update();
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
    for (int loopc=0; loopc<elements.size(); loopc++)
    {
        delete elements[loopc];
    }
    elements.clear();
    
    parser->reset();
    
    current_page = p;
    highest_y = 0;
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
