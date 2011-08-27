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

    start_y = 0;

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
    int y = start_y;

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

        if (e->pageTerminator())
        {
            break;
        }
        
        QRect size = e->size(width());

        int dropout = 0;
        
        e->render(this, 0, y, width(), height(), dropout);
        
#ifdef DEBUG_LAYOUT    
        qDebug("At %d widget height %d item height %d dropout %d\n", y,
               height(), size.height(), dropout);
#endif        
        if (y+size.height() > height())
        {
            int dropdiff = (height()-y) - dropout;
            end_y = -start_y + (height() - dropdiff);
            
#ifdef DEBUG_LAYOUT    
            qDebug("Stopped rendering at %d, dropout %d, dropdiff %d, end_y %d",
                   y, dropout, dropdiff, end_y);
#endif
            break;
        }
        else
        {
            y += size.height();
        }

        loopc++;
    }
}

void Page::nextPage()
{
#ifdef DEBUG_LAYOUT
    qDebug("\nNext page\n");
#endif
    start_y = -end_y;
    update();
}

void Page::previousPage()
{
}

void Page::backPushed()
{
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
