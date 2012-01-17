#ifndef _MAINWINDOW_
#define _MAINWINDOW_

#include <QtGui/QStackedWidget>
#include <QtGui/QKeyEvent>

class MainWindow : public QStackedWidget
{
  public:

    MainWindow();
    virtual void closeEvent(QCloseEvent *);

    virtual void keyPressEvent(QKeyEvent *);
    
};


#endif
