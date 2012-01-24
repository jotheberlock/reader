#include <QtCore/QHash>
#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtGui/QInputDialog>

#include "bookshelf.h"
#include "wordchangerfilter.h"
#include "mobi.h"
#include "shelfscreen.h"

// Number of seconds until a press becomes 'undo' not 'change'
#define UNDO_DELAY 2

class ChangeRecord
{
public:
    
    QString from;
    QString to;
};
    
class ParagraphRecord
{
public:

        // We use a map because it's an ordered container
    QMap<int, ChangeRecord> changes;
};

    
void WordChangerFilter::newBook(Page * p)
{
     QHash<qint64, ParagraphRecord *>::const_iterator i = changes.constBegin();
     while (i != changes.constEnd())
     {
         delete i.value();
         i++;
     }
     changes.clear();
     bookname = p->getMobi()->getFileName().split('.')[0];
     load();
}

void WordChangerFilter::endBook(Page *)
{
    save();
    bookname = "";
}

void WordChangerFilter::beforeLayout(ParagraphElement * p)
{
    if (!changes.contains(p->number()))
    {
        return;
    }
    
    ParagraphRecord * pr = changes[p->number()];

    QMap<int, ChangeRecord>::const_iterator i = pr->changes.constBegin();
     
    int count = 0;
    QList<StringFragment> & fragments = p->getFragments();
    for (int loopc=0; loopc<fragments.size(); loopc++)
    {
        QStringList & text = fragments[loopc].text;
        for (int loopc2=0; loopc2<text.size(); loopc2++)
        {
            if (i.key() == count)
            {
                const ChangeRecord & cr = i.value();
                if (text[loopc2] == cr.from)
                {
                    text[loopc2] = cr.to;
                }
                else
                {
                    qWarning("Expected to change [%s] to [%s] found [%s]",
                             cr.from.toAscii().data(), cr.to.toAscii().data(),
                             text[loopc2].toAscii().data());
                }
                i++;
                if (i == pr->changes.constEnd())
                {
                        // Nothing left to change
                    return;
                }
            }
            
            QString word = text[loopc2];
            count++;
        }    
    }
}


void WordChangerFilter::onPress(ParagraphElement *, Page *, qint64, qint64)
{
    press_time = QTime::currentTime();
}

void WordChangerFilter::onRelease(ParagraphElement * para, Page * page, qint64 x,
                                  qint64 y)
{
    if (press_time.secsTo(QTime::currentTime()) > UNDO_DELAY)
    {
        if (changes.contains(para->number()))
        {
            ParagraphRecord * pr = changes[para->number()];
            pr->changes.clear();
            changes.remove(para->number());
            delete pr;
            page->reflow();
        }
        return;
    }    
    
    Word w = para->getWord(x,y);
    if (w.text == "")
    {
        return;
    }

    QString newstr = QInputDialog::getText(top_level,
                                           "Changing word",
                                           "Enter new word for '"+w.text+"'");

    if (newstr != "")
    {
        newstr.remove(" ");
    
        ParagraphRecord * pr;
        if (changes[para->number()])
        {
            pr = changes[para->number()];
        }
        else
        {
            pr = new ParagraphRecord;
            changes.insert(para->number(),pr);
        }

        ChangeRecord & cr = pr->changes[w.index];
            // if previous record is there, maintain original word
        if (cr.from == "")
        {
            cr.from = w.text;
        }
    
        cr.to = newstr;
        top_level->activateWindow();
        page->reflow();
    }
}

void WordChangerFilter::save()
{
    if (bookname == "")
    {
        return;
    }
    
    QDir save = bookshelf->getSaveDirectory();
    QFile out(save.path()+QDir::separator()+bookname+".wcf");
    if (!out.open(QIODevice::WriteOnly))
    {
        qWarning("Could not open %s", out.fileName().toAscii().data());
        return;
    }

    QTextStream qts(&out);
    
    QHash<qint64, ParagraphRecord *>::const_iterator i = changes.constBegin();
    while (i != changes.constEnd())
    {
        ParagraphRecord * pr = i.value();
        
        QMap<int, ChangeRecord>::const_iterator j = pr->changes.constBegin();
        while (j != pr->changes.constEnd())
        {
            QString outs = QString::number(i.key());
            outs += " ";
            outs += QString::number(j.key());
            outs += " ";
            outs += j.value().from;
            outs += " ";
            outs += j.value().to;
            outs += "\n";
            qts << outs;
            j++;
        }
        i++;
    }
    out.close();
}

void WordChangerFilter::load()
{
    if (bookname == "")
    {
        return;
    }
    
    QDir save = bookshelf->getSaveDirectory();
    QFile in(save.path()+QDir::separator()+bookname+".wcf");
    if (!in.open(QIODevice::ReadOnly))
    {
        qWarning("Could not open %s", in.fileName().toAscii().data());
        return;
    }

    qint64 current_para = -1;
    ParagraphRecord * pr;
    
    QTextStream qts(&in);
    while (!qts.atEnd())
    {
        QString ins = qts.readLine();
        ins.remove("\n");
        QStringList qsl = ins.split(" ");
        if (qsl.size() != 4)
        {
            qWarning("Odd line [%s]", ins.toAscii().data());
            continue;
        }
        
        bool ok;
        qint64 pnum = qsl[0].toULongLong(&ok);
        if (!ok)
        {
            continue;
        }
        int wnum =  qsl[1].toInt(&ok);
        if (!ok)
        {
            continue;
        }

        if (pnum != current_para)
        {
            if (!changes.contains(pnum))
            {
                pr = new ParagraphRecord;
                changes[pnum] = pr;
            }
            else
            {
                pr = changes[pnum];
            }
            current_para = pnum;
        }

        ChangeRecord & cr = pr->changes[wnum];
        cr.from = qsl[2];
        cr.to = qsl[3];
    }
}
