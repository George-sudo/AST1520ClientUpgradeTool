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
#include <QJsonDocument>
#include <QJsonObject>
#include "loginwindow.h"
#include <vector>
#include <QTimer>
#include <QMessageBox>
#include "crc.h"


namespace Ui {
class MainWindow;
}

class LoginWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitView();
    void InitLoginView();
    void InsertOneDevice(std::vector<QString> &eviceInfo);
    QByteArray DataPackages(int actioncode, QString device_name, QString data);
    void SendErrorCondition(int result, QString ErrorMessage);

public slots:
    void SendDeviceBtSlot();
    void ReceiveDeviceBtSlot();
    void STableCheckBoxChanged(int row, int col);
    void RTableCheckBoxChanged(int row, int col);
    void ReceiveUdpData();

private slots:
    void on_SendCheckBox_clicked();
    void on_ReceiveCheckBox_clicked();
    void SendJsonOder(int SendState,QByteArray OderData);
    void on_SelectFileBt_clicked();
    void TimeoutFun();

private:
    //返回result码
    enum{
        SUCCEED = 200,
        LTP = 201,//接收并解析成功，但处理需要较长时间。先返回这个，执行成功后再返回200
        CRC_ERROR = 401,
        CCBR = 404,//命令无法解析（包括没有0xFF结束，JSON格式不正确）
        SVRJT = 406,//服务拒绝，接收方不处理发送方的请求。登录或其他验证错误
        FAILED = 500//执行失败（命令错误，或接收端故障）
    };
    //PC->Server命令码
    enum{
        PC_LOGIN_TO_SERVER = 5000,
        PC_LOGOUT_FROM_SERVER,
        PC_GET_DEVICE_LIST,
        PC_UPDATE_DEVICE_LIST,
        PC_CANCEL_UPDATE_DEVICE_LIST,
        PC_FIRMWARE_UPLOAD_START,
        PC_REDLED_BLINK_TRIGGER,
    };
    //Server->PC
    enum{
        REPLY_PC_LOGIN_TO_SERVER = 5100,
        REPLY_PC_LOGOUT_FROM_SERVER,
        REPLAY_DEVICE_LIST,
        REPLY_PC_UPDATE_DEVICE_LIST,
        REPLY_PC_CANCEL_UPDATE_DEVICE_LIST,
        REPLY_PC_FIRMWARE_UPLOAD_START,
        REPLY_PC_REDLED_BLINK_TRIGGER
    };
    //PC<->Server
    enum{
       COMMAND_REFUSE = 5555
    };
    //结束符
    enum{
        END = 0xff
    };
    //发送状态
    enum{
        NormalToSend,
        TimeoutRetransmission,
        ErrorRetransmission
    };

private:
    Ui::MainWindow *ui;
    LoginWindow *loginWindow;
    QByteArray m_OderData;
    int m_msg_id;
    QTimer *MyTimer;
    int TimeoutCount;

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
