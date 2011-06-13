#include <QtCore/QFile>
#include <QtCore/QBuffer>
#include <QtGui/QApplication>

#include "mobi.h"
#include "page.h"
#include "parser.h"
#include "bookdevice.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    
    QFile file(argc > 1 ? argv[1] : "../Behemoth.prc");
    file.open(QIODevice::ReadOnly);
    
    Mobi mobi;
    mobi.open(&file);

    BookDevice bd(&mobi);
    bd.open(QIODevice::ReadOnly);
    
        //Parser parser(&bd, mobi.encoding());

    char buf[4096];
    strcpy(buf, "<html>stuff</html>");

    QByteArray qba(buf, strlen(buf));
    QBuffer buffer(&qba);
    buffer.open(QIODevice::ReadOnly);
    Parser parser(&buffer, 1);
                  
    parser.next();
    
        /*
    Page page(&mobi);
    page.show();
        */
    
    app.exec();
}
