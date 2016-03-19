#include "bookshelf.h"

#include <QDir>
#include <QTextStream>
#include <QApplication>

Bookshelf::Bookshelf()
    : QObject(0)
{
    watcher = new QFileSystemWatcher();
    connect(watcher, SIGNAL(directoryChanged(const QString &)), this,
            SLOT(dirChange(const QString &)));
    
#if defined(Q_OS_ANDROID)
    setSaveDirectory("/sdcard/calliope");
#else
    QString docs_path;
    docs_path = QDir::homePath() + QDir::separator();
    setSaveDirectory(docs_path+"calliope");
#endif
}

Bookshelf::~Bookshelf()
{
    delete watcher;
}

void Bookshelf::addPath(QString d)
{
    qDebug("Adding path %s", d.toUtf8().data());
    
    QDir dir(d);
    if (!dir.exists())
    {
        return;
    }
    
    if (!directories.contains(d))
    {
        directories.push_back(d);
        scanDirectory(d);
    }

    watcher->addPath(d);
}

QString Bookshelf::getFile(QString name)
{
    for (int loopc=0; loopc<directories.size(); loopc++)
    {
        QFile ret(directories[loopc]+QDir::separator()+name);
        if (ret.exists() && ret.open(QIODevice::ReadOnly))
        {
            return ret.fileName();
        }   
    }
    return "";
}

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
        if (mobi->sniff(file, entries[loopc]))
        {
            qDebug("Adding [%s]", fname.toUtf8().data());
            books.push_back(mobi);
        }
        else
        {
            qDebug("Can't read [%s]", fname.toUtf8().data());
            delete mobi;
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

void Bookshelf::dirChange(const QString &)
{
    for (int loopc=0; loopc<books.size(); loopc++)
    {
        if (!books[loopc]->isOpen())
        {
            delete books[loopc];
        }
    }
    books.clear();
    for (int loopc=0; loopc<directories.size(); loopc++)
    {
        scanDirectory(directories[loopc]);
    }

    emit shelfChanged();
}

QDir Bookshelf::getSaveDirectory()
{
    QDir save(save_path);
    if (!save.exists())
    {
        save.mkpath(save.path());
    }

    return save;
}

