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

    for (int loopc=0; loopc<fragments.size(); loopc++)
    {
        StringFragment & sf = fragments[loopc];
        QFont metrics;
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
            
            if ( (xpos + adv) > (page->getPageWidth() - page->getMargin()))
            {
                xpos = page->getMargin();
                ypos += qfm.lineSpacing();
                cached_height += qfm.lineSpacing();
            }
            
            word.x = xpos;
            word.y = ypos+qfm.ascent();
            xpos += adv;
            words.push_back(word);
        }
    }

    return cached_height;
}

void ParagraphElement::render(qint64 offset)
{
    QFont f;
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

qint64 PictureElement::height()
{
        // Acts as pagebreak then whole page
    qint64 ph = page->getPageHeight();    
    return ph + ( ph - (current_position % ph) );
}

void PictureElement::render(qint64 offset)
{
    QPainter painter(page);
    QRect target(0, current_position+offset, page->getPageWidth(), page->getPageHeight());
    QRect source(0,0,pixmap.width(), pixmap.height());
    painter.drawPixmap(target, pixmap, source);
}

qint64 PagebreakElement::height()
{
    qint64 ph = page->getPageHeight();    
    return ph - (current_position % ph);
}
