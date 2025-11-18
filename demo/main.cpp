#include "MainWindow.hpp"
#include <QApplication>

#include <QDebug>
#include <QDir>

int
main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.showMaximized();

    return app.exec();
}
