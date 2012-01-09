#include <stdio.h>
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>

#include "element.h"
#include "page.h"

qint64 ParagraphElement::height()
{
    if (cached_height > -1)
    {
        return cached_height;
    }

    words.clear();
    cached_height = 0;
    
    qint64 xpos = page->getIndent() + page->getMargin();
    qint64 ypos = current_position;

    qint64 page_end = ypos;
    page_end = page_end / page->getPageHeight();
    page_end = page_end * page->getPageHeight();
    page_end += page->getPageHeight();

    qint64 line_length = 0;
    
    for (int loopc=0; loopc<fragments.size(); loopc++)
    {
        StringFragment & sf = fragments[loopc];
        QFont metrics(page->getFont());
        metrics.setPointSize(page->getFontSize());
        metrics.setItalic(sf.is_italic);
        metrics.setBold(sf.is_bold);
        QFontMetrics qfm(metrics);
        for (int loopc2=0; loopc2<sf.text.size(); loopc2++)
        {
            Word word;
            word.text = sf.text[loopc2];
            word.lx = xpos;
            word.ly = ypos;
            word.w = qfm.width(word.text);
            word.h = qfm.height();

            qint64 adv = word.w + qfm.width(" ");

                /*
            printf("%lld %lld %lld %lld [%s]\n", xpos, ypos, adv,
                   (page->getPageWidth() - page->getMargin()),
                   word.text.toAscii().data());
                */

            line_length = qfm.lineSpacing() + page->getLeading();
            
            if ( (xpos + adv) > (page->getPageWidth() - page->getMargin()))
            {
                xpos = page->getMargin();
                ypos += line_length;
                cached_height += line_length;
            }

            if ( (ypos + line_length) > page_end)
            {
                    // Text drawn here would clip
                qDebug("Paragraph %lld skipping to next page", element_number);
                cached_height += (page_end - ypos);
                ypos = page_end;
                page_end += page->getPageHeight();
            }
            
            word.x = xpos;
            word.y = ypos+qfm.ascent();
            xpos += adv;
            words.push_back(word);
        }
    }

    cached_height += line_length;
    return cached_height;
}

void ParagraphElement::render(qint64 offset)
{
    QFont f;
    f.setFamily(page->getFont());
    f.setPointSize(page->getFontSize());
    QPainter p(page);
    
    for (int loopc=0; loopc<words.size(); loopc++)
    {
        Word & word = words[loopc];
        f.setItalic(word.is_italic);
        f.setBold(word.is_bold);
        p.setFont(f);
        p.drawText(word.x, word.y+offset, word.text);
    }    
}

QString ParagraphElement::hitTest(qint64 x, qint64 y)
{
    height();  // Ensure word list is built - it should be but no harm in being safe
    for (int loopc=0; loopc<words.size(); loopc++)
    {
        Word & word = words[loopc];
        if (x>=word.lx && y>=word.ly &&
            x<=(word.lx+word.w) && y<=(word.ly+word.h))
        {
            return word.text;
        }
    }

    return "";
}

qint64 PictureElement::height()
{
    double sw = pixmap.width();
    double dw = page->getPageWidth();
    double multi = dw/sw;
    
    return (pixmap.height() * multi);
}

void PictureElement::render(qint64 offset)
{
    double sw = pixmap.width();
    double dw = page->getPageWidth();
    double multi = dw/sw;
    
    QPainter painter(page);
    QRect target(0, current_position+offset, page->getPageWidth(), pixmap.height() * multi);
    QRect source(0,0,pixmap.width(), pixmap.height());
    painter.drawPixmap(target, pixmap, source);
}

qint64 PagebreakElement::height()
{
    qint64 ph = page->getPageHeight();
    if ( (current_position % ph) == 0)
    {
        qDebug("Null pagebreak");
        return 0;
    }
    else
    {
        qDebug("Pagebreak %lld %lld %lld",
               ph, current_position, ph - (current_position %  ph));
        return ph - (current_position % ph);
    } 
}
