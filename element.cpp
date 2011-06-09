#include "element.h"
#include <stdio.h>

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

QRect ParagraphElement::size(int w)
{
    int xx = 20;
    int yy = 0;
    
    QFontMetrics qfm(font);
    
    int space_width = qfm.averageCharWidth();

    for (int count=0; count<text.size(); count++)
    {
        QString str = text.at(count);
        QRect rect = qfm.boundingRect(str);

        if (xx + rect.width() > w)
        {
            xx = 0;
            yy += qfm.lineSpacing();
        }

        xx += rect.width();
        xx += space_width;
    }

    return QRect(0,0,w,yy+qfm.lineSpacing());
}

bool ParagraphElement::render(QPaintDevice * d, int x,int y, int w, int h,
                              int & dropout)
{
    int xx = x + 20;
    int yy = y;
    
    QFontMetrics qfm(font);
    
    int space_width = qfm.averageCharWidth();

    QPainter p;
    p.begin(d);
    p.setFont(font);

    for (int count=0; count<text.size(); count++)
    {
        QString str = text.at(count);
        QRect rect = qfm.boundingRect(str);

        if (xx + rect.width() > w)
        {
            xx = x;
            yy += qfm.lineSpacing();

            if (yy + qfm.lineSpacing() > h)
            {
                dropout = yy - y;
                return true;
            }            
        }

        p.drawText(xx, yy+qfm.ascent(), str);
        xx += rect.width();
        xx += space_width;
    }
    
    p.end();
    return false;
}

QRect PictureElement::size(int w)
{
    double scale = ((double)w) / ((double)pixmap.width());
    return QRect(0,0,(int)(scale * pixmap.width()),
                (int)(scale * pixmap.height()));
}

bool PictureElement::render(QPaintDevice * d, int x,int y, int w, int h,
                            int & dropout)
{
    if (pixmap.width() < 1 || pixmap.height() < 1)
    {
        return false;
    }
    
    double scale = ((double)w-x) / ((double)pixmap.width());
    int length = (int)(((double)pixmap.height()) * scale);

    if (y+length > h)
    {
        dropout = h-y;
    }
    
    QPainter p;
    p.begin(d);
    p.drawPixmap(x,y,(w-x),length, pixmap);
    p.end();

    return true;
}

