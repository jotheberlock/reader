#ifndef _INFOSCREEN_
#define _INFOSCREEN_

#include <QtGui/QScrollArea>
#include "page.h"

class InfoScreen : public QScrollArea
{
    Q_OBJECT

        public:

    InfoScreen(Page * page,QString caption, QString message);

    public slots:

    void okPressed();

    protected:

    Page * page;
    
};

#endif
