#ifndef _MAINWINDOW_
#define _MAINWINDOW_

#include <QtGui/QStackedWidget>

class MainWindow : public QStackedWidget
{
  public:

    MainWindow();
    virtual void closeEvent(QCloseEvent *);
    
};


#endif
