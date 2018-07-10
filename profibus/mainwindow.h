#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dp_device.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
void make_table(int max_ow, int max_column);
void insertIntoTableRow(QString item1, QString item2);
void setName(const QString &name);
void connectGateway();
int create_socket();
void insertThreeIntoTableRow(QString item1, QString item2,QString item3);
void parse_composite_directory (std::pair<std::vector<int>, int>& input, Dp_device& device);
private slots:
void on_disconnect_gateway_clicked();
void on_gateway_connect_clicked();
void on_read_xml_clicked();
void on_composite_list_directory_clicked();



private:
    Ui::MainWindow *ui;
};






#endif // MAINWINDOW_H
