#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("AST1520设备升级软件");

    //创建连接窗口对象
    loginWindow = new LoginWindow();

    //初始化变量
    m_msg_id = 0;
    TimeoutCount = 0;

    //创建定时器
    MyTimer = new QTimer();

    //初始化主界面
    InitView();

    //初始化连接窗口
    InitLoginView();

    //绑定信号和槽函数
    connect(ui->SendTableWidget,&QTableWidget::cellChanged,this,&MainWindow::STableCheckBoxChanged);
    connect(ui->ReceiveTableWidget,&QTableWidget::cellChanged,this,&MainWindow::RTableCheckBoxChanged);
    connect(MainWindow::MyTimer,&QTimer::timeout,this,&MainWindow::TimeoutFun);

#if 0
    std::vector<QString> tem;
    tem.push_back("tx2083");
    tem.push_back("V1.0.1");
    tem.push_back("424C450018A6");
    tem.push_back("TX");
    InsertOneDevice(tem);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

/***初始化连接窗口***/
void MainWindow::InitLoginView()
{
    loginWindow->setWindowTitle("连接");
    loginWindow->setWindowModality(Qt::ApplicationModal);//除了此窗口其他窗口无法使用
    loginWindow->show();
    //连接信号槽
    connect(LoginWindow::UdpSocket,&QUdpSocket::readyRead,this,&MainWindow::ReceiveUdpData);
    connect(loginWindow,&LoginWindow::JsonOder,this,&MainWindow::SendJsonOder);
}

/***初始化主界面***/
void MainWindow::InitView()
{
    //初始化主界面
    ui->SendTableWidget->setColumnCount(4);
    ui->ReceiveTableWidget->setColumnCount(4);
    QStringList header;
    header.append(tr("设备名称"));
    header.append(tr("版本号"));
    header.append(tr("升级状态"));
    header.append(tr("操作"));
    ui->SendTableWidget->setHorizontalHeaderLabels(header);
    ui->ReceiveTableWidget->setHorizontalHeaderLabels(header);
    ui->SendTableWidget->horizontalHeader()->setStretchLastSection(true);//使行列头自适应宽度，最后一列将会填充空白部分
    ui->SendTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//使行列头自适应宽度，所有列平均分来填充空白部分
    ui->ReceiveTableWidget->horizontalHeader()->setStretchLastSection(true);//使行列头自适应宽度，最后一列将会填充空白部分
    ui->ReceiveTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//使行列头自适应宽度，所有列平均分来填充空白部分
//    ui->SendTableWidget->setShowGrid(false);//隐藏表格线条
}

