#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QUdpSocket>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>
#include "checkout.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

    static QUdpSocket *UdpSocket;
    static int ServerPort;
    static QHostAddress ServerAddress;
    static QString m_min;
    static QString m_sec;
    static int m_num;
    static QString User;
    static QString Pass;

    //命令码
    enum{
        PC_LOGIN_TO_SERVER = 5000,
        PC_LOGOUT_FROM_SERVER,
        PC_GET_DEVICE_LIST,
        PC_UPDATE_DEVICE_LIST,
        PC_CANCEL_UPDATE_DEVICE_LIST,
        PC_FIRMWARE_UPLOAD_START,
        PC_REDLED_BLINK_TRIGGER,
        COMMAND_REFUSE = 5555
    };

    //结束符号
    enum{
        END = 0xff
    };
    //发送状态
    enum{
        NormalToSend,
        TimeoutRetransmission,
        ErrorRetransmission
    };

signals:
    void JsonOder(int SendState,QByteArray OderData);

private slots:
    void on_QuitBt_clicked();
    void on_LoginBt_clicked();

private:
    Ui::LoginWindow *ui;

};

#endif // LOGINWINDOW_H
