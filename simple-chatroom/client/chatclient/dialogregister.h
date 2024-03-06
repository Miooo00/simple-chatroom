#ifndef DIALOGREGISTER_H
#define DIALOGREGISTER_H

#include <QDialog>
#include <QTcpSocket>
namespace Ui {
class Dialogregister;
}

class Dialogregister : public QDialog
{
    Q_OBJECT

public:
    explicit Dialogregister(QWidget *parent = nullptr);
    ~Dialogregister();
    QTcpSocket *sockfd;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Dialogregister *ui;
};

#endif // DIALOGREGISTER_H
