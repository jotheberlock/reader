#include "filter.h"

FilterManager::FilterManager()
{
    touch_filter = 0;
}

void FilterManager::addFilter(Filter * filter) 
{
    filters.push_back(filter);
    if (!touch_filter && filter->handlesTouch())
    {
        touch_filter = filter;
    }
}

Filter * FilterManager::getFilter(QString name)
{
    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        if (filters[loopc]->name() == name)
        {
            return filters[loopc];
        }
    }

    return 0;
}

QList<Filter *> FilterManager::getTouchFilters()
{
    QList<Filter *> ret;
    
    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        if (filters[loopc]->handlesTouch())
        {
            ret.push_back(filters[loopc]);
        }
    }

    return ret;
}

QList<Filter *> FilterManager::getFilters()
{
    QList<Filter *> ret = filters;
    return ret;
}