/****在TableWidget表追加插入一行设备信息****
     * DeviceInfo参数说明
     * DeviceInfo[0]:设备名称
     * DeviceInfo[1]:设备版本
     * DeviceInfo[2]：设备MAC地址
     * DeviceInfo[3]：设备类型（TX/RX）
**************************************/
void MainWindow::InsertOneDevice(std::vector<QString> &DeviceInfo)
{
    //发送端
    if(DeviceInfo[3] == "TX")
    {
        int row = SendDeviceName.size();

        //增加一行数据
        ui->SendTableWidget->setRowCount(row+1);

        //设置显示设备名栏
        SendDeviceName.push_back(new QTableWidgetItem(DeviceInfo[0]));
        SendDeviceName[row]->setTextAlignment(Qt::AlignCenter);//文字居中对齐
        SendDeviceName[row]->setCheckState(Qt::Unchecked);//插入复选框,Qt::PartiallyChecked不全选
//        SendDeviceName.at(SendDeviceName.size())->setText("文字");
        ui->SendTableWidget->setItem(row,0,SendDeviceName[row]);

        //设置显示版本号栏
        SendDeviceVersion.push_back(new QTableWidgetItem(DeviceInfo[1]));
        SendDeviceVersion[row]->setTextAlignment(Qt::AlignCenter);//文字居中对齐
        ui->SendTableWidget->setItem(row,1,SendDeviceVersion[row]);

        //保存设备的MAC地址
        SendDeviceMAC.push_back(DeviceInfo[2]);
        //qDebug()<<"MAC地址:"<<SendDeviceMAC[row];

        //设置显示升级状态栏
        SendDeviceBar.push_back(new QProgressBar());
//        SendDeviceBar[row]->setValue(50);//设置进度条值
        SendDeviceBar[row]->setStyleSheet(
                        "QProgressBar {border: 2px solid grey;border-radius: 5px;"
                        "background-color: #FFFFFF;"
                        "text-align: center;}"
                        "QProgressBar::chunk {background-color: rgb(0,250,0) ;}"
                        );
        SendDeviceBar[row]->hide();
        SendDeviceLabel.push_back(new QLabel);
//        SendDeviceLabel[row]->setText("正在升级");
//        SendDeviceLabel[row]->setStyleSheet("color: rgb(0, 230, 0);");
        SendDeviceLabel[row]->setAlignment(Qt::AlignCenter);
        SendDeviceLabel[row]->hide();
//        ui->SendTableWidget->setCellWidget(0,2,bar);//此方式也可以设置进度条，但不能够在表格居中显示
        QWidget *widget = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout;//为了居中显示label和进度条
        layout->setSpacing(0);
        layout->setMargin(0);
        layout->addWidget(SendDeviceLabel[row]);
        layout->addWidget(SendDeviceBar[row]);
        widget->setLayout(layout);
        ui->SendTableWidget->setCellWidget(row,2,widget);

        //设置显示操作栏按钮
        SendDeviceBt.push_back(new QPushButton);
        SendDeviceBt[row]->setText(QString("点亮设备%1LED").arg(row+1));
        SendDeviceBt[row]->setStyleSheet("QPushButton{background-color:rgb(192, 192, 192);border-radius: 10px;  border: 2px groove gray;}" // 按键本色
                "QPushButton:hover{background-color:rgb(255, 158, 0); color: black;}"  // 鼠标停放时的色彩
                "QPushButton:pressed{background-color:red;}");
        connect(SendDeviceBt[row],&QPushButton::clicked,this,&MainWindow::SendDeviceBtSlot);
        ui->SendTableWidget->setCellWidget(row,3,SendDeviceBt[row]);

    }

    //接受端
    if(DeviceInfo[3] == "RX")
    {
        int row = ReceiveDeviceName.size();
        //增加一行数据
        ui->ReceiveTableWidget->setRowCount(row+1);

        //设置显示设备名栏
        ReceiveDeviceName.push_back(new QTableWidgetItem(DeviceInfo[0]));
        ReceiveDeviceName[row]->setTextAlignment(Qt::AlignCenter);//文字居中对齐
        ReceiveDeviceName[row]->setCheckState(Qt::Unchecked);//插入复选框,Qt::PartiallyChecked不全选
//        ReceiveDeviceName.at(ReceiveDeviceName.size())->setText("文字");
        ui->ReceiveTableWidget->setItem(row,0,ReceiveDeviceName[row]);

        //设置显示版本号栏
        ReceiveDeviceVersion.push_back(new QTableWidgetItem(DeviceInfo[1]));
        ReceiveDeviceVersion[row]->setTextAlignment(Qt::AlignCenter);//文字居中对齐
        ui->ReceiveTableWidget->setItem(row,1,ReceiveDeviceVersion[row]);

        //保存设备的MAC地址
        ReceiveDeviceMAC.push_back(DeviceInfo[2]);
        //qDebug()<<"MAC地址:"<<ReceiveDeviceMAC[row];

        //设置显示升级状态栏
        ReceiveDeviceBar.push_back(new QProgressBar());
//        ReceiveDeviceBar[row]->setValue(50);//设置进度条值
        ReceiveDeviceBar[row]->setStyleSheet(
                        "QProgressBar {border: 2px solid grey;border-radius: 5px;"
                        "background-color: #FFFFFF;"
                        "text-align: center;}"
                        "QProgressBar::chunk {background-color: rgb(0,250,0) ;}"
                        );
        ReceiveDeviceBar[row]->hide();
        ReceiveDeviceLabel.push_back(new QLabel);
//        ReceiveDeviceLabel[row]->setText("正在升级");
//        ReceiveDeviceLabel[row]->setStyleSheet("color: rgb(0, 230, 0);");
        ReceiveDeviceLabel[row]->setAlignment(Qt::AlignCenter);
        ReceiveDeviceLabel[row]->hide();
//        ui->ReceiveTableWidget->setCellWidget(0,2,bar);//此方式也可以设置进度条，但不能够在表格居中显示
        QWidget *widget = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout;//为了居中显示label和进度条
        layout->setSpacing(0);
        layout->setMargin(0);
        layout->addWidget(ReceiveDeviceLabel[row]);
        layout->addWidget(ReceiveDeviceBar[row]);
        widget->setLayout(layout);
        ui->ReceiveTableWidget->setCellWidget(row,2,widget);

        //设置显示操作栏按钮
        ReceiveDeviceBt.push_back(new QPushButton);
        ReceiveDeviceBt[row]->setText(QString("点亮设备%1LED").arg(row+1));
        ReceiveDeviceBt[row]->setStyleSheet("QPushButton{background-color:rgb(192, 192, 192);border-radius: 10px;  border: 2px groove gray;}" // 按键本色
                "QPushButton:hover{background-color:rgb(255, 158, 0); color: black;}"  // 鼠标停放时的色彩
                "QPushButton:pressed{background-color:red;}");
        connect(ReceiveDeviceBt[row],&QPushButton::clicked,this,&MainWindow::ReceiveDeviceBtSlot);
        ui->ReceiveTableWidget->setCellWidget(row,3,ReceiveDeviceBt[row]);
    }

}

