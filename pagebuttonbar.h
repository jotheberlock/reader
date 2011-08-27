#ifndef _PAGEBUTTONBAR_
#define _PAGEBUTTONBAR_

#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

class Page;

class PageButtonBar : public QWidget
{
    Q_OBJECT
        
  public:

    PageButtonBar(Page *);

  protected:

    Page * page;
    QPushButton * back;
    QPushButton * dump;
    
};


#endif
