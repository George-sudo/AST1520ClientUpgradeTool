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
#include "checkout.h"
#include "mydialog.h"
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QProgressDialog>
#include "unzip_ufw.h"

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
    QByteArray JsonDataPackages(int actioncode, QString device_name, QString data);
    QByteArray FileDataPackages(QByteArray FileAllData, uchar opcode);
    void SendErrorCondition(int result, QString ErrorMessage);
    void DeviceListDataManage(QString DeviceList);
    void ChangeLedBtStateText(QString DevideMac, QString ReturnMessage);
    void DealWithUdpJsonData();
    void DealWithUdpFileData();
    void DealWithDeviceStatus();
    QString FindBinPath(QString FirmwareTpye);
    void FileTransferOperation(uchar operation);
    void SendUpgradeOrder();
    void UpdateUpdateStatusUi(QString data);

public slots:
    void SendDeviceBtSlot();
    void ReceiveDeviceBtSlot();
    void STableCheckBoxChanged(int row, int col);
    void RTableCheckBoxChanged(int row, int col);
    void ReceiveUdpData();
    void CancelSend();

private slots:
    void on_SendCheckBox_clicked();
    void on_ReceiveCheckBox_clicked();
    void SendJsonOder(int SendState,QByteArray OderData);
    void on_SelectFileBt_clicked();
    void TimeoutFun();

    void on_QuitLoginBt_clicked();
    void on_StartUpgradeBt_clicked();
    void on_RefreshListBt_clicked();

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
        REPLY_UPDATE_PROGRESS
    };
    //Server->PC
    enum{
        REPLY_PC_LOGIN_TO_SERVER = 5100,
        REPLY_PC_LOGOUT_FROM_SERVER,
        REPLAY_DEVICE_LIST,
        REPLY_PC_UPDATE_DEVICE_LIST,
        REPLY_PC_CANCEL_UPDATE_DEVICE_LIST,
        REPLY_PC_FIRMWARE_UPLOAD_START,
        REPLY_PC_REDLED_BLINK_TRIGGER,
        UPDATE_COMPLETED,
        PROGRESS_UPDATE
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
        NormalToSend = 0x11,
        TimeoutRetransmission = 0x22,
        ErrorRetransmission = 0x33
    };
    //文件传输操作码
    enum{
        SATAR_FILE_SEND = 0x01,
        REPLY_SATAR_FILE_SEND = 0x10,
        WRITE_DATA = 0x02,
        REPLY_WRITE_DATA_FINISH = 0x20,
        DATA_VERIFICATION_FAILURE = 0x2f,
        FILE_SEND_FINISH = 0x04,
        REPLY_FILE_SEND_FINISH = 0x40,
        CANCEL_FILE_SEND = 0x05,
        REPLY_CANCEL_FILE_SEND = 0x50
    };
    enum{
        //数据包大小
        PKTSIZE = 512
    };

private:
    Ui::MainWindow *ui;
    LoginWindow *loginWindow;
    QByteArray m_OderData;
    int m_msg_id;
    QTimer *MyTimer;
    int TimeoutCount;
    MyDialog *myDialog;
    static int flags;
    QProgressDialog *ProgressDialog;
    QString m_BinFileDirPath;
    QString m_BinFilePath;
    QStringList m_FileList;
    std::vector<QString> m_ReadySendFile;
    QByteArray m_OneFileData;
    QByteArray m_FileTemData;
    uint m_FileNum;
    uint m_BlockNum;
    uint m_TotalBlockNum;
    uint m_ActualSendFileSize;
    uint m_OneFileSentSize;
    uint m_OneFileSize;
    uint m_AllFileSentSize;
    uint m_AllFileSize;
    uint m_UpdateDeviceCount;
    uint m_CheckDeviceFlag;
    QString m_UpdatingDivece;

    //发送端
    std::vector<QTableWidgetItem*> SendDeviceName;
    std::vector<QTableWidgetItem*> SendDeviceVersion;
    std::vector<QLabel*> SendDeviceLabel;
    std::vector<QProgressBar*> SendDeviceBar;
    std::vector<QPushButton*> SendDeviceBt;
    std::vector<QString> SendDeviceMAC;
    std::vector<int> UpdatingSendDevice;

    //接收端
    std::vector<QTableWidgetItem*> ReceiveDeviceName;
    std::vector<QTableWidgetItem*> ReceiveDeviceVersion;
    std::vector<QLabel*> ReceiveDeviceLabel;
    std::vector<QProgressBar*> ReceiveDeviceBar;
    std::vector<QPushButton*> ReceiveDeviceBt;
    std::vector<QString> ReceiveDeviceMAC;
    std::vector<int> UpdatingReceiveDevice;
};

#endif // MAINWINDOW_H
