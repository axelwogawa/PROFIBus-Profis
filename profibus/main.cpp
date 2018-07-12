#include "mainwindow.h"
#include <QApplication>
#include<QPushButton>
#include <model.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setStyleSheet("background-color:gray;");
    w.show();
    w.connectGateway();

    w.setName("cool");
    return a.exec();
}
