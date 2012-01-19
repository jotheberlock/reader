#include <QtCore/QCoreApplication>

#include "settings.h"

Settings::Settings()
    : settings("Joel Dillon", "Calliope")
{
    settings.beginGroup("General");
    
    needs_reflow = true;
        // set sensible defaults if necessary
        // this can alternatively done directly in value()
        // as in getFilterActive() below but this centralises
        // the defaults better
    
    if (!settings.contains("fontsize"))
    {
#if defined(Q_OS_ANDROID)
        setFont("Droid Sans");
#else
        setFont("Times New Roman");
#endif
    }
    if (!settings.contains("fontsize"))
    {
        setFontSize(10);
    }
    if (!settings.contains("width"))
    {
        setWidth(400);
    }
    if (!settings.contains("height"))
    {
        setHeight(800);
    }
    if (!settings.contains("indent"))
    {
        setIndent(20);
    }
    if (!settings.contains("margin"))
    {
        setMargin(40);
    }
    
    if (!settings.contains("fullscreen"))
    {
#if defined(Q_OS_ANDROID)
        setFullScreen(true);
#else
        setFullScreen(false);
#endif
    }
}

Settings::~Settings()
{
    settings.sync();
}

bool Settings::needsReflow()
{
    return needs_reflow;
    needs_reflow = false;
}

void Settings::save()
{
    settings.sync();
}

void Settings::setFontSize(int f)
{
    needs_reflow = needs_reflow || (f != getFontSize());
    settings.setValue("fontsize", f);
}

int Settings::getFontSize()
{
    return settings.value("fontsize").toInt();
}

void Settings::setMargin(int m)
{
    needs_reflow = needs_reflow || (m != getMargin());
    settings.setValue("margin", m);
}

int Settings::getMargin()
{
    return settings.value("margin").toInt();
}

void Settings::setIndent(int i)
{
    needs_reflow = needs_reflow || (i != getIndent());
    settings.setValue("indent", i);
}

int Settings::getIndent()
{
    return settings.value("indent").toInt();
}

void Settings::setLeading(int l)
{
    needs_reflow = needs_reflow || (l != getLeading());
    settings.setValue("leading", l);
}

int Settings::getLeading()
{
    return settings.value("leading").toInt();
}

void Settings::setCurrentPage(int p)
{
    settings.setValue("currentpage", p);
}

int Settings::getCurrentPage()
{
    return settings.value("currentpage").toInt();
}
    
void Settings::setCurrentBook(QString b)
{
    settings.setValue("currentbook", b);
}

QString Settings::getCurrentBook()
{
    return settings.value("currentbook").toString();
}

void Settings::setFullScreen(bool f)
{
    needs_reflow = needs_reflow || (f != getFullScreen());
    settings.setValue("fullscreen", f);
}

bool Settings::getFullScreen()
{
    return settings.value("fullscreen").toBool();
}

void Settings::setFilterActive(QString name,bool active)
{
    settings.setValue(QString("filter_")+name, active);
}

bool Settings::getFilterActive(QString name)
{
    return settings.value(name, true).toBool();
}

void Settings::setActiveTouch(QString t)
{
    settings.setValue("active_touch", t);
}

QString Settings::getActiveTouch()
{
    return settings.value("active_touch").toString();
}

void Settings::setWidth(int w)
{
    needs_reflow = needs_reflow || (w != getWidth());
    settings.setValue("width", w);
}

int Settings::getWidth()
{
    return settings.value("width").toInt();
}

void Settings::setHeight(int h)
{
    needs_reflow = needs_reflow || (h != getHeight());
    settings.setValue("height", h);
}

int Settings::getHeight()
{
    return settings.value("height").toInt();
}

void Settings::setX(int x)
{
    settings.setValue("x", x);
}

int Settings::getX()
{
    return settings.value("x").toInt();
}

void Settings::setY(int y)
{
    settings.setValue("y", y);
}

int Settings::getY()
{
    return settings.value("y").toInt();
}

void Settings::setFont(QString f)
{
    needs_reflow = needs_reflow || (f != getFont());
    settings.setValue("font", f);
}

QString Settings::getFont()
{
    return settings.value("font").toString();
}
