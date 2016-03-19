#include <stdio.h>
#include <QPainter>
#include <QFontMetrics>

#include "element.h"
#include "page.h"
#include "filter.h"

// #ifdef DEBUG_ELEMENTS

void ParagraphElement::dump()
{
    if (fragments.size() == 0)
    {
        qDebug("Null paragraph element!");
    }
    
    for (int loopc=0; loopc<fragments.size(); loopc++)
    {
        QString out = QString::number(loopc);
        out += ": ";
        StringFragment & sf = fragments[loopc];
        for (int loopc2=0; loopc2<sf.text.size(); loopc2++)
        {
            out += sf.text[loopc2];
            out += " ";
        }
        qDebug("%s", out.toUtf8().data());
    }   
}

qint64 ParagraphElement::height()
{
    if (cached_height > -1)
    {
        return cached_height;
    }

    words.clear();
    cached_height = 0;

        // Apply before filters
    QList<Filter *> filters = filter_manager->getFilters();
    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        if (filters[loopc]->getActive())
        {
            filters[loopc]->beforeLayout(this);
        }
    }
    
    qint64 xpos = page->getIndent() + page->getMargin();
    qint64 ypos = current_position;

    qint64 page_end = ypos;
    page_end = page_end / page->getPageHeight();
    page_end = page_end * page->getPageHeight();
    page_end += page->getPageHeight();

    qint64 line_length = 0;

    int count = 0;
    
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
            word.w = qfm.width(word.text);
            word.h = qfm.height();
            word.index = count;
            count++;
            
            qint64 adv = word.w + qfm.width(" ");

            qDebug("%lld %lld %lld %lld [%s]", xpos, ypos, adv,
                   (page->getPageWidth() - page->getMargin()),
                   word.text.toUtf8().data());
            
            line_length = qfm.lineSpacing() + page->getLeading();
            
            if ( (xpos + adv) > (page->getPageWidth() - page->getMargin()))
            {
                xpos = page->getMargin();
                ypos += line_length;
            }

            if ( (ypos + line_length) > page_end)
            {
                    // Text drawn here would clip
#ifdef DEBUG_ELEMENTS
                qDebug("Paragraph %lld skipping to next page", element_number);
#endif
                ypos = page_end;
                page_end += page->getPageHeight();
            }
            
            word.x = xpos;
            word.y = ypos+qfm.ascent();
            word.lx = xpos;
            word.ly = ypos;
            xpos += adv;
            words.push_back(word);
        }
    }

    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        if (filters[loopc]->getActive())
        {
            filters[loopc]->afterLayout(this);
        }
    }

        // Calculate height, after words have possibly
        // been moved by the above filters
    for (int loopc=0; loopc<words.size(); loopc++)
    {
        Word & word = words[loopc];
        qint64 bottom = (word.ly + word.h) - current_position;
        if (bottom > cached_height)
        {
            cached_height = bottom;
        }
    }

    qDebug("Cached height is %lld", cached_height);
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


Word ParagraphElement::getWord(qint64 x, qint64 y)
{   
    for (int loopc=0; loopc<words.size(); loopc++)
    {
        Word & word = words[loopc];
        if (x>=word.lx && y>=word.ly &&
            x<=(word.lx+word.w) && y<=(word.ly+word.h))
        {
            return words[loopc];
        }
    }

    Word dummy;
    return dummy;
}

qint64 PictureElement::height()
{
    double sw = pixmap.width();
    double dw = page->getPageWidth() - (page->getMargin() * 2);
    double multi = dw/sw;
    
    return (pixmap.height() * multi);
}

void PictureElement::render(qint64 offset)
{
    int margins = page->getMargin() * 2;
    
    double sw = pixmap.width();
    double dw = page->getPageWidth() - margins;
    double multi = dw/sw;
    
    QPainter painter(page);
    QRect target(page->getMargin(),
                 current_position+offset, page->getPageWidth() - margins, pixmap.height() * multi);
    QRect source(0,0,pixmap.width(), pixmap.height());
    painter.drawPixmap(target, pixmap, source);
}

qint64 PagebreakElement::height()
{
    qint64 ph = page->getPageHeight();
    if ( (current_position % ph) == 0)
    {
#ifdef DEBUG_ELEMENTS
        qDebug("Null pagebreak");
#endif
        return 0;
    }
    else
    {
#ifdef DEBUG_ELEMENTS
        qDebug("Pagebreak %lld %lld %lld",
               ph, current_position, ph - (current_position %  ph));
#endif
        return ph - (current_position % ph);
    } 
}
