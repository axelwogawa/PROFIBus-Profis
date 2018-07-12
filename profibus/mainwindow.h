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
void make_dynamic_table (int coloumns,int row, QStringList coloumn_names);
void insert_row_into_table(int coloumns,QStringList coloumn_names);

private slots:
void on_disconnect_gateway_clicked();
void on_gateway_connect_clicked();
void on_read_xml_clicked();
void on_composite_list_directory_clicked();
void requestParameters(int,int);


void on_readPb_clicked();

private:
    Ui::MainWindow *ui;
    std::vector<Dp_device> devices;
};






#endif // MAINWINDOW_H
