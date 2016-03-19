#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QApplication>

#include "whitaker.h"
#include "bookshelf.h"

 QDataStream & operator>> ( QDataStream & out, WhitLocation val )
 {
     out >> val.pos;
     out >> val.len;
     return out;
 }

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

         QString dpath = bookshelf->getFile("whitdefinitions.cal");
         if (dpath == "")
         {
             qDebug("Can't open definitions file");
             return "";
         }
         QFile definitions(dpath);
         definitions.open(QIODevice::ReadOnly);
         qDebug("Matching [%s] with %lld %lld",
                word.toUtf8().data(), wl.pos, wl.len);

         definitions.seek(wl.pos);
         QByteArray data = definitions.read(wl.len);
         QString ret(data);
         return ret;
     }
     else
     {
         qDebug("Can't find [%s]", word.toUtf8().data());
         return "Unknown";
     }
 }

 void WhitakerDictionary::doLoad()
 {
         /*
     QFile index(docs_path+"/indexhash.cal");
     if (!index.open(QIODevice::ReadOnly))
     {
         qDebug("Can't open indexhash.cal");
         return;
     }
     QDataStream qds(&index);
     qds >> words;
     */
     QString ipath = bookshelf->getFile("whitindex.cal");
     if (ipath == "")
     {
         qDebug("Can't open definitions file");
         return;
     }

     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
          
     QFile index(ipath);
     index.open(QIODevice::ReadOnly);
     QTextStream qds(&index);
     while (!qds.atEnd())
     {
         QString word,pos,len;
         word = qds.readLine();
         pos = qds.readLine();
         len = qds.readLine();
         word.remove("\n");
         WhitLocation wl;
         wl.pos = pos.toLongLong();
         wl.len = len.toLongLong();
         words[word] = wl;
     }

     loaded = true;
     QApplication::restoreOverrideCursor();
}

