#include "mainwindow.h"
#include <QApplication>
#include<QPushButton>
#include <model.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setGUI();
    w.show();
    return a.exec();
}
