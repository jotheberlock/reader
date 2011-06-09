#include "element.h"
#include <stdio.h>

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

bool ParagraphElement::render(QPaintDevice * d, QFont font, int x,int y, int w,
                              int h, int & rendered_h)
{
    rendered_h = 0;
    
    int xx = x + 40;
    int yy = y;

    w -= 20;
    
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
            xx = 20;
            yy += qfm.lineSpacing();

            if (yy + qfm.lineSpacing() > h)
            {
                rendered_h = yy-y;
                return true;
            }            
        }

        p.drawText(xx, yy+qfm.ascent(), str);
        xx += rect.width();
        xx += space_width;
    }
    
    p.end();
    rendered_h = yy-y+qfm.lineSpacing();
    return false;
}

