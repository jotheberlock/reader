#include <QtCore/QFile>

#include "../mobi.h"
#include "../bookdevice.h"

QByteArray known_text("<html><head><guide></guide></head><body><p align=\"center\"><img onclick=\"document.goto_page_relative(1)\" hirecindex=\"00001\" recindex=\"00001\" lorecindex=\"00001\"/></p> <mbp:pagebreak/><br> Produced by Juliet Sutherland, Josephine Paolucci, Tom Allen, and the Online Distributed Proofreading Team.<br> <br> <b>THE FALSE FACES</B><br> <br> <b>FURTHER ADVENTURES FROM THE HISTORY OF THE LONE WOLF</B><br> <br> <b>BY LOUIS JOSEPH VANCE</B><br> <br> <b>1918</B><br> <br> <b>CONTENTS</B><br> <br> I Out of No Man's Land<br> <br> II From a British Port<br> <br> III In the Barred Zone<br> <br> IV In Deep Waters<br> <br> V On the Banks<br> <br> VI Under Suspicion<br> <br> VII In Stateroom 29<br> <br> VIII Off Nantucket<br> <br> IX Sub Sea<br> <br> X At Base<br> <br> XI Under the Rose<br> <br> XII Resurrection<br> <br> XIII Reincarnation<br> <br> XIV Defamation<br> <br> XV Recognition<br> <br> XVI Au Printemps<br> <br> XVII Finesse<br> <br> XVIII Danse Macabre<br> <br> XIX Force Majeure<br> <br> XX Riposte<br> <br> XXI Question<br> <br> XXII Chicane<br> <br> XXIII Amnesty<br> <br> <b>I</B><br> <br> <b>OUT OF NO MAN'S LAND</B><br> <br> On the muddy verge of a shallow little pool the man lay prone and still, as still as those poor dead whose broken bodies rested all about him, where they had fallen, months or days, hours or weeks ago, in those grim contests which the quick were wont insensately to wage for a few charnel yards of that debatable ground.<br> <br> ");

int main(int, char **)
{
        // Project Gutenberg test text
    QFile file("FalseFaces.mobi");
    Mobi mobi;
    if (!mobi.sniff(&file))
    {
        qFatal("Failed to open file for test book");
        return 1;
    }

    mobi.open();
    
    BookDevice device(&mobi);
    if (!device.open(QIODevice::ReadOnly))
    {
        qFatal("Failure to open bookdevice");
        return 2;
    }

    QByteArray test1 = device.read(100);
    QByteArray known1 = known_text.left(100);
    if (known1 != test1)
    {
        qFatal("Read first 100 bytes test failed, expected [%s] got [%s]",
               known1.data(), test1.data());
        return 3;
    }

    device.reset();
    QByteArray test2 = device.read(100);
    if (known1 != test2)
    {
        qFatal("Reset test failed, expected [%s] got [%s]",
               known1.data(), test2.data());
        return 4;
    }

    device.seek(50);
    QByteArray known2 = known_text.mid(50,50);
    QByteArray test3 = device.read(50);
    if (known2 != test3)
    {
        qFatal("Seek test failed, expected [%s] got [%s]",
               known2.data(), test3.data());
        return 5;
    }
    
    qDebug("All tests passed");
    return 0;
}
