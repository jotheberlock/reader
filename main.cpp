#include <QtCore/QFile>
#include <QtGui/QApplication>

#include "mobi.h"
#include "page.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    
    QFile file(argc > 1 ? argv[1] : "../Behemoth.prc");
    file.open(QIODevice::ReadOnly);
    
    Mobi mobi;
    mobi.open(&file);

    Page page(&mobi);
    page.show();
    
    app.exec();
}