/***打包命令数据***/
QByteArray MainWindow::DataPackages(int actioncode, QString device_name, QString data)
{
    //获取当前时间的分秒
    QString min;
    QString sec;
    QTime CurrentTime = QTime::currentTime();
    min = QString("%1").arg(CurrentTime.minute());
    sec = QString("%1").arg(CurrentTime.second());

    //生成数字编号
    if(LoginWindow::m_min==min && LoginWindow::m_sec==sec)
    {
        if(LoginWindow::m_num < 9)
            ++LoginWindow::m_num;
        else
            LoginWindow::m_num = 0;
    }
    else
    {
        LoginWindow::m_num = 0;
        LoginWindow::m_min = min;
        LoginWindow::m_sec = sec;
    }

    //创建Json对象
    QJsonObject obj;
    obj.insert("actioncode",actioncode);
    obj.insert("device_name",device_name);
    obj.insert("data",data);
    obj.insert("msg_id",(LoginWindow::m_min+LoginWindow::m_sec+QString("%1").arg(LoginWindow::m_num)).toInt());

    //序列化Json对象
    QJsonDocument jsonDoc(obj);
    QByteArray ba = jsonDoc.toJson();

    //生成CRC16
    quint16 crc = crc16_ccitt(ba.data(),ba.size());
    ba.append(crc>>8);
    ba.append(crc&0x00ff);

    //添加结束符
    ba.append(END);

    return ba;
}

