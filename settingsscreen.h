#ifndef _SETTINGSSCREEN_
#define _SETTINGSSCREEN_

#include <QScrollArea>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include "settings.h"
#include "page.h"

class SettingsScreen : public QScrollArea
{
    Q_OBJECT
        
  public:

    SettingsScreen(Page *);

    public slots:

    void quitPressed();
    void cancelPressed();

  protected:

    QComboBox * font;
    QLineEdit * font_size;
    QLineEdit * margin;
    QLineEdit * indent;
    QLineEdit * leading;
    QComboBox * active_touch;
    
    QGridLayout * layout;
    QPushButton * quit_button;
    QPushButton * cancel_button; 

    Page * page;
    
};


#endif
