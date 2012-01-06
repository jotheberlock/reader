#ifndef _FILTER_
#define _FILTER_

#include "page.h"
#include "element.h"

// Contract:
// Only one Filter of each subclass will be instantiated
// beforeLayout will always be called before afterLayout

class Filter
{
  public:

    Filter()
    {
        active = true;
    }

    bool getActive()
    {
        return active;
    }

    void setActive(bool a)
    {
        active=a;
    }

    virtual bool handlesTouch() = 0;
    virtual QString name() = 0;
        // Called before StringFragments are laid out
    virtual void beforeLayout(ParagraphElement *) = 0;
        // Called after Words are positioned
    virtual void afterLayout(ParagraphElement *) = 0;
        // Called on mouse/touch press - virtual coordinates
    virtual void onPress(ParagraphElement *, Page *, qint64 x, qint64 y) = 0;
        // Called on mouse/touch release - virtual coordinates
    virtual void onRelease(ParagraphElement *, Page *, qint64 x, qint64 y) = 0;

  protected:

    bool active;
    
};

class FilterManager
{
  public:

    FilterManager();
    void addFilter(Filter * filter);
    Filter * getFilter(QString name);
    QList<Filter *> getFilters();
    QList<Filter *> getTouchFilters();
    void setActiveTouchFilter(Filter * f) { touch_filter=f; }
    Filter * getActiveTouchFilter() { return touch_filter; }
    
  protected:

    QList<Filter *> filters;
    Filter * touch_filter;
    
};

extern FilterManager * filter_manager;

#endif
