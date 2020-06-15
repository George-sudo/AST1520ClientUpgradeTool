#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QProgressBar>
#include <QPushButton>
#include <QDebug>
#include "loginwindow.h"
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitView();
    void InsertOneDevice(std::vector<QString> &eviceInfo);

public slots:
    void SendDeviceBtSlot();
    void ReceiveDeviceBtSlot();
    void STableCheckBoxChanged(int row, int col);
    void RTableCheckBoxChanged(int row, int col);

private slots:
    void on_SendCheckBox_clicked();

    void on_ReceiveCheckBox_clicked();

private:
    Ui::MainWindow *ui;
    LoginWindow *loginWindow;
    //发送端
    std::vector<QTableWidgetItem*> SendDeviceName;
    std::vector<QTableWidgetItem*> SendDeviceVersion;
    std::vector<QLabel*> SendDeviceLabel;
    std::vector<QProgressBar*> SendDeviceBar;
    std::vector<QPushButton*> SendDeviceBt;
    std::vector<QString> SendDeviceMAC;
    //接收端
    std::vector<QTableWidgetItem*> ReceiveDeviceName;
    std::vector<QTableWidgetItem*> ReceiveDeviceVersion;
    std::vector<QLabel*> ReceiveDeviceLabel;
    std::vector<QProgressBar*> ReceiveDeviceBar;
    std::vector<QPushButton*> ReceiveDeviceBt;
    std::vector<QString> ReceiveDeviceMAC;

};

#endif // MAINWINDOW_H
