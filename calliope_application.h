#ifndef _CALLIOPE_APPLICATION_
#define _CALLIOPE_APPLICATION_

#include <QtGui/QApplication>

class CalliopeApplication : public QApplication
{
    Q_OBJECT
        
  public:

    CalliopeApplication(int &, char **);
    virtual void commitData(QSessionManager &);

  protected:

    QString exe_path;
    
};

#endif
