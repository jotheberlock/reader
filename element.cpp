#include "element.h"
#include <stdio.h>

#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

QRect ParagraphElement::size(int w, int downpage, int pageheight)
{
    int xx = 20;
    int yy = 0;
    int pos = downpage;
    
    int linespacing = 0;
    
    for (int fcount=0; fcount<fragments.size(); fcount++)
    {
        StringFragment sf = fragments.at(fcount);
        font.setItalic(sf.is_italic);
        font.setBold(true /*sf.is_bold*/);
        
        QFontMetrics qfm(font);
        qfm = QFontMetrics(font);
        
        int space_width = qfm.averageCharWidth();
        linespacing = qfm.lineSpacing();
        
        for (int count=0; count<sf.text.size(); count++)
        {
            QString str = sf.text.at(count);
            QRect rect = qfm.boundingRect(str);

            if (xx + rect.width() > w)
            {
                xx = 0;
                yy += qfm.lineSpacing();
                pos += qfm.lineSpacing();

                    /*
                if (pos + qfm.lineSpacing() > pageheight)
                {
                    yy += (pageheight - pos);
                    pos = 0;
                    }
                    */       
            }

            xx += rect.width();
            xx += space_width;
        }
    }

    return QRect(0,0,w,yy+linespacing);
}

bool ParagraphElement::render(QPaintDevice * d, int x,int y, int w, int h,
                              int & dropout)
{
    int xx = x + 20;
    int yy = y;
    
    QPainter p;
    p.begin(d);

    int linespacing = 0;
    
    for (int fcount=0; fcount<fragments.size(); fcount++)
    {
        StringFragment sf = fragments.at(fcount);
        font.setItalic(sf.is_italic);
        font.setBold(sf.is_bold);

        QFontMetrics qfm(font);
        qfm = QFontMetrics(font);
        p.setFont(font);
        
        int space_width = qfm.averageCharWidth();
        linespacing = qfm.lineSpacing();
        
        for (int count=0; count<sf.text.size(); count++)
        {
            QString str = sf.text.at(count);
            QRect rect = qfm.boundingRect(str);

            if (xx + rect.width() > w)
            {
                xx = x;
                yy += qfm.lineSpacing();

                    /*
                if (yy + qfm.lineSpacing() > h)
                {
                    dropout = yy - y;
                    return true;
                    }
                    */      
            }

            p.drawText(xx, yy+qfm.ascent(), str);
            xx += rect.width();
            xx += space_width;
        }
    }
    
    p.end();
    return false;
}

QRect PictureElement::size(int w, int downpage, int pageheight)
{
    double scale = ((double)w) / ((double)pixmap.width());

    qDebug(">> Scale %f %f %f", scale, ((double)w), ((double)pixmap.width()));
    
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

QRect PagebreakElement::size(int w, int downpage, int pageheight)
{
    QRect r;
    r.setWidth(w);
    r.setHeight(pageheight-downpage);
    return r;
}
