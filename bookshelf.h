#ifndef _BOOKSHELF_
#define _BOOKSHELF_

#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QFileSystemWatcher>

#include "mobi.h"

class Bookshelf : public QObject
{
    Q_OBJECT
        
  public:

    Bookshelf();
    ~Bookshelf();
    
    void addPath(QString);
    
    Mobi * getBook(int);
    int numBooks();

        // Looks in all directories for a file with a specific name
    QString getFile(QString name);

  signals:

    void shelfChanged();
    
    public slots:

    void dirChange(const QString &);
    
  protected:
    
    void scanDirectory(QString);
    QList<Mobi *> books;
    QList<QString> directories;
    QFileSystemWatcher * watcher;
    
};

extern Bookshelf * bookshelf;

#endif
