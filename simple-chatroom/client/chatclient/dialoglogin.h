#ifndef DIALOGLOGIN_H
#define DIALOGLOGIN_H

#include <QDialog>
#include <QTcpSocket>
#include <dialogregister.h>
namespace Ui {
class Dialoglogin;
}

class Dialoglogin : public QDialog
{
    Q_OBJECT

public:
    explicit Dialoglogin(QWidget *parent = nullptr);
    ~Dialoglogin();
public:
    QTcpSocket *sockfd;
    Dialogregister *dialogregister;

private slots:
    void on_btn_login_clicked();

    void on_btn_register_clicked();

private:
    Ui::Dialoglogin *ui;
};

#endif // DIALOGLOGIN_H
