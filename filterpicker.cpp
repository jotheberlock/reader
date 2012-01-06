#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtCore/QSettings>

#include "filterpicker.h"

extern QSettings * settings;

FilterPicker::FilterPicker(QWidget * parent)
    : QWidget(parent)
{
    layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    QList<Filter *> filters = filter_manager->getFilters();
    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        Filter * filter = filters[loopc];
        QCheckBox * filterbox = new QCheckBox(filter->name());
        layout->addWidget(filterbox,1);
        filter_mapper[filterbox] = filter;
        QString setting_name = "filter_"+filter->name();
        if (settings->contains(setting_name))
        {
            filter->setActive(settings->value(setting_name).toBool());
        }
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

    QString setting_name = "filter_"+filter->name();
    settings->setValue(setting_name, (state == Qt::Checked));
    filter->setActive(state == Qt::Checked);
}

void FilterPicker::quitPressed()
{
    close();
}

