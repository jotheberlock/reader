#include <QtCore/QProcess>

#include "wordplugin.h"

void WhitakerPlugin::handleWord(QString word, Page * page)
{
    word.remove(",");
    word.remove(";");
    word.remove(".");
    word.remove(":");
    word.remove("'");
    word.remove("\"");
    QProcess process;
    QStringList args;
    args.push_back(word);
    process.start("words", args);
    process.waitForStarted();
    process.waitForFinished();
    QByteArray ret = process.readAllStandardOutput();
    QString retstring(ret);
    page->displayMessage(word, retstring);
}
