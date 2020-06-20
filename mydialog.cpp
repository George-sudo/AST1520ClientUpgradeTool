#include "mydialog.h"
#include "ui_mydialog.h"

MyDialog::MyDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("提示");
}

MyDialog::MyDialog(QString text)
{
    ui->label->setText(text);
}

MyDialog::~MyDialog()
{
    delete ui;
}

void MyDialog::setText(QString text)
{
    ui->label->setText(text);
}

void MyDialog::on_pushButton_clicked()
{
    this->hide();
}
