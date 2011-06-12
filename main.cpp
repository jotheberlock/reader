#include <QtCore/QFile>
#include <QtGui/QApplication>

#include "mobi.h"
#include "page.h"
#include "parser.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    
    QFile file(argc > 1 ? argv[1] : "../Behemoth.prc");
    file.open(QIODevice::ReadOnly);
    
    Mobi mobi;
    mobi.open(&file);

    BookDevice bd(&mobi);
    bd.open(QIODevice::ReadOnly);
    
    Parser parser(&bd, mobi.encoding());
    parser.next();
    
        /*
    Page page(&mobi);
    page.show();
        */
    
    app.exec();
}
