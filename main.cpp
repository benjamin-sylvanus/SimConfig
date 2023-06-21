#include "mainwindow.h"
#include <QApplication>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
