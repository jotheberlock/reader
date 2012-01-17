#include <limits.h>
#include <QtGui/QSessionManager>

#include "calliope_application.h"
#include "settings.h"

// We define our own QApplication in order to participate
// in session management.

CalliopeApplication::CalliopeApplication(int & argc, char ** argv)
    : QApplication(argc, argv)
{
    exe_path = argv[0];

#if defined(__linux__)
        // Figuring out the full path means we can do session management
        // without Calliope having to be in $PATH
    char realbuf[PATH_MAX];
    if (readlink("/proc/self/exe", realbuf, PATH_MAX) != -1)
    {
        exe_path = realbuf;
    }
#endif
}

void CalliopeApplication::commitData(QSessionManager & sm)
{
    settings->save();
    
#ifndef QT_NO_SESSIONMANAGER
    QStringList qsl;
    qsl.push_back(exe_path);
    qsl.push_back("-session");
    qsl.push_back(sessionId());

    sm.setRestartCommand(qsl);
#endif
}


    
