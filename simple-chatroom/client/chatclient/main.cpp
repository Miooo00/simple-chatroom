#include "mainwindow.h"
#include "dialoglogin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialoglogin *login=new Dialoglogin;
    if(login->exec()==QDialog::Accepted)
    {
        MainWindow w(nullptr,login->sockfd);
        delete login;
        w.show();
        return a.exec();
    }
    return -1;
    // MainWindow w;
    // w.show();
    // return a.exec();
}
