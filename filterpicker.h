#ifndef _FILTERPICKER_
#define _FILTERPICKER_

#include <QScrollArea>
#include <QBoxLayout>
#include "settings.h"
#include "filter.h"

class FilterPicker : public QScrollArea
{
    Q_OBJECT
        
  public:

    FilterPicker(Page * p);

    public slots:

    void filterChanged(int state);
    void quitPressed();
    
  protected:

    QBoxLayout * layout;
    QHash<QObject *, Filter *> filter_mapper;
    Page * page;
    
};

#endif
