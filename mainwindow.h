#ifndef _MAINWINDOW_
#define _MAINWINDOW_

#include <QStackedWidget>
#include <QKeyEvent>

class MainWindow : public QStackedWidget
{
  public:

    MainWindow();
    virtual void closeEvent(QCloseEvent *);

    virtual void keyPressEvent(QKeyEvent *);
    
};


#endif
