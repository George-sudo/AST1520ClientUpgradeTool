#include "mainwindow.h"
#include "ui_mainwindow.h"

int MainWindow::flags = -1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("AST1520设备升级软件");

    ProgressDialog = new QProgressDialog("正在上传固件...","取消",0,100,this);
    ProgressDialog->setWindowTitle("进度");
    ProgressDialog->setWindowModality(Qt::WindowModal);
    ProgressDialog->cancel();
//    ProgressDialog->show();
//    ProgressDialog->setValue(50);
//    ProgressDialog->setRange(0,1000);

    //创建窗口对象
    loginWindow = new LoginWindow();
    myDialog = new MyDialog();

    //初始化变量
    m_msg_id = 0;
    TimeoutCount = 0;
    m_FileNum = 0;
    m_BlockNum = 1;
    m_AllFileSentSize = 0;
    m_AllFileSize = 0;
    m_CheckDeviceFlag = 0;

    //创建定时器
    MyTimer = new QTimer();

    //初始化主界
    InitView();

    //初始化连接窗口
    InitLoginView();

    //绑定信号和槽函数
    connect(ui->SendTableWidget,&QTableWidget::cellChanged,this,&MainWindow::STableCheckBoxChanged);
    connect(ui->ReceiveTableWidget,&QTableWidget::cellChanged,this,&MainWindow::RTableCheckBoxChanged);
    connect(MainWindow::MyTimer,&QTimer::timeout,this,&MainWindow::TimeoutFun);
    connect(ProgressDialog,&QProgressDialog::canceled,this,&MainWindow::CancelSend);

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
    //    SendDeviceBar[0]->setRange(0,100000);
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
                );//"QPushButton:pressed{background-color:red;}"
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
                );//"QPushButton:pressed{background-color:red;}"
        connect(ReceiveDeviceBt[row],&QPushButton::clicked,this,&MainWindow::ReceiveDeviceBtSlot);
        ui->ReceiveTableWidget->setCellWidget(row,3,ReceiveDeviceBt[row]);
    }

}

/***打包命令数据***/
QByteArray MainWindow::JsonDataPackages(int actioncode, QString device_name, QString data)
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