/***返回接收错误的情况***/
void MainWindow::SendErrorCondition(int result, QString ErrorMessage)
{
    //创建Json对象
    QJsonObject obj;
    obj.insert("actioncode",COMMAND_REFUSE);
    obj.insert("device_name","KVM_PC_9500");
    obj.insert("result",result);
    obj.insert("return_message",ErrorMessage);
    obj.insert("data","");
    obj.insert("msg_id",m_msg_id);

    //序列化Json对象
    QJsonDocument jsonDoc(obj);
    QByteArray ba = jsonDoc.toJson();

    //生成CRC16
    quint16 crc = crc16_ccitt(ba.data(),ba.size());
    ba.append(crc>>8);
    ba.append(crc&0x00ff);

    //添加结束符
    ba.append(END);

    //发送接收错误情况
    LoginWindow::UdpSocket->writeDatagram(ba.data(),ba.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
}

/***将获取到的设备列表处理显示***/
void MainWindow::DeviceListDataManage(QString DeviceList)
{
    uint SendDeviceCount = SendDeviceName.size();
    //刷新窗口界面前，先将tablewidget表格清空
    for(uint i=0; i<SendDeviceCount; ++i)
    {
        delete SendDeviceName[i];
        delete SendDeviceVersion[i];
        delete SendDeviceLabel[i];
        delete SendDeviceBar[i];
        //delete SendDeviceBt[i];//导致程序异常结束
        SendDeviceName.clear();
        SendDeviceVersion.clear();
        SendDeviceLabel.clear();
        SendDeviceBar.clear();
        SendDeviceBt.clear();
        SendDeviceMAC.clear();
    }
    uint ReceiveDeviceCount = ReceiveDeviceName.size();
    for(uint j=0; j<ReceiveDeviceCount; ++j)
    {
        delete ReceiveDeviceName[j];
        delete ReceiveDeviceVersion[j];
        delete ReceiveDeviceLabel[j];
        delete ReceiveDeviceBar[j];
        //delete ReceiveDeviceBt[i];//导致程序异常结束
        ReceiveDeviceName.clear();
        ReceiveDeviceVersion.clear();
        ReceiveDeviceLabel.clear();
        ReceiveDeviceBar.clear();
        ReceiveDeviceBt.clear();
        ReceiveDeviceMAC.clear();
    }
    ui->SendTableWidget->clear();
    ui->ReceiveTableWidget->clear();
    ui->SendTableWidget->setRowCount(0);
    ui->ReceiveTableWidget->setRowCount(0);
    InitView();

    if(DeviceList.size() > 0)
    {
        QStringList devices = DeviceList.split(";");
        qDebug()<<"devices.size:"<<devices.size();
        uint DevicesCount = devices.size();
        for(uint k=0; k<DevicesCount; ++k)
        {
            QString device = devices.at(k);
            QStringList deviceItem = device.split(",");

            std::vector<QString> tem;
            tem.push_back(deviceItem.at(0));
            tem.push_back(deviceItem.at(1));
            tem.push_back(deviceItem.at(2));
            tem.push_back(deviceItem.at(4));
            InsertOneDevice(tem);
        }
    }
}

void MainWindow::ChangeLedBtStateText(QString DevideMac, QString ReturnMessage)
{
    std::vector<QString>::iterator SIter = std::find(SendDeviceMAC.begin(),SendDeviceMAC.end(),DevideMac);
    if(SIter == SendDeviceMAC.end())
    {
        std::vector<QString>::iterator RIter = std::find(ReceiveDeviceMAC.begin(),ReceiveDeviceMAC.end(),DevideMac);
        int index = std::distance(ReceiveDeviceMAC.begin(),RIter);
        QString str = ReceiveDeviceBt[index]->text().mid(ReceiveDeviceBt[index]->text().size()-4,4);
        if(ReturnMessage.contains("start",Qt::CaseInsensitive))
        {
            ReceiveDeviceBt[index]->setText("关闭设备"+str);
            ReceiveDeviceBt[index]->setStyleSheet("QPushButton{background-color:rgb(85, 170, 255);}");
        }
        if(ReturnMessage.contains("stop",Qt::CaseInsensitive))
        {
            ReceiveDeviceBt[index]->setText("点亮设备"+str);
            ReceiveDeviceBt[index]->setStyleSheet("QPushButton{background-color:rgb(192, 192, 192);}");
        }
    }
    else
    {
        int index = std::distance(SendDeviceMAC.begin(),SIter);
        QString str = SendDeviceBt[index]->text().mid(SendDeviceBt[index]->text().size()-4,4);
        if(ReturnMessage.contains("start",Qt::CaseInsensitive))
        {
            SendDeviceBt[index]->setText("关闭设备"+str);
            SendDeviceBt[index]->setStyleSheet("QPushButton{background-color:rgb(85, 170, 255);}");
        }
        if(ReturnMessage.contains("stop",Qt::CaseInsensitive))
        {
            SendDeviceBt[index]->setText("点亮设备"+str);
            SendDeviceBt[index]->setStyleSheet("QPushButton{background-color:rgb(192, 192, 192);}");
        }
    }
}

/***发送端点亮设备按钮槽函数***/
void MainWindow::SendDeviceBtSlot()
{
    QPushButton* bt = (QPushButton*)sender();  // 获取到了发送该信号按钮的指针
    QString btn = bt->text();
    int num = btn[btn.size()-4].toLatin1()-48;
    QByteArray ba = DataPackages(PC_REDLED_BLINK_TRIGGER,"KVM_PC_9500",SendDeviceMAC[num-1]);
    SendJsonOder(NormalToSend,ba);
}

/***接收端点亮设备按钮槽函数***/
void MainWindow::ReceiveDeviceBtSlot()
{
    QPushButton* bt = (QPushButton*)sender();  // 获取到了发送该信号按钮的指针
    QString btn = bt->text();
    int num = btn[btn.size()-4].toLatin1()-48;
    QByteArray ba = DataPackages(PC_REDLED_BLINK_TRIGGER,"KVM_PC_9500",ReceiveDeviceMAC[num-1]);
    SendJsonOder(NormalToSend,ba);
}

/***发送端复选框状态检查***/
void MainWindow::STableCheckBoxChanged(int row, int col)
{
    uint count = 0;
    //判断是否是复选框改变
    if(col == 0)
    {
        for(uint i=0; i<SendDeviceName.size(); i++)
        {
            if(SendDeviceName[i]->checkState() == Qt::Checked)
                ++count;
        }
        if(count>0 && count<ReceiveDeviceName.size())//部分选
            ui->SendCheckBox->setCheckState(Qt::PartiallyChecked);
        if(count == 0)//全不选
            ui->SendCheckBox->setCheckState(Qt::Unchecked);
        if(count == SendDeviceName.size())//全选
            ui->SendCheckBox->setCheckState(Qt::Checked);
    }
}

/***接收端复选框状态检查***/
void MainWindow::RTableCheckBoxChanged(int row, int col)
{
    uint count = 0;
    //判断是否是复选框改变
    if(col == 0)
    {
        for(uint i=0; i<ReceiveDeviceName.size(); i++)
        {
            if(ReceiveDeviceName[i]->checkState() == Qt::Checked)
                ++count;
        }
        if(count>0 && count<ReceiveDeviceName.size())//部分选
            ui->ReceiveCheckBox->setCheckState(Qt::PartiallyChecked);
        if(count == 0)//全不选
            ui->ReceiveCheckBox->setCheckState(Qt::Unchecked);
        if(count == ReceiveDeviceName.size())//全选
            ui->ReceiveCheckBox->setCheckState(Qt::Checked);
    }

}

/***SendCheckBox复选框槽函数***/
void MainWindow::on_SendCheckBox_clicked()
{
    if(ui->SendCheckBox->checkState() == Qt::Unchecked)
    {
        for(uint i=0; i<SendDeviceName.size(); i++)
        {
            SendDeviceName[i]->setCheckState(Qt::Unchecked);
        }
    }

    if(ui->SendCheckBox->checkState() == Qt::Checked)
    {
        for(uint i=0; i<SendDeviceName.size(); i++)
        {
            SendDeviceName[i]->setCheckState(Qt::Checked);
        }
    }
}

/***ReceiveCheckBox复选框槽函数***/
void MainWindow::on_ReceiveCheckBox_clicked()
{
    if(ui->ReceiveCheckBox->checkState() == Qt::Unchecked)
    {
        for(uint i=0; i<ReceiveDeviceName.size(); i++)
        {
            ReceiveDeviceName[i]->setCheckState(Qt::Unchecked);
        }
    }

    if(ui->ReceiveCheckBox->checkState() == Qt::Checked)
    {
        for(uint i=0; i<ReceiveDeviceName.size(); i++)
        {
            ReceiveDeviceName[i]->setCheckState(Qt::Checked);
        }
    }
}

/***接收socket数据***/
void MainWindow::ReceiveUdpData()
{
    qDebug()<<"******数据到来******";
    //判断数据是否为空
    if(LoginWindow::UdpSocket->pendingDatagramSize()>0)
    {
        QByteArray data;
        data.resize(LoginWindow::UdpSocket->pendingDatagramSize());
        LoginWindow::UdpSocket->readDatagram(data.data(),data.size());

        qDebug()<<data.data();
        //截取数据流的Json数据
        QByteArray JsonBA = data.mid(0,data.size()-3);
        //将QByteArray数据流装换为Json对象
        QJsonObject obj = QJsonDocument::fromJson(JsonBA).object();

        //计算Json数据的CRC
        quint16 JsonCrc = crc16_ccitt(JsonBA.data(),JsonBA.size());

        //查看是否有END结束符
        if((uchar)data.at(data.size()-1) != END)
        {
            SendErrorCondition(CCBR,"No end character");
            return;
        }

        //获取网络流中的CRC值
        quint16 crc = (uchar)data.at(data.size()-3)*256 + (uchar)data.at(data.size()-2);
        //比较crc
        if(JsonCrc != crc)
        {
            SendErrorCondition(CRC_ERROR,"CRC check error");
            return;
        }

        //通过msg_id来判断，接收到的数据是否为本次需要的数据
        if(m_msg_id == obj.value("msg_id").toInt())
        {
            TimeoutCount = 0;
            MyTimer->stop();

            //判断消息是否需要长时间返回宿舍
            if(obj.value("result").toInt() == LTP)
            {
                return;
            }

            switch (obj.value("actioncode").toInt()) {
            //返回登录状态
            case REPLY_PC_LOGIN_TO_SERVER:
                if(obj.value("result").toInt() == SUCCEED)
                {
                    loginWindow->hide();
                    on_RefreshListBt_clicked();
                }
                if(obj.value("result").toInt() == SVRJT)
                {
                    QMessageBox::information(NULL, "提醒", "密码或者用户名错误，请重新登录！");
                }
                break;
            //返回注销登录状态
            case REPLY_PC_LOGOUT_FROM_SERVER:
                if(obj.value("result").toInt() == SUCCEED)
                {
                    //显示登录窗口
                    loginWindow->setWindowModality(Qt::ApplicationModal);//除了此窗口其他窗口无法使用
                    loginWindow->show();
                }
                if(obj.value("result").toInt() == SVRJT)
                {
                    QMessageBox::information(NULL, "提醒", "密码或者用户名错误，退出失败！");
                }
                break;
            //返回设备列表
            case REPLAY_DEVICE_LIST:
                DeviceListDataManage(obj.value("data").toString());
                break;
            //响应设备升级
            case REPLY_PC_UPDATE_DEVICE_LIST:
                //接下来就是等待服务器返回设备升级的状态

                break;
            //响应取消设备升级
            case REPLY_PC_CANCEL_UPDATE_DEVICE_LIST:

                break;
            //响应固件上传指令
            case REPLY_PC_FIRMWARE_UPLOAD_START:
                //开始上传固件

                //上传固件完成之后，发送升级设备命令

                break;
            //响应指定设备闪烁红灯
            case REPLY_PC_REDLED_BLINK_TRIGGER:
                if(obj.value("result").toInt() == SUCCEED)
                {
                    QString mac = obj.value("data").toString();
                    QString message = obj.value("return_message").toString();
                    //根据返回的信息改变按钮文本
                    ChangeLedBtStateText(mac, message);
                }
                break;
            //返回设备升级完成状态
            case UPDATE_COMPLETED:

                break;
            //返回设备升级状态
            case PROGRESS_UPDATE:

                break;
            //错误情况
            case COMMAND_REFUSE:

                break;
            default:
                break;
            }
        }
    }
    else
    {
        MyTimer->stop();

        //服务器异常的提醒
        QMessageBox::information(NULL, "提醒", "服务器异常，请重新登录！");

        //显示登录窗口
        loginWindow->setWindowModality(Qt::ApplicationModal);//除了此窗口其他窗口无法使用
        loginWindow->show();

        //此部分是为了解决以下发生的情况
        //1、当服务器未启动，就对socket发送数据，此时socket会发出readyRead信号，但是socket里的数据为空
        //2、当1发生之后，服务启动后，再尝试对socket发送数据，socket不会发出readyRead信号（能发送数据到服务器，服务器也能正常返回信息）
        delete LoginWindow::UdpSocket;
        LoginWindow::UdpSocket = new QUdpSocket(this);
        LoginWindow::UdpSocket->bind(50010,QAbstractSocket::DontShareAddress);//绑定本地端口号
        connect(LoginWindow::UdpSocket,&QUdpSocket::readyRead,this,&MainWindow::ReceiveUdpData);
    }
}

/***发送Json数据+CRC16+0xFF***/
void MainWindow::SendJsonOder(int SendState,QByteArray OderData)
{
    //正常，非重发情况
    if(SendState == NormalToSend)
    {
        m_OderData = OderData;

        //获取保存即将发送的msg_id,用于判断接收的数据是否为PC端想要的数据
        QByteArray JsonBA = m_OderData.mid(0,m_OderData.size()-3);
        QJsonObject obj = QJsonDocument::fromJson(JsonBA).object();
        m_msg_id = obj.value("msg_id").toInt();

        //发送数据
        LoginWindow::UdpSocket->writeDatagram(m_OderData.data(),m_OderData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);

        //启动定时器
        MyTimer->start(3000);
    }

    //超时重发
    if(SendState == TimeoutRetransmission)
    {
        LoginWindow::UdpSocket->writeDatagram(m_OderData.data(),m_OderData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);

        //启动定时器
        MyTimer->start(3000);
    }

    //错误重发
    if(SendState == ErrorRetransmission)
    {
        QByteArray JsonBA = m_OderData.mid(0,m_OderData.size()-3);
        QJsonObject obj = QJsonDocument::fromJson(JsonBA).object();

        //更新msg_id
        QString min;
        QString sec;
        QTime CurrentTime = QTime::currentTime();
        min = QString("%1").arg(CurrentTime.minute());
        sec = QString("%1").arg(CurrentTime.second());
        if(LoginWindow::m_min==min && LoginWindow::m_sec==sec)
        {
            if(LoginWindow::m_num < 9)
                ++LoginWindow::m_num;
            else
                LoginWindow::m_num = 0;
        }
        else
        {
            LoginWindow::m_num = 0;
            LoginWindow::m_min = min;
            LoginWindow::m_sec = sec;
        }
        obj["msg_id"] = (LoginWindow::m_min+LoginWindow::m_sec+QString("%1").arg(LoginWindow::m_num)).toInt();

        //保存本次发送的msg_id，用于判断接收的数据是否为PC端想要的数据
        m_msg_id = (LoginWindow::m_min+LoginWindow::m_sec+QString("%1").arg(LoginWindow::m_num)).toInt();

        //更新CRC16
        QJsonDocument jsonDoc(obj);
        QByteArray ba = jsonDoc.toJson();
        quint16 crc = crc16_ccitt(ba.data(),ba.size());
        ba.append(crc>>8);
        ba.append(crc&0x00ff);

        //保存该条数据
        m_OderData = ba;

        //重发
        LoginWindow::UdpSocket->writeDatagram(m_OderData.data(),m_OderData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);

        //启动定时器
        MyTimer->start(3000);
    }
}

/***超时重传***/
void MainWindow::TimeoutFun()
{
    ++TimeoutCount;
    if(TimeoutCount == 2)
    {
        TimeoutCount = 0;
        MyTimer->stop();

        //服务器异常的提醒
        QMessageBox::information(NULL, "提醒", "服务器异常，请重新登录！");

        //显示登录窗口
        loginWindow->setWindowModality(Qt::ApplicationModal);//除了此窗口其他窗口无法使用
        loginWindow->show();
    }
    else
    {
        qDebug("重发");
        //超时重发信息
       SendJsonOder(TimeoutRetransmission, m_OderData);
    }
}


void MainWindow::on_SelectFileBt_clicked()
{
    QString str = SendDeviceBt[0]->text().mid(SendDeviceBt[0]->text().size()-4,4);
    SendDeviceBt[0]->setText("关闭设备"+str);
    SendDeviceBt[0]->setStyleSheet("QPushButton{background-color:rgb(192, 192, 192);}");
}

void MainWindow::on_QuitLoginBt_clicked()
{
    QByteArray ba = DataPackages(PC_LOGOUT_FROM_SERVER,"KVM_PC_9500", LoginWindow::User+','+LoginWindow::Pass);
    SendJsonOder(NormalToSend,ba);
}

void MainWindow::on_StartUpgradeBt_clicked()
{
//    qDebug()<<"发送端的MAC：";
//    for(int i=0; i<SendDeviceMAC.size(); ++i)
//    {
//        qDebug()<<SendDeviceMAC[i];
//    }

//    qDebug()<<"接收端的MAC：";
//    for(int i=0; i<ReceiveDeviceMAC.size(); ++i)
//    {
//        qDebug()<<ReceiveDeviceMAC[i];
//    }
//    QTableWidgetItem::checkState();
    //固件上传命令发送
    qDebug()<<SendDeviceName[0]->checkState();
}

void MainWindow::on_RefreshListBt_clicked()
{
    QByteArray ba = DataPackages(PC_GET_DEVICE_LIST,"KVM_PC_9500", "");
    SendJsonOder(NormalToSend,ba);
}
