#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtCore/QSettings>

#include "shelfscreen.h"
#include "page.h"
#include "filterpicker.h"

FilterPicker::FilterPicker(Page * p)
    : QScrollArea(p)
{
    page = p;
    layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    QList<Filter *> filters = filter_manager->getFilters();
    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        printf(">> filter woo\n");
        Filter * filter = filters[loopc];
        QCheckBox * filterbox = new QCheckBox(filter->name());
        layout->addWidget(filterbox,1);
        filter_mapper[filterbox] = filter;
        filter->setActive(settings->getFilterActive(filter->name()));
        filterbox->setChecked(filter->getActive());
        connect(filterbox, SIGNAL(stateChanged(int)), this, SLOT(filterChanged(int)));
    }
    QPushButton * quit_button = new QPushButton("Done");
    layout->addWidget(quit_button,1);
    connect(quit_button, SIGNAL(clicked()), this, SLOT(quitPressed()));
    setAutoFillBackground(true);
}

void FilterPicker::filterChanged(int state)
{
    Filter * filter = filter_mapper[sender()];
    if (!filter)
    {
        qWarning("Failed to find filter in filter mapper");
        return;
    }

    settings->setFilterActive(filter->name(), state == Qt::Checked);
    filter->setActive(state == Qt::Checked);
}

void FilterPicker::quitPressed()
{
    top_level->setCurrentWidget(page);
    close();
}

