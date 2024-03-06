#include "headprocess.h"


Headprocess::Headprocess() {}

QString Headprocess::encode(QString src,int flag){
    QString str(HEADSIZE,'0');

    str[0] = (char)(flag + '0');
    src.insert(0,str);
    QString tstr=QString::number(src.size()-HEADSIZE);
    for(int i=0;i<tstr.size();i++)
    {
        QString itm(1,tstr.at(tstr.size()-1-i));
        src.replace(HEADSIZE-1-i,1,itm);
    }
    return src;
}

QByteArray Headprocess::encode(QByteArray src, int flag){

    QString str(HEADSIZE,'0');

    str[0] = (char)(flag + '0');
    src.insert(0,str.toUtf8());
    QByteArray tstr=QByteArray::number(src.size()-HEADSIZE);
    for(int i=0;i<tstr.size();i++)
    {
        QByteArray itm(1,tstr.at(tstr.size()-1-i));
        src.replace(HEADSIZE-1-i,1,itm);
    }
    return src;
}
QString Headprocess::encoder(QString src,int flag){
    QString str(HEADSIZE,'0');

    str[0] = (char)(flag + '0');
    src.insert(0,str);
    QString tstr=QString::number(src.size()-HEADSIZE);
    for(int i=0;i<tstr.size();i++)
    {
        QString itm(1,tstr.at(tstr.size()-1-i));
        src.replace(HEADSIZE-1-i,1,itm);
    }
    return src;
}
