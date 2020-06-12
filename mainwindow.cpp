#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTreeView>
#include <QAction>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("AST1520设备升级软件");

//    //连接窗口
//    LoginWindow *loginWindow = new LoginWindow();
//    loginWindow->setWindowTitle("连接");
//    loginWindow->setWindowModality(Qt::ApplicationModal);
//    loginWindow->show();

    //初始化界面
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
//    ui->SendTableWidget->setShowGrid(false);

//    ui->SendTableWidget->set
}

MainWindow::~MainWindow()
{
    delete ui;
}
