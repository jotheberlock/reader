#ifndef _BOOKSHELF_
#define _BOOKSHELF_

#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QDir>

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

    QDir getSaveDirectory();
    void setSaveDirectory(QString s) { save_path = s; }
    
  signals:

    void shelfChanged();
    
    public slots:

    void dirChange(const QString &);
    
  protected:
    
    void scanDirectory(QString);
    QList<Mobi *> books;
    QList<QString> directories;
    QFileSystemWatcher * watcher;
    QString save_path;
    
};

extern Bookshelf * bookshelf;

#endif
