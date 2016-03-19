#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>

#include "shelfscreen.h"
#include "infoscreen.h"

InfoScreen::InfoScreen(Page * p, QString caption, QString text)
    : QScrollArea(p)
{
    page = p;
    QVBoxLayout * layout = new QVBoxLayout(this);

    QLabel * label = new QLabel(caption);
    layout->addWidget(label);
    QTextEdit * textedit = new QTextEdit(text);
    layout->addWidget(textedit,1);
    textedit->setReadOnly(true);
    QPushButton * push = new QPushButton("Ok");
    layout->addWidget(push);
    connect(push, SIGNAL(clicked()), this, SLOT(okPressed()));
}

void InfoScreen::okPressed()
{
    top_level->setCurrentWidget(page);
    close();
}
