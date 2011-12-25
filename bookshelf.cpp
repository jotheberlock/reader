#include "bookshelf.h"

#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtGui/QApplication>

void Bookshelf::scanDirectory(QString d)
{
    QDir dir(d);
    if (!dir.exists())
    {
        return;
    }

    QStringList entries = dir.entryList(QDir::Files | QDir::Readable);

    for (int loopc=0; loopc<entries.size(); loopc++)
    {
        QString fname = dir.absoluteFilePath(entries[loopc]);
        QFile * file = new QFile(fname);
            
        Mobi * mobi = new Mobi;
        if (mobi->sniff(file))
        {
            qDebug("Adding [%s]", fname.toAscii().data());
            books.push_back(mobi);
        }
        else
        {
            qDebug("Can't read [%s]", fname.toAscii().data());
            delete mobi;
            delete file;
        }
    }

        // Check for stylesheet
    QFile stylefile(dir.absoluteFilePath("calliope.css"));
    if (stylefile.exists())
    {
        if (stylefile.open(QIODevice::ReadOnly))
        {
            qDebug("Applying stylesheet");
            QTextStream in(&stylefile);
            QString sheet = in.readAll();
            stylefile.close();
            qApp->setStyleSheet(sheet);
        }
    }
    
}

Mobi * Bookshelf::getBook(int index)
{
    return books[index];
}

int Bookshelf::numBooks()
{
    return books.size();
}
