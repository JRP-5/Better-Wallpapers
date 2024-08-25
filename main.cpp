#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.setWindowFlags(Qt::Tool);
    w.resize(500,500);
    //w.show();
    a.exec();
    return 0;
}
