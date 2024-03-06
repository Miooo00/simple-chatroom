#include "dialoglogin.h"
#include "ui_dialoglogin.h"
#include <QTcpSocket>
#include <QMessageBox>
#include "dialogregister.h"
#include "headprocess.h"

Dialoglogin::Dialoglogin(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialoglogin)
{
    ui->setupUi(this);
    sockfd = new QTcpSocket;
    dialogregister = new Dialogregister(this);
    dialogregister->hide();
    connect(sockfd,&QTcpSocket::readyRead,this,[=](){
        QByteArray msg = sockfd->readAll();
        int statu = msg.toInt();
        qDebug() << "statu:" << statu;
        if(statu == 1)
        {
            this->accept();
            return;
        }
        else if(statu == 0)
        {
            QMessageBox::warning(this,"登录失败","用户名或密码错误");
            return;
        }
        QMessageBox::warning(this,"登录失败","用户名不存在");
        return;
    });
}

Dialoglogin::~Dialoglogin()
{
    delete ui;
}

void Dialoglogin::on_btn_login_clicked()
{
    //首先从界面上获取信息
    //然后访问服务器获取数据库中的用户信息
    QString username = ui->edit_user->text();
    QString password = ui->edit_pwd->text();
    if(username.isEmpty() || password.isEmpty())
        return;
    QString u_info = username + '|' + password;
    qDebug()<<u_info;
    u_info = Headprocess::encoder(u_info,C_LOGIN);

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


void Dialoglogin::on_btn_register_clicked()
{
    dialogregister->show();
}

