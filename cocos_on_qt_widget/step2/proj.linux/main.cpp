#if 1
#include "../Classes/AppDelegate.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "mainwindow.h"
#include <QApplication>

USING_NS_CC;

int main(int argc, char **argv)
{
    // create the application instance
    QApplication a(argc, argv);
    MainWindow w;
    AppDelegate app;
    w.setCocosAppDelegate(&app);
    w.initGLWidget();
    w.show();
    return a.exec();
}
#else
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
#endif
