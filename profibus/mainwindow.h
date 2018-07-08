#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
private slots:
void on_disconnect_gateway_clicked();
void on_gateway_connect_clicked();
void on_read_xml_clicked();




private:
    Ui::MainWindow *ui;
};






#endif // MAINWINDOW_H
