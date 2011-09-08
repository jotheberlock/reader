#include "pagebuttonbar.h"
#include "page.h"

PageButtonBar::PageButtonBar(Page * p)
    : QWidget(p)
{
    page = p;

    back = new QPushButton("Back");
    dump = new QPushButton("Dump");

    back->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dump->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    back->setObjectName("backbutton");
    dump->setObjectName("dumpbutton");
    
    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(back);
    layout->addWidget(dump);
    setLayout(layout);

    connect(back, SIGNAL(clicked()), page, SLOT(backPushed()));
    connect(dump, SIGNAL(clicked()), page, SLOT(dumpPushed()));
}


