#ifndef HEADPROCESS_H
#define HEADPROCESS_H

#include <QString>
#include <QByteArray>
#include "mystatu.h"

class Headprocess
{
public:
    Headprocess();
public:
    QString encode(QString src,int flag);
    QByteArray encode(QByteArray src,int flag);

    static QString encoder(QString src,int flag);
};

#endif // HEADPROCESS_H