QByteArray MainWindow::FileDataPackages(QByteArray OneFileData, uchar opcode)
{
    uchar CompanyCheckCode[2] = {0x42, 0x4c};
    uchar ProjectCode[2] = {0x13, 0x8D};
    uchar ExtensionHeadLen = 0x05;

    //打包开始文件传输命令
    if(opcode == SATAR_FILE_SEND)
    {
        //数据长度
        uchar PacketDataLen[2] = {0x00, 0x00};
        uchar BlockNum[3] = {0x00};

        uint num = 0;
        if(OneFileData.size()%512)
            num = (OneFileData.size()/512)+1;
        else
            num = OneFileData.size()/512;

        m_TotalBlockNum = num;
        //文件总块数
        BlockNum[0] = num>>16;
        BlockNum[1] = (num>>8)&0x0000ff;
        BlockNum[2] = num&0x000000ff;

        //和校验
        uchar CheckNum = 0x00;

        //拼接数据
        QByteArray ba;
        ba.resize(0);
        ba.append((char*)CompanyCheckCode,2);
        ba.append((char*)ProjectCode,2);
        ba.append((char*)PacketDataLen,2);
        ba.append(ExtensionHeadLen);
        ba.append(SATAR_FILE_SEND);
        ba.append((char*)BlockNum,3);
        ba.append(CheckNum);
        return ba;
    }

    //打包512字节文件数据（除文件最后一个包）
    if(opcode == WRITE_DATA)
    {
        //计算数据长度
        QByteArray data = OneFileData.mid((m_BlockNum-1)*PKTSIZE,PKTSIZE);
        uint16_t len = data.size();
        m_ActualSendFileSize = len;
        uchar PacketDataLen[2] = {0x00};
        PacketDataLen[0] = len>>8;
        PacketDataLen[1] = len&0x00ff;

        //块编号
        uchar BlockNum[3] = {0x00};
        BlockNum[0] = m_BlockNum>>16;
        BlockNum[1] = (m_BlockNum>>8)&0x0000ff;
        BlockNum[2] = m_BlockNum&0x000000ff;

        //和校验
        uchar CheckNum = TX_CheckSum((uint8_t *)data.data(), data.size());

        //拼接数据
        QByteArray ba;
        ba.resize(0);
        ba.append((char*)CompanyCheckCode,2);
        ba.append((char*)ProjectCode,2);
        ba.append((char*)PacketDataLen,2);
        ba.append(ExtensionHeadLen);
        ba.append(WRITE_DATA);
        ba.append((char*)BlockNum,3);
        ba.append(CheckNum);
        ba.append(data.data(),len);
        return ba;
    }

    //打包取消文件传输命令
    if(opcode == CANCEL_FILE_SEND)
    {

    }

    //文件传输完成命令打包
    if(opcode == FILE_SEND_FINISH)
    {
        //数据长度
        uchar PacketDataLen[2] = {0x00, 0x00};

        //块编号
        uchar BlockNum[3] = {0x00};

        //和校验
        uchar CheckNum = 0x00;

        //拼接数据
        QByteArray ba;
        ba.resize(0);
        ba.append((char*)CompanyCheckCode,2);
        ba.append((char*)ProjectCode,2);
        ba.append((char*)PacketDataLen,2);
        ba.append(ExtensionHeadLen);
        ba.append(FILE_SEND_FINISH);
        ba.append((char*)BlockNum,3);
        ba.append(CheckNum);
        return ba;
    }
    return NULL;
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

    flags = 0;//标记socket数据到来的是Json数据
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
            ReceiveDeviceBt[index]->setStyleSheet("QPushButton{background-color:rgb(0, 213, 53);border-radius: 10px; border: 2px groove gray;}" // 按键本色
                                                  "QPushButton:hover{background-color:rgb(255, 158, 0); color: black;}"  // 鼠标停放时的色彩
                                                  );
        }
        if(ReturnMessage.contains("stop",Qt::CaseInsensitive))
        {
            ReceiveDeviceBt[index]->setText("点亮设备"+str);
            ReceiveDeviceBt[index]->setStyleSheet("QPushButton{background-color:rgb(192, 192, 192);border-radius: 10px; border: 2px groove gray;}" // 按键本色
                                                  "QPushButton:hover{background-color:rgb(255, 158, 0); color: black;}"  // 鼠标停放时的色彩
                                                  );
        }
    }
    else
    {
        int index = std::distance(SendDeviceMAC.begin(),SIter);
        QString str = SendDeviceBt[index]->text().mid(SendDeviceBt[index]->text().size()-4,4);
        if(ReturnMessage.contains("start",Qt::CaseInsensitive))
        {
            SendDeviceBt[index]->setText("关闭设备"+str);
            SendDeviceBt[index]->setStyleSheet("QPushButton{background-color:rgb(0, 213, 53);border-radius: 10px; border: 2px groove gray;}" // 按键本色
                                               "QPushButton:hover{background-color:rgb(255, 158, 0); color: black;}"  // 鼠标停放时的色彩
                                               );
        }
        if(ReturnMessage.contains("stop",Qt::CaseInsensitive))
        {
            SendDeviceBt[index]->setText("点亮设备"+str);
            SendDeviceBt[index]->setStyleSheet("QPushButton{background-color:rgb(192, 192, 192);border-radius: 10px; border: 2px groove gray;}" // 按键本色
                                               "QPushButton:hover{background-color:rgb(255, 158, 0); color: black;}"  // 鼠标停放时的色彩
                                               );
        }
    }
}

/***发送端点亮设备按钮槽函数***/
void MainWindow::SendDeviceBtSlot()
{
    QPushButton* bt = (QPushButton*)sender();  // 获取到了发送该信号按钮的指针
    QString btn = bt->text();
    int num = btn[btn.size()-4].toLatin1()-48;
    QByteArray ba = JsonDataPackages(PC_REDLED_BLINK_TRIGGER,"KVM_PC_9500",SendDeviceMAC[num-1]);
    SendJsonOder(NormalToSend,ba);
}

