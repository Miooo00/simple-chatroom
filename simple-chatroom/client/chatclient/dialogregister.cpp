#include "dialogregister.h"
#include "ui_dialogregister.h"
#include <QString>
#include "headprocess.h"
#include <QMessageBox>
#include <QTcpSocket>

Dialogregister::Dialogregister(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialogregister)
{
    ui->setupUi(this);
    sockfd = new QTcpSocket;
    connect(sockfd,&QTcpSocket::readyRead,this,[=](){
        QByteArray msg = sockfd->readAll();
        int statu = msg.toInt();
        qDebug() << "statu:" << statu;
        sockfd->close();
        if(statu == 1)
        {
            QMessageBox::information(this,NULL,"注册成功!");
            return;
        }
        else if(statu == 0)
        {
            QMessageBox::warning(this,"注册失败","用户名存在");
            return;
        }
        QMessageBox::warning(this,"注册失败","未知错误");
        return;
    });
}

Dialogregister::~Dialogregister()
{
    delete ui;
}

void Dialogregister::on_pushButton_clicked()
{
    this->close();
}


void Dialogregister::on_pushButton_2_clicked()
{
    QString username = ui->edit_username->text();
    QString password = ui->edit_password->text();
    if(username.size()<6 || username.size()>=20 || password.size() < 3)
        return;

    QString u_info = username + '|' + password;
    qDebug()<<u_info;
    u_info = Headprocess::encoder(u_info,C_REGISTER);

    QByteArray msg = u_info.toUtf8();
    char* s_msg = msg.data();
    if(sockfd->state() != QAbstractSocket::ConnectedState)
        sockfd->connectToHost("192.168.202.130", 9999);
    else
    {
        sockfd->write(s_msg);
        return;
    }
    sockfd->write(s_msg);

}

