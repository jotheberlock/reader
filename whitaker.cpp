#include <QtCore/QFile>
#include <QtCore/QDir>

#include "whitaker.h"

QString WhitakerDictionary::lookup(QString word)
{
    if (!loaded)
    {
        doLoad();
    }

    if (words.contains(word))
    {
        QString docs_path;
        docs_path = QDir::homePath() + QDir::separator() + "Documents";
        WhitLocation wl = words[word];
        QFile definitions(docs_path+"/definitions.cal");
        if (!definitions.open(QIODevice::ReadOnly))
        {
            qDebug("Can't open definitions.cal");
            return "";
        }

        qDebug("Matching [%s] with %lld %lld",
               word.toAscii().data(), wl.pos, wl.len);
        
        definitions.seek(wl.pos);
        QByteArray data = definitions.read(wl.len);
        QString ret(data);
        return ret;
    }

    return "";
}

void WhitakerDictionary::doLoad()
{
    QString docs_path;
    docs_path = QDir::homePath() + QDir::separator() + "Documents";
    QFile index(docs_path+"/index.cal");
    if (!index.open(QIODevice::ReadOnly))
    {
        qDebug("Can't open index.cal");
        return;
    }

    while(!index.atEnd())
    {
        WhitLocation wl;
        QString word;
        word = index.readLine();
        QString poss = index.readLine();
        QString lens = index.readLine();

        word.remove("\n");
        wl.pos = poss.toLongLong();
        wl.len = lens.toLongLong();
        words[word] = wl;
    }

    qDebug("Read %d entries", words.size());
    loaded = true;
}

