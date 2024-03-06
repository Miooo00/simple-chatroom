#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QMessageBox>
#include <pthread.h>
#include <QTimer>
#include <QInputDialog>
#include <QByteArray>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QByteArray>
#include <QImageReader>
#include <QBuffer>
#include <string>


MainWindow::MainWindow(QWidget *parent,QTcpSocket *sockfd)
    : QMainWindow(parent)
    , sockfd(sockfd)
    , ui(new Ui::MainWindow)
    , m_isConnect(false)
    , m_isConnecting(false)
    , m_datasize(-1)
    , m_pos(0)
{
    sockfd->setParent(this);
    ui->setupUi(this);

    ui->btn_close->setEnabled(false);
    ui->btn_send->setEnabled(false);
    ui->editMsg->setIconSize(QSize(200,200));
    ui->editUsers->setReadOnly(true);
    ui->actLogin->setEnabled(true);
    ui->actDisconnect->setEnabled(true);
    ui->statusBar->showMessage("已连接到:192.168.202.130");

    hp = new Headprocess;
    m_buf = new char[MAXBUF];
    memset(m_buf,'\0',MAXBUF);
    resize(750,540);
    m_timer = new QTimer;
    // if(sockfd == nullptr)
    //     sockfd = new QTcpSocket(this);

    connect(sockfd,&QTcpSocket::connected,this,&MainWindow::on_socket_connected);
    connect(sockfd,&QTcpSocket::readyRead,this,&MainWindow::on_socket_readyread);
    connect(sockfd,&QTcpSocket::disconnected,this,&MainWindow::on_socket_disconnected);
    connect(m_timer,&QTimer::timeout,this,&MainWindow::on_timer_timesout);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_socket_connected()
{
    QMessageBox::warning(this,"与服务器的连接","连接成功!");
    // ui->statusBar->showMessage("连接到:175.178.120.220 端口:9999");

    ui->statusBar->showMessage("已连接到:192.168.202.130");
    ui->actLogin->setEnabled(true);
    ui->actDisconnect->setEnabled(true);
    m_timer->stop();
    m_isConnect = true;
    m_isConnecting = false;
}

void MainWindow::on_socket_readyread()
{
    //等于-1时表示接收新数据
    qDebug()<<"有数据读";
    int n,pos=0,headsize=5;
    if(m_datasize == -1)
    {
        while(headsize>0)
        {
            n = sockfd->read(m_buf+pos,headsize);
            if(n<=0)
                break;
            pos += n;
            headsize-=n;
        }
        qDebug()<< m_buf;
        m_datasize = atoi(m_buf);
        qDebug()<<m_datasize<<":m_datasize";
        memset(m_buf,'\0',MAXBUF);

        while(m_datasize>0)
        {
            n = sockfd->read(m_buf+m_pos,m_datasize);
            if(n<=0)
                break;
            m_pos += n;
            m_datasize-=n;
        }
        qDebug()<<"需要读取的字节数:"<<m_datasize;

        if(m_datasize > 0)
            return;

        //接收到了std::string(能够完好的接收数据) 按<sep>先拆分
        std::string rec = std::string(m_buf,m_pos);

        if(rec.empty())
        {
            memset(m_buf,'\0',MAXBUF);
            m_datasize = -1;
            m_pos = 0;
            return;
        }
        qDebug()<< QString::fromStdString(rec);
        int start = rec.find("<sep>");
        std::string p1 = rec.substr(0,start);
        std::string p2 = rec.substr(start+5,rec.size()-start-5);
        if(!p1.empty())
            ui->editMsg->addItem(QString::fromStdString(p1));
        if(!p2.empty())
        {
            if(p2.size()<512)
            {
                ui->editUsers->clear();
                QStringList userList=QString::fromStdString(p2).split(',');
                for(auto &c:userList)
                {
                    if(c.isEmpty())
                        continue;
                    ui->editUsers->appendPlainText(c);
                }
            }
            else
            {
                QByteArray img = QByteArray::fromStdString(p2);
                qDebug()<< "img size:"<<img.size();
                QFile svfile("./t.jpg");
                svfile.open(QIODevice::WriteOnly);
                svfile.write(img);

                QListWidgetItem *pic = new QListWidgetItem;
                QPixmap pt;
                pt.loadFromData(img);
                pic->setIcon(QIcon(pt));
                ui->editMsg->addItem('['+user +']'+':');
                ui->editMsg->addItem(pic);
            }
        }
        if(sockfd->bytesAvailable()>0)          //有脏数据残留在sockfd中 全部读完 避免影响下一次信息传输
            sockfd->readAll();

        memset(m_buf,'\0',MAXBUF);
        m_datasize = -1;
        m_pos = 0;
    }
    else
    {
        qDebug()<<"没读完的字节数:"<<m_datasize << "mpos: "<<m_pos;
        while(m_datasize>0)
        {
            n = sockfd->read(m_buf+m_pos,m_datasize);
            if(n<=0)
                break;
            m_pos += n;
            m_datasize-=n;
        }
        qDebug()<<"还剩:"<<m_datasize << "mpos: "<<m_pos;
        if(m_datasize > 0)
            return;

        //接收到了std::string(能够完好的接收数据) 按<sep>先拆分
        std::string rec = std::string(m_buf,m_pos);
        if(rec.empty())
        {
            memset(m_buf,'\0',MAXBUF);
            m_datasize = -1;
            m_pos = 0;
            return;
        }
        int start = rec.find("<sep>");
        std::string p1 = rec.substr(0,start);
        std::string p2 = rec.substr(start+5,rec.size()-start-5);
        if(!p1.empty())
            ui->editMsg->addItem(QString::fromStdString(p1));
        if(!p2.empty())
        {
            if(p2.size()<512)
            {
                ui->editUsers->clear();
                QStringList userList=QString::fromStdString(p2).split(',');
                for(auto &c:userList)
                {
                    if(c.isEmpty())
                        continue;
                    ui->editUsers->appendPlainText(c);
                }
            }
            else
            {
                QByteArray img = QByteArray::fromStdString(p2);
                qDebug()<< "img size:"<<img.size();
                QFile svfile("./t.jpg");
                svfile.open(QIODevice::WriteOnly);
                svfile.write(img);

                QListWidgetItem *pic = new QListWidgetItem;
                QPixmap pt;
                pt.loadFromData(img);
                pic->setIcon(QIcon(pt));
                ui->editMsg->addItem('['+user +']'+':');
                ui->editMsg->addItem(pic);
            }
        }
        if(sockfd->bytesAvailable()>0)  //不知道为什么有脏数据残留在sockfd中 全部读完 避免影响下一次信息传输
            sockfd->readAll();
        memset(m_buf,'\0',MAXBUF);
        m_datasize = -1;
        m_pos = 0;
    }
}

void MainWindow::on_socket_disconnected()
{
    QMessageBox::warning(this,"与服务器的连接","与服务器断开连接!");
    ui->statusBar->showMessage("连接断开");
    ui->actLogin->setEnabled(false);
    m_isConnect = false;
    m_isConnecting = false;
    ui->btn_close->setEnabled(false);
    ui->btn_send->setEnabled(false);
}

void MainWindow::on_timer_timesout()
{
    if(sockfd->state()!=QAbstractSocket::ConnectedState)
    {
        m_isConnecting=false;
        sockfd->abort();
        QMessageBox::critical(this,"与服务器的连接","连接失败!");
        ui->statusBar->showMessage("连接超时");
    }
}

void MainWindow::on_actConnect_triggered()
{
    if(sockfd->state() == QAbstractSocket::ConnectedState)
    {
        QMessageBox::warning(this,"与服务器的连接","已经连接到服务器...");
        return;
    }
    //连接按钮
    QMessageBox::StandardButton st=QMessageBox::warning(
        this,"与服务器进行连接","确定进行连接?",
        QMessageBox::Yes|QMessageBox::No);

    if(st == QMessageBox::Yes && !m_isConnecting)
    {
        if(m_isConnect)
        {
            QMessageBox::warning(this,"与服务器的连接","已经连接到服务器...");
            return;
        }
        m_timer->setSingleShot(true);
        m_timer->setInterval(5000);
        m_timer->start();

        m_isConnecting=true;
        // sockfd->connectToHost("175.178.120.220",9999);
        sockfd->connectToHost("192.168.202.130",9999);
    }
}

void MainWindow::on_btn_send_clicked()
{
    QString sendMsg=ui->editSend->toPlainText();
    ui->editMsg->addItem('['+user +']'+':'+sendMsg);
    ui->editSend->clear();

    //1个字节存储数据类型，4个字节存储数据大小
    sendMsg = hp->encode(sendMsg,C_TEXT);

    qDebug()<<sendMsg;
    QByteArray by=sendMsg.toUtf8();     //这里要先用qbytearray对象去接收数据 不能一次性写
    char* msg=by.data();
    qDebug()<<msg;
    sockfd->write(msg);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(sockfd->state()==QAbstractSocket::ConnectedState)
        qDebug() << "Socket is connected.";
    else
        qDebug() << "Socket is not connected.";
    sockfd->close();
    event->accept();
}

void MainWindow::on_btn_close_clicked()
{
    sockfd->close();
    close();
}

void MainWindow::on_actLogin_triggered()
{
    //加入连接按钮
    while(true)
    {
        user= QInputDialog::getText(this,"输入名称", "您的名字:",QLineEdit::Normal);
        if(user.length()>=10)
        {
            QMessageBox::warning(this,"","名字过长或为空");
            continue;
        }
        else if(user.isEmpty())
            return;
        else
            break;
    }
    QString sendMsg="";
    sendMsg += user;
    sendMsg = hp->encode(sendMsg,C_LIST);

    qDebug()<<sendMsg;
    ui->actLogin->setEnabled(false);
    ui->btn_send->setEnabled(true);
    sockfd->write(sendMsg.toUtf8().data());

}

void MainWindow::on_actDisconnect_triggered()
{
    //断开连接按钮
    if(online_users.size()==1)
        online_users.erase(online_users.begin());
    else
    {
        auto it = online_users.find(this->user);
        if(it!=online_users.end())
            online_users.erase(it);
    }
    ui->editUsers->clear();
    sockfd->abort();
    ui->actLogin->setEnabled(false);
    ui->actDisconnect->setEnabled(false);
    m_isConnect = false;
    m_isConnecting = false;
    ui->btn_close->setEnabled(false);
    ui->btn_send->setEnabled(false);
}

void MainWindow::on_btn_expression_clicked()
{
    QString curdir = QApplication::applicationDirPath();
    QString filename = QFileDialog::getOpenFileName(this,"打开一张图片",curdir,"图片文件(*.jpg)");
    qDebug()<<filename;
    QListWidgetItem *pic = new QListWidgetItem;
    pic->setIcon(QIcon(filename));
    ui->editMsg->addItem('['+user +']'+':');
    ui->editMsg->addItem(pic);

    //将图片发送到服务器
    QFile img(filename);
    img.open(QIODevice::ReadOnly);
    QByteArray imgdata = img.readAll();
    imgdata = hp->encode(imgdata,C_IMAGE);

    qDebug() << "图片含头部大小:"<<imgdata.size();
    if(!sockfd->isOpen())
        return;

    sockfd->write(imgdata,imgdata.size());  //！！！第二个参数标明传输字节大小 不然会出错
}

