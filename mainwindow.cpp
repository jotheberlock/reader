#include "mainwindow.h"
#include "settings.h"

MainWindow::MainWindow()
    : QStackedWidget(0)
{
    setObjectName("mainwindow");
    move(settings->getX(), settings->getY());
    resize(settings->getWidth(), settings->getHeight());
    if (settings->getFullScreen())
    {
        setWindowState(windowState() ^ Qt::WindowFullScreen);
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    settings->setX(x());
    settings->setY(y());
    settings->setWidth(width());
    settings->setHeight(height());
    settings->setFullScreen(windowState() | Qt::WindowFullScreen);
}
