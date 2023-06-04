#ifndef MACOS_H
#define MACOS_H

#include <QGuiApplication>
#include <QWindow>

class MacOS {
    MacOS(long winid);
public:
    static void removeTitlebarFromWindow(long winId = -1);
};

#endif // MACOS_H
