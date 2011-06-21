#include <stdio.h>

#include "page.h"
#include "mobi.h"

// #define DEBUG_LAYOUT

Page::Page(Mobi * m)
{
    mobi = m;
    
    QImage image;
    image = QImage::fromData(mobi->readBlock(mobi->firstImage()),"GIF");
    if (!image.isNull())
    {
        printf("Got image\n");
    }
    else
    {
        printf("No image\n");
    }

    start_y = 0;
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
    int sensitive_zone = width() / 10;
    if (event->x() < sensitive_zone)
    {
        previousPage();
    }
    else if (event->x() > width() - sensitive_zone)
    {
        nextPage();
    }
}

void Page::layoutElements()
{
    int y = start_y;

#ifdef DEBUG_LAYOUT    
    printf("Start y %d\n", y);
#endif
    
    for (int loopc=0;loopc<elements.size();loopc++)
    {
        Element * e = elements.at(loopc);
        QRect size = e->size(width());

        int dropout = 0;
        
        e->render(this, 0, y, width(), height(), dropout);
        
#ifdef DEBUG_LAYOUT    
        printf("At %d widget height %d item height %d dropout %d\n", y,
               height(), size.height(), dropout);
#endif        
        if (y+size.height() > height())
        {
            int dropdiff = (height()-y) - dropout;
            end_y = -start_y + (height() - dropdiff);
            
#ifdef DEBUG_LAYOUT    
            printf("Stopped rendering at %d, dropout %d, dropdiff %d, end_y %d\n",
                   y, dropout, dropdiff, end_y);
#endif
            break;
        }
        else
        {
            y += size.height();
        }
    }
}

void Page::nextPage()
{
#ifdef DEBUG_LAYOUT
    printf("\n\nNext page\n\n");
#endif
    start_y = -end_y;
    update();
}

void Page::previousPage()
{
}

