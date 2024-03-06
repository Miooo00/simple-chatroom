#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QTcpSocket>
#include <QMainWindow>
#include <QTimer>
#include <set>
#include "headprocess.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QTcpSocket *sockfd;
public:
    MainWindow(QWidget *parent = nullptr,QTcpSocket *sockfd = nullptr);
    ~MainWindow();

private slots:
    void on_socket_connected();

    void on_socket_readyread();

    void on_socket_disconnected();

    void on_timer_timesout();

    //------------------------------

    void on_actConnect_triggered();

    void on_btn_send_clicked();

    void on_btn_close_clicked();

    void on_actLogin_triggered();

    void on_actDisconnect_triggered();

    void on_btn_expression_clicked();

private:
    Ui::MainWindow *ui;

    bool m_isConnect;
    bool m_isConnecting;
    Headprocess* hp;
    QString user;
    QTimer *m_timer;
    QTimer *m_timerForinfo;
    std::set<QString> online_users;
    qint64 m_datasize;
    qint64 m_pos;
    char *m_buf;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
