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

    elements.push_back(new PictureElement(image));

    elements.push_back(new ParagraphElement("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin tincidunt, lacus sit amet adipiscing ullamcorper, neque quam iaculis ante, at molestie orci purus sed diam. Suspendisse iaculis turpis et libero pellentesque at vehicula dolor ornare. Praesent vitae diam at sapien suscipit adipiscing. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce justo orci, sollicitudin vel vestibulum eu, gravida at nibh. Quisque ut urna eget dolor pulvinar semper nec a magna. Cras vitae risus purus, nec malesuada leo. Donec vitae scelerisque diam. Suspendisse orci ligula, vulputate feugiat ullamcorper vel, fringilla et magna. Nam eget lorem sapien, sed laoreet diam. Nunc faucibus commodo rhoncus. Ut non mauris eu felis ornare tristique vel et lorem. "));
     elements.push_back(new ParagraphElement("Donec eget felis eu erat laoreet tempus. Phasellus euismod massa sed turpis ullamcorper vehicula. Pellentesque massa eros, iaculis imperdiet tincidunt ut, sagittis a sapien. Suspendisse a laoreet ipsum. Nullam rutrum turpis sed elit varius id tincidunt mauris tincidunt. Quisque pharetra dui et arcu commodo pharetra. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Aliquam a tellus ac orci faucibus sagittis. Cras tincidunt laoreet erat vitae placerat. Nulla nulla diam, fermentum mollis tincidunt sit amet, porta vitae nisi. Nam eu purus nisl. Duis sagittis dui sed mauris fringilla et euismod ligula pulvinar. Donec adipiscing, mi sed aliquet vestibulum, nunc erat consectetur augue, ac volutpat leo felis congue mi. Suspendisse vitae tincidunt purus. Nullam ut augue ipsum. Maecenas posuere euismod venenatis. Mauris consequat, sapien ac ultrices scelerisque, purus ante adipiscing augue, eu congue leo tellus sed felis. "));
    
    start_y = 0;
    end_y = 0;
    setFont(QFont("Times New Roman", 20));
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

