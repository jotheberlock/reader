#ifndef _FILTERPICKER_
#define _FILTERPICKER_

#include <QtGui/QWidget>
#include <QtGui/QBoxLayout>
#include "settings.h"
#include "filter.h"

class FilterPicker : public QWidget
{
    Q_OBJECT
        
  public:

    FilterPicker(QWidget * parent);

    public slots:

    void filterChanged(int state);
    void quitPressed();
    
  protected:

    QBoxLayout * layout;
    QHash<QObject *, Filter *> filter_mapper;
    
};

#endif
