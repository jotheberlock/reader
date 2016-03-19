#ifndef _SETTINGS_
#define _SETTINGS_

#include <QString>
#include <QSettings>

class Settings
{
  public:

    Settings();
    ~Settings();

        // Has a settings change occurred requiring re-layout?
        // Note that calling this clears the flag
    bool needsReflow();
        // Makes sure QSettings syncs
    void save();
    
    void setFontSize(int);
    int getFontSize();

    void setMargin(int);
    int getMargin();
    
    void setIndent(int);
    int getIndent();
    
    void setLeading(int);
    int getLeading();

    void setCurrentPage(int);
    int getCurrentPage();
    
    void setCurrentBook(QString);
    QString getCurrentBook();

    void setFullScreen(bool);
    bool getFullScreen();

    void setFilterActive(QString name,bool);
    bool getFilterActive(QString name);

    void setActiveTouch(QString);
    QString getActiveTouch();

    void setWidth(int);
    int getWidth();

    void setHeight(int);
    int getHeight();
    
    void setX(int);
    int getX();
    
    void setY(int);
    int getY();
    
    void setFont(QString);
    QString getFont();
    
  protected:

    QSettings settings;
    bool needs_reflow;
    
};

extern Settings * settings;

#endif
