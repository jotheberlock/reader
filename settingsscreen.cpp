#include <QtGui/QLabel>
#include <QtGui/QFontDatabase>
#include <QtGui/QIntValidator>

#include "settingsscreen.h"
#include "filter.h"

SettingsScreen::SettingsScreen(Page * p)
    : QWidget(p)
{
    page =  p;
    
    layout = new QGridLayout(this);

    font = new QComboBox();
    font->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    font_size = new QLineEdit();
    font_size->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    margin = new QLineEdit();
    margin->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    indent = new QLineEdit();
    indent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    active_touch = new QComboBox();
    active_touch->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    quit_button = new QPushButton("Apply");
    cancel_button = new QPushButton("Cancel");

    font_size->setValidator(new QIntValidator);
    margin->setValidator(new QIntValidator);
    indent->setValidator(new QIntValidator);
    
    layout->addWidget(new QLabel("Font"), 0, 0);
    layout->addWidget(font, 0, 1);
    layout->addWidget(new QLabel("Size"), 1, 0);
    layout->addWidget(font_size, 1, 1);
    layout->addWidget(new QLabel("Margin"), 2, 0);
    layout->addWidget(margin, 2, 1);
    layout->addWidget(new QLabel("Indent"), 3, 0);
    layout->addWidget(indent, 3, 1);
    layout->addWidget(new QLabel("Touch handler"), 4, 0);
    layout->addWidget(active_touch, 4, 1);
    layout->addWidget(cancel_button,5,0);
    layout->addWidget(quit_button,5,1);
    
    QFontDatabase fontdb;
    font->insertItems(0,fontdb.families());
    QStringList touchfilters;
    QList<Filter *> filters = filter_manager->getTouchFilters();
    for (int loopc=0; loopc<filters.size(); loopc++)
    {
        touchfilters.push_back(filters[loopc]->name());
    }
    active_touch->insertItems(0, touchfilters);

    font->setCurrentIndex(font->findText(settings->getFont()));
    font_size->setText(QString::number(settings->getFontSize()));
    margin->setText(QString::number(settings->getMargin()));
    indent->setText(QString::number(settings->getIndent()));
    if (filter_manager->getActiveTouchFilter())
    {
        active_touch->setCurrentIndex(font->findText(filter_manager->getActiveTouchFilter()->name()));
    }

    connect(quit_button, SIGNAL(clicked()), this, SLOT(quitPressed()));
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(cancelPressed()));
    
    setAutoFillBackground(true);
}

void SettingsScreen::quitPressed()
{
    settings->setFont(font->currentText());
    settings->setFontSize(font_size->text().toInt());
    settings->setMargin(margin->text().toInt());
    settings->setIndent(indent->text().toInt());
    settings->setActiveTouch(active_touch->currentText());
    
    filter_manager->setActiveTouchFilter(
        filter_manager->getFilter(active_touch->currentText()));

    page->getSettings();
    if (settings->needsReflow())
    {
        page->reflow();
    }
    else
    {
        qDebug("No reflow!");
    }
    
    close();
}

void SettingsScreen::cancelPressed()
{
    close();
}

