HEADERS = mobi.h page.h element.h bookdevice.h parser.h bookshelf.h shelfscreen.h filter.h dictionary.h filterpicker.h dictionaryfilter.h whitaker.h settings.h settingsscreen.h

SOURCES = mobi.cpp page.cpp element.cpp main.cpp bookdevice.cpp parser.cpp bookshelf.cpp shelfscreen.cpp filter.cpp filterpicker.cpp dictionaryfilter.cpp whitaker.cpp settings.cpp settingsscreen.cpp

CONFIG += debug

RESOURCES = calliope.qrc

OTHER_FILES += \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-es/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values-it/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/values-el/strings.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/values-ro/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-nb/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-ja/strings.xml \
    android/res/values-fa/strings.xml \
    android/AndroidManifest.xml

