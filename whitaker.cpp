#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTextStream>

#include "whitaker.h"

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
     else
     {
         qDebug("Can't find [%s]", word.toAscii().data());
         return "Unknown";
     }
 }

 void WhitakerDictionary::doLoad()
 {
     QString docs_path;
     docs_path = QDir::homePath() + QDir::separator() + "Documents";

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
         
     QFile index(docs_path+"/index.cal");
     if (!index.open(QIODevice::ReadOnly))
     {
         qDebug("Can't open indexhash.cal");
         return;
     }
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
 }