/***接收端点亮设备按钮槽函数***/
void MainWindow::ReceiveDeviceBtSlot()
{
    QPushButton* bt = (QPushButton*)sender();  // 获取到了发送该信号按钮的指针
    QString btn = bt->text();
    int num = btn[btn.size()-4].toLatin1()-48;
    QByteArray ba = JsonDataPackages(PC_REDLED_BLINK_TRIGGER,"KVM_PC_9500",ReceiveDeviceMAC[num-1]);
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
    qDebug()<<"******************Data coming******************";
    myDialog->hide();

    //判断数据是否为空
    if(LoginWindow::UdpSocket->pendingDatagramSize()>0)
    {
        //处理Json类型的数据
        if(flags == 0)
        {
            DealWithUdpJsonData();
            return;
        }
        //处理文件操作返回来的数据
        if(flags == 1)
        {
            DealWithUdpFileData();
            return;
        }
        //处理服务器自发起的数据（设备升级状态）
        if(flags == 2)
        {
            DealWithDeviceStatus();
            return;
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

void MainWindow::CancelSend()
{
    qDebug()<<"取消";
}

/***处理服务器返回来的Json数据***/
void MainWindow::DealWithUdpJsonData()
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

        //判断消息是否需要长时间返回数据
        if(obj.value("result").toInt() == LTP)
        {
            myDialog->setText("正在执行指令，请耐心等待...");
            myDialog->show();
            MyTimer->start(6000);
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
            if(obj.value("result").toInt() == SUCCEED)
            {
                //记录和显示即将升级的设备的进度条和升级状态
                UpdateUpdateStatusUi(obj.value("data").toString());
                //保存需要升级的设备数量
                m_UpdateDeviceCount = UpdatingSendDevice.size()+UpdatingReceiveDevice.size();
                myDialog->hide();
                flags = 2;//使udp接收函数跳到处理服务器发起的数据（也就是msg_id是服务器产生的）
            }
            break;
        //响应取消设备升级
        case REPLY_PC_CANCEL_UPDATE_DEVICE_LIST:

            break;
        //响应固件上传指令
        case REPLY_PC_FIRMWARE_UPLOAD_START:
            //开始上传固件
            if(obj.value("result").toInt() == SUCCEED)
            {
                FileTransferOperation(SATAR_FILE_SEND);
            }
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
        //错误情况
        case COMMAND_REFUSE:
            SendJsonOder(ErrorRetransmission, m_OderData);
            break;
        default:
            break;
        }
    }
}

void MainWindow::DealWithUdpFileData()
{
    QByteArray data;
    data.resize(LoginWindow::UdpSocket->pendingDatagramSize());
    LoginWindow::UdpSocket->readDatagram(data.data(),data.size());

    qDebug()<<"**********file operation return data*************";
    qDebug()<<"receive size："<<data.size();
    for(int i=0; i<data.size(); ++i)
    {
        qDebug("%#x ",(uchar)data.at(i));
    }

    TimeoutCount = 0;
    MyTimer->stop();

    //应答开始文件传输
    if(data.at(7) == REPLY_SATAR_FILE_SEND)
    {
        //显示固件传输窗口
        ProgressDialog->show();
        //写入数据包
        FileTransferOperation(WRITE_DATA);
    }

    //数据包写入完成
    if(data.at(7) == REPLY_WRITE_DATA_FINISH)
    {
        m_BlockNum += 1;
        m_AllFileSentSize += m_ActualSendFileSize;
        m_OneFileSentSize += m_ActualSendFileSize;
        ProgressDialog->setValue(m_AllFileSentSize);
        qDebug()<<"m_AllFileSentSize:"<<m_AllFileSentSize<<"  m_AllFileSize:"<<m_AllFileSize;
        qDebug()<<"next m_BlockNum:"<<m_BlockNum<<"m_TotalBlockNum:"<<m_TotalBlockNum;
        //写入数据包
        if(m_OneFileSentSize >= m_OneFileSize)
        {
            qDebug()<<"发送文件传输完成命令";
            //发送文件传输完成命令
            FileTransferOperation(FILE_SEND_FINISH);
        }
        else
        {
            FileTransferOperation(WRITE_DATA);
        }

        qDebug()<<"################send file data size:"<<m_FileTemData.size()<<"############################";
    }

    //数据验证错误
    if(data.at(7) == DATA_VERIFICATION_FAILURE)
    {
        //重发
        FileTransferOperation(DATA_VERIFICATION_FAILURE);
    }

    //应答取消文件传输命令
    if(data.at(7) == REPLY_CANCEL_FILE_SEND)
    {

    }

    //一个固件传输完成
    if(data.at(7) == REPLY_FILE_SEND_FINISH)
    {
        //ProgressDialog->setValue(m_AllFileSize);
        m_BlockNum = 1;
        m_FileNum += 1;
        qDebug()<<"一个固件传输完成！";
        //继续发送下一个固件
        FileTransferOperation(SATAR_FILE_SEND);
    }
}

void MainWindow::DealWithDeviceStatus()
{
    QByteArray data;
    data.resize(LoginWindow::UdpSocket->pendingDatagramSize());
    LoginWindow::UdpSocket->readDatagram(data.data(),data.size());

    qDebug()<<data.data();
    //截取数据流的Json数据
    QByteArray JsonBA = data.mid(0,data.size()-3);
    //将QByteArray数据流装换为Json对象
    QJsonObject obj = QJsonDocument::fromJson(JsonBA).object();
    m_msg_id = obj.value("msg_id").toInt();

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

    //返回设备的升级状态
    if(obj.value("actioncode").toInt() == PROGRESS_UPDATE)
    {
        QString str = obj.value("data").toString();
        QStringList strlist = str.split(",");
        QString mac = strlist[0];
        QString progress = strlist[1];
        //查找正在升级的设备
        if(m_CheckDeviceFlag == 0)
        {
            std::vector<QString>::iterator SIter = std::find(SendDeviceMAC.begin(),SendDeviceMAC.end(),mac);
            if(SIter == SendDeviceMAC.end())
            {
                std::vector<QString>::iterator RIter = std::find(ReceiveDeviceMAC.begin(),ReceiveDeviceMAC.end(),str.mid(1,str.size()-1));
                int index = std::distance(ReceiveDeviceMAC.begin(),RIter);
                m_UpdatingDivece = "R"+QString("%1").arg(index);
            }
            else
            {
                int index = std::distance(SendDeviceMAC.begin(),SIter);
                m_UpdatingDivece = "T,"+QString("%1").arg(index);
            }
            m_CheckDeviceFlag = 1;
        }

        QStringList list = m_UpdatingDivece.split(",");
        if(list[0] == "T")
        {
            QString strIndex = list[1];
            SendDeviceBar[strIndex.toInt()]->setValue(progress.toInt());
        }
        else
            if(list[0] == "R")
            {
                QString strIndex = list[1];
                ReceiveDeviceBar[strIndex.toInt()]->setValue(progress.toInt());
            }
    }
    else
    {
        //返回设备的升级完成状态
        if(obj.value("actioncode").toInt() == UPDATE_COMPLETED)
        {
            QStringList list = m_UpdatingDivece.split(",");
            if(list[0] == "T")
            {
                QString strIndex = list[1];
                SendDeviceLabel[strIndex.toInt()]->setText("升级完成");
                SendDeviceLabel[strIndex.toInt()]->setStyleSheet("color: rgb(85, 170, 255);");
                SendDeviceBar[strIndex.toInt()]->setStyleSheet("color: rgb(85, 170, 255);");
            }
            else
                if(list[0] == "R")
                {
                    QString strIndex = list[1];
                    ReceiveDeviceLabel[strIndex.toInt()]->setText("升级完成");
                    ReceiveDeviceLabel[strIndex.toInt()]->setStyleSheet("color: rgb(85, 170, 255);");
                    ReceiveDeviceBar[strIndex.toInt()]->setStyleSheet("color: rgb(85, 170, 255);");
                }

            m_CheckDeviceFlag = 0;
            if(m_UpdateDeviceCount <= 0)
            {
                qDebug()<<"所有设备升级完成";
                myDialog->setText("请等待所有设备重启后，即可完成设备升级...");
                myDialog->show();
            }
            else
                --m_UpdateDeviceCount;
        }
    }

    //响应服务器发送设备升级状态
    obj["actioncode"] = REPLY_UPDATE_PROGRESS;
    QJsonDocument jsonDoc(obj);
    QByteArray ba_tem = jsonDoc.toJson();
    //生成CRC16
    quint16 crc_tem = crc16_ccitt(ba_tem.data(),ba_tem.size());
    ba_tem.append(crc_tem>>8);
    ba_tem.append(crc_tem&0x00ff);
    //添加结束符
    ba_tem.append(END);
    flags = 2;
    LoginWindow::UdpSocket->writeDatagram(ba_tem.data(),ba_tem.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
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

        flags = 0;
        //发送数据
        LoginWindow::UdpSocket->writeDatagram(m_OderData.data(),m_OderData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
        qDebug()<<"send size:"<<m_OderData.size();
        //启动定时器
        MyTimer->start(3000);
    }

    //超时重发
    if(SendState == TimeoutRetransmission)
    {
        flags = 0;
        LoginWindow::UdpSocket->writeDatagram(m_OderData.data(),m_OderData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
        qDebug()<<"retransmission send size:"<<m_OderData.size();
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

        flags = 0;
        //重发
        LoginWindow::UdpSocket->writeDatagram(m_OderData.data(),m_OderData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
        qDebug()<<"error retransmission send size:"<<m_OderData.size();
        //启动定时器
        MyTimer->start(3000);
    }
}

/***opreration:SATAR_FILE_SEND、WRITE_DATA、CANCEL_FILE_SEND***/
void MainWindow::FileTransferOperation(uchar operation)
{
    //开始文件传输
    if(operation == SATAR_FILE_SEND)
    {
        if(m_FileNum < m_ReadySendFile.size())
        {
            m_OneFileSentSize = 0;
            QFile file(m_ReadySendFile[m_FileNum]);
            file.open(QFile::ReadOnly);
            m_OneFileData.resize(0);
            m_OneFileData = file.readAll();
            m_OneFileSize = m_OneFileData.size();
            QByteArray ba = FileDataPackages(m_OneFileData,SATAR_FILE_SEND);
            m_FileTemData = ba;
            flags = 1;
            MyTimer->start(3000);
            LoginWindow::UdpSocket->writeDatagram(m_FileTemData.data(),m_FileTemData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
        }
        else//固件已全部上传到服务器
        {
            m_FileNum = 0;
            //显示固件上传完成
            myDialog->setText("正在准备升级设备，请耐心等待...");
            myDialog->show();
            //发送升级设备命令
            SendUpgradeOrder();
            return;
        }
    }

    //写入文本数据
    if(operation == WRITE_DATA)
    {
        QByteArray ba = FileDataPackages(m_OneFileData,WRITE_DATA);
        m_FileTemData = ba;
        flags = 1;
//        for(int i=12; i<m_FileTemData.size(); ++i)
//        {
//            qDebug("%#x",(uchar)m_FileTemData.at(i));
//        }
        MyTimer->start(3000);
        LoginWindow::UdpSocket->writeDatagram(m_FileTemData.data(),m_FileTemData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
    }

    //取消文件传输
    if(operation == CANCEL_FILE_SEND)
    {

    }

    //文件传输完成
    if(operation == FILE_SEND_FINISH)
    {
        QByteArray ba = FileDataPackages(m_OneFileData,FILE_SEND_FINISH);
        m_FileTemData = ba;
        flags = 1;
        MyTimer->start(3000);
        LoginWindow::UdpSocket->writeDatagram(m_FileTemData.data(),m_FileTemData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
    }

    //超时/错误重发
    if(operation == TimeoutRetransmission || operation == DATA_VERIFICATION_FAILURE)
    {
        flags = 1;
        MyTimer->start(3000);
        qDebug()<<"operation:"<<operation;
        LoginWindow::UdpSocket->writeDatagram(m_FileTemData.data(),m_FileTemData.size(),LoginWindow::ServerAddress,LoginWindow::ServerPort);
    }
}

void MainWindow::SendUpgradeOrder()
{
    QString Macs;
    //判断哪些设备需要升级
    for(uint i=0; i<SendDeviceName.size(); ++i)
    {
        if(SendDeviceName[i]->checkState() == Qt::Checked)
        {
            if(Macs.size() > 0)
                Macs += (",T"+SendDeviceMAC[i]);
            else
                Macs = "T"+SendDeviceMAC[i];
        }
    }
    for(uint i=0; i<ReceiveDeviceName.size(); ++i)
    {
        if(ReceiveDeviceName[i]->checkState() == Qt::Checked)
        {
            if(Macs.size() > 0)
                Macs += (",R"+ReceiveDeviceMAC[i]);
            else
                Macs = "R"+ReceiveDeviceMAC[i];
        }
    }
    QByteArray ba = JsonDataPackages(PC_UPDATE_DEVICE_LIST,"KVM_PC_9500", Macs);
    SendJsonOder(NormalToSend,ba);
}

void MainWindow::UpdateUpdateStatusUi(QString data)
{
    QString str;
    QStringList strlist = data.split(",");
    UpdatingSendDevice.resize(0);
    UpdatingReceiveDevice.resize(0);
    for(int i=0; i<str.size(); ++i)
    {
        str = strlist[i];
        std::vector<QString>::iterator SIter = std::find(SendDeviceMAC.begin(),SendDeviceMAC.end(),str.mid(1,str.size()-1));
        if(SIter == SendDeviceMAC.end())
        {
            std::vector<QString>::iterator RIter = std::find(ReceiveDeviceMAC.begin(),ReceiveDeviceMAC.end(),str.mid(1,str.size()-1));
            int index = std::distance(ReceiveDeviceMAC.begin(),RIter);
            if(str.at(0) == 'Y')
            {
                UpdatingReceiveDevice.push_back(index);
                ReceiveDeviceLabel[index]->setText("正在升级");
                ReceiveDeviceLabel[index]->setStyleSheet("color: rgb(0, 230, 0);");
                ReceiveDeviceLabel[index]->show();

                QString FilePath = FindBinPath("KPR");
                QFile file(FilePath);
                ReceiveDeviceBar[index]->setRange(0,file.size());
                ReceiveDeviceBar[index]->setValue(0);//设置进度条值
                ReceiveDeviceBar[index]->setStyleSheet("color: rgb(0, 230, 0);");
                ReceiveDeviceBar[index]->show();
            }
            else
            {
                ReceiveDeviceLabel[index]->setText("已经升级");
                ReceiveDeviceLabel[index]->setStyleSheet("color: rgb(85, 170, 255);");
                ReceiveDeviceLabel[index]->show();

                ReceiveDeviceBar[index]->setRange(0,100);
                ReceiveDeviceBar[index]->setValue(100);//设置进度条值
                ReceiveDeviceBar[index]->setStyleSheet("color: rgb(85, 170, 255);");
                ReceiveDeviceBar[index]->show();
            }
        }
        else
        {
            int index = std::distance(SendDeviceMAC.begin(),SIter);

            if(str.at(0) == 'Y')
            {
                UpdatingSendDevice.push_back(index);
                SendDeviceLabel[index]->setText("正在升级");
                SendDeviceLabel[index]->setStyleSheet("color: rgb(0, 230, 0);");
                SendDeviceLabel[index]->show();

                QString FilePath = FindBinPath("KPT");
                QFile file(FilePath);
                SendDeviceBar[index]->setRange(0,file.size());
                SendDeviceBar[index]->setValue(0);//设置进度条值
                SendDeviceBar[index]->setStyleSheet("color: rgb(0, 230, 0);");
                SendDeviceBar[index]->show();
            }
            else
            {
                SendDeviceLabel[index]->setText("已经升级");
                SendDeviceLabel[index]->setStyleSheet("color: rgb(85, 170, 255);");
                SendDeviceLabel[index]->show();

                SendDeviceBar[index]->setRange(0,100);
                SendDeviceBar[index]->setValue(100);//设置进度条值
                SendDeviceBar[index]->setStyleSheet("color: rgb(85, 170, 255);");
                SendDeviceBar[index]->show();
            }
        }
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

        //如果是文件传送过程超时，将数据以下数据归零
        if(flags == 1)
        {
            TimeoutCount = 0;
            m_FileNum = 0;
            m_BlockNum = 1;
            m_AllFileSentSize = 0;
            m_AllFileSize = 0;
        }
    }
    else
    {
        qDebug("超时重发");
        if(flags == 0)
            //Json数据超时重发信息
            SendJsonOder(TimeoutRetransmission, m_OderData);
        else
            //文件操作数据发送超时重发
            FileTransferOperation(TimeoutRetransmission);
    }
}


void MainWindow::on_SelectFileBt_clicked()
{
    //获取文件路径（包括文件名）
    QString ufwFilePath = QFileDialog::getOpenFileName(this, tr("选择文件"),tr("*.ufw"));
    //获取文件名
    QStringList str= ufwFilePath.split("/");
    QString ufwFileName = str[str.count()-1];
    if(ufwFileName.size()>0)
    {
        ui->textBrowser->setText(ufwFileName);

        //获取压缩包所在目录
        QString ufwDir = ufwFilePath.mid(0,ufwFilePath.size()-ufwFileName.size());
        //解压ufw压缩文件
        unzipFile(ufwDir,ufwFilePath);
        //保存解压目录
        m_BinFileDirPath = ufwDir;
        //保存.bin文件名
        QDir dir(m_BinFileDirPath);
        QStringList nameFilters;
        if (dir.exists())
        {
            nameFilters << "*.bin";//过滤文件
            m_FileList = dir.entryList(nameFilters,QDir::Files | QDir::NoDotAndDotDot);
        }
        else{
            m_FileList.clear();
            qDebug() << "该目录不存在！！！";
            return;
        }
    }
}

/***查找升级的固件路径（带固件名）***
    *FirmwareTpye参数说明：
    * KPT 发送端固件；
    * KPR 接收端固件
********************************/
QString MainWindow::FindBinPath(QString FirmwareTpye)
{
    QString BinPath;
    for(int i=0; i<m_FileList.size(); ++i)
    {
        if(m_FileList[i].contains(FirmwareTpye,Qt::CaseInsensitive))
        {
            BinPath = m_BinFileDirPath + m_FileList[i];
            return BinPath;
        }
    }

    BinPath =  "not exist";
    return BinPath;
}

void MainWindow::on_QuitLoginBt_clicked()
{
    QByteArray ba = JsonDataPackages(PC_LOGOUT_FROM_SERVER,"KVM_PC_9500", LoginWindow::User+','+LoginWindow::Pass);
    SendJsonOder(NormalToSend,ba);
}

void MainWindow::on_StartUpgradeBt_clicked()
{
#if 1
    m_AllFileSentSize = 0;
    m_AllFileSize = 0;
    m_ReadySendFile.clear();

    QString data;

    //判断哪些固件需要发送给服务器
    for(uint i=0; i<SendDeviceName.size(); ++i)
    {
        if(SendDeviceName[i]->checkState() == Qt::Checked)
        {
            QString file = FindBinPath("KPT");
            if(file != "not exist")
            {
                m_ReadySendFile.push_back(file);
                data = "TX";
                break;
            }
        }
    }
    for(uint i=0; i<ReceiveDeviceName.size(); ++i)
    {
        if(ReceiveDeviceName[i]->checkState() == Qt::Checked)
        {
            QString file = FindBinPath("KPR");
            if(file != "not exist")
            {
                m_ReadySendFile.push_back(file);
                if(data.size()>0)
                    data += ",RX";
                else
                    data = "RX";
                break;
            }
        }
    }

    if(data.size()>0)
    {
        //保存需要发送所有固件的大小和
        for(uint i=0; i<m_ReadySendFile.size(); ++i)
        {
            QFile file(m_ReadySendFile[i]);
            m_AllFileSize += file.size();
        }
        ProgressDialog->setRange(0,m_AllFileSize);
        ProgressDialog->setValue(0);
        QByteArray ba = JsonDataPackages(PC_FIRMWARE_UPLOAD_START,"KVM_PC_9500",data);
        SendJsonOder(NormalToSend,ba);
    }
    else
        QMessageBox::information(NULL,"提醒","请选择升级需要的固件和需要升级的设备!");
#endif
}

void MainWindow::on_RefreshListBt_clicked()
{
    QByteArray ba = JsonDataPackages(PC_GET_DEVICE_LIST,"KVM_PC_9500", "");
    SendJsonOder(NormalToSend,ba);
}
