#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("AST1520设备升级软件");
    //绑定复选框改变槽函数
    connect(ui->SendTableWidget,&QTableWidget::cellChanged,this,&MainWindow::STableCheckBoxChanged);
    connect(ui->ReceiveTableWidget,&QTableWidget::cellChanged,this,&MainWindow::RTableCheckBoxChanged);
    //初始化界面
    InitView();

#if 1
    std::vector<QString> tem;
    tem.push_back("tx2083");
    tem.push_back("V1.0.1");
    tem.push_back("424C450018A6");
    tem.push_back("TX");
    InsertOneDevice(tem);
    std::vector<QString> tem1;
    tem1.push_back("tx2020");
    tem1.push_back("V1.2.2");
    tem1.push_back("asdfasdfawe");
    tem1.push_back("TX");
    InsertOneDevice(tem1);

    std::vector<QString> tem2;
    tem2.push_back("rx2083");
    tem2.push_back("V1.0.1");
    tem2.push_back("424C450018A6");
    tem2.push_back("RX");
    InsertOneDevice(tem2);
    std::vector<QString> tem3;
    tem3.push_back("rx2020");
    tem3.push_back("V1.2.2");
    tem3.push_back("asdfasdfawe");
    tem3.push_back("RX");
    InsertOneDevice(tem3);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

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

#if 0
    //连接窗口
    loginWindow = new LoginWindow();
    loginWindow->setWindowTitle("连接");
    loginWindow->setWindowModality(Qt::ApplicationModal);//除了此窗口其他窗口无法使用
    loginWindow->show();
#endif
}

/***在TableWidget表追加插入一行设备信息***
 * DeviceInfo参数说明
 * DeviceInfo[0]:设备名称
 * DeviceInfo[1]:设备版本
 * DeviceInfo[2]：设备MAC地址
 * DeviceInfo[3]：设备类型（TX/RX）***/
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
        qDebug()<<"MAC地址:"<<SendDeviceMAC[row];

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
        SendDeviceBt[row]->setText(QString("点亮设备%1").arg(row+1));
        SendDeviceBt[row]->setStyleSheet("QPushButton{background-color:rgb(85, 170, 255);border-radius: 10px;  border: 2px groove gray;}" // 按键本色
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
        qDebug()<<"MAC地址:"<<ReceiveDeviceMAC[row];

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
        ReceiveDeviceBt[row]->setText(QString("点亮设备%1").arg(row+1));
        ReceiveDeviceBt[row]->setStyleSheet("QPushButton{background-color:rgb(85, 170, 255);border-radius: 10px;  border: 2px groove gray;}" // 按键本色
                "QPushButton:hover{background-color:rgb(255, 158, 0); color: black;}"  // 鼠标停放时的色彩
                "QPushButton:pressed{background-color:red;}");
        connect(ReceiveDeviceBt[row],&QPushButton::clicked,this,&MainWindow::ReceiveDeviceBtSlot);
        ui->ReceiveTableWidget->setCellWidget(row,3,ReceiveDeviceBt[row]);
    }

}

/***发送端按钮槽函数***/
void MainWindow::SendDeviceBtSlot()
{
    QPushButton* btn = (QPushButton*)sender();  // 获取到了发送该信号按钮的指针
    qDebug() << btn->text();
}
/***接收端按钮槽函数***/
void MainWindow::ReceiveDeviceBtSlot()
{
    QPushButton* btn = (QPushButton*)sender();  // 获取到了发送该信号按钮的指针
    qDebug() << btn->text();
}

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
        if(0 < count < ReceiveDeviceName.size())//部分选
            ui->ReceiveCheckBox->setCheckState(Qt::PartiallyChecked);
        if(count == 0)//全不选
            ui->ReceiveCheckBox->setCheckState(Qt::Unchecked);
        if(count == ReceiveDeviceName.size())//全选
            ui->ReceiveCheckBox->setCheckState(Qt::Checked);
    }

}

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
        if(0 < count < SendDeviceName.size())//部分选
            ui->SendCheckBox->setCheckState(Qt::PartiallyChecked);
        if(count == 0)//全不选
            ui->SendCheckBox->setCheckState(Qt::Unchecked);
        if(count == SendDeviceName.size())//全选
            ui->SendCheckBox->setCheckState(Qt::Checked);
    }
}

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
