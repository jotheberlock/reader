#include <QtGui/QIcon>

#include "mainwindow.h"
#include "settings.h"
#include "page.h"

MainWindow::MainWindow()
    : QStackedWidget(0)
{
    setObjectName("mainwindow");
    move(settings->getX(), settings->getY());
    resize(settings->getWidth(), settings->getHeight());
#if defined(Q_OS_ANDROID)
        setWindowState(windowState() ^ Qt::WindowFullScreen);
#endif        
    setWindowIcon(QIcon(":/images/calliope.png"));
}

void MainWindow::closeEvent(QCloseEvent *)
{
    settings->setX(x());
    settings->setY(y());
    settings->setWidth(width());
    settings->setHeight(height());
    settings->setFullScreen(windowState() | Qt::WindowFullScreen);
}

void MainWindow::keyPressEvent(QKeyEvent * k)
{
#if defined(Q_OS_ANDROID)
    if (k->key() == Qt::Key_Menu) 
    {
        Page * page = dynamic_cast<Page *>(currentWidget());
        if (page)
        {
            page->menuPushed();
        }
    }
#endif
}
