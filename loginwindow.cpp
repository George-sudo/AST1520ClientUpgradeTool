#include "loginwindow.h"
#include "ui_loginwindow.h"

QUdpSocket *LoginWindow::UdpSocket = nullptr;
int LoginWindow::ServerPort = 50010;
QHostAddress LoginWindow::ServerAddress;
QString LoginWindow::m_min = "NULL";
QString LoginWindow::m_sec = "NULL";
int LoginWindow::m_num = 0;

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    LoginWindow::UdpSocket = new QUdpSocket(this);
    LoginWindow::UdpSocket->bind(50010,QAbstractSocket::DontShareAddress);//绑定本地端口号
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_QuitBt_clicked()
{
    QApplication* app;
    app->quit();
}

void LoginWindow::on_LoginBt_clicked()
{
    //获取当前时间的分秒
    QString min;
    QString sec;
    QTime CurrentTime = QTime::currentTime();
    min = QString("%1").arg(CurrentTime.minute());
    sec = QString("%1").arg(CurrentTime.second());

    //生成数字编号
    if(m_min==min && m_sec==sec)
    {
        if(m_num < 9)
            ++m_num;
        else
            m_num = 0;
    }
    else
    {
        m_num = 0;
        m_min = min;
        m_sec = sec;
    }

    //创建登录服务器命令的Json对象
    QJsonObject obj;
    obj.insert("actioncode",PC_LOGIN_TO_SERVER);
    obj.insert("device_name","KVM_PC_9500");
    obj.insert("data",ui->UserLE->text()+','+ui->PassLE->text());
    obj.insert("msg_id",(m_min+m_sec+QString("%1").arg(m_num)).toInt());

    //序列化Json对象
    QJsonDocument jsonDoc(obj);
    QByteArray ba = jsonDoc.toJson();

    //生成CRC16
    quint16 crc = crc16_ccitt(ba.data(),ba.size());
    ba.append(crc>>8);
    ba.append(crc&0x00ff);

    //添加结束符
    ba.append(END);

    //设置ip
    LoginWindow::ServerAddress.setAddress(ui->IpLE->text());
//    qDebug()<<UdpSocket->bind(Address,Port,QAbstractSocket::ShareAddress);

    //发出登录服务器命令信号
    emit JsonOder(NormalToSend,ba);
}

