#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QWidget>

namespace Ui {
class MyDialog;
}

class MyDialog : public QWidget
{
    Q_OBJECT

public:
    explicit MyDialog(QWidget *parent = 0);
    MyDialog(QString text);
    ~MyDialog();

    void setText(QString text);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MyDialog *ui;
};

#endif // MYDIALOG_H
