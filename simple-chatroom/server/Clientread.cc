#include "Clientread.h"
#include <string.h>
#include <unistd.h>


Clientread::Clientread():attr{0,0,0}{
    memset(buffer,'\0',sizeof(buffer));
}

Clientread::~Clientread(){

}

void Clientread::readHead(int fd,std::set<std::string>& users, std::map<int,Client>& clients){
    int headsize=HEADSIZE,pos=0,n;
    while(headsize>0)
    {
        n = read(fd,buffer+pos,headsize);
        if(n<0)
        {
            //读异常标志
            return;
        }
        else if(n==0)
        {
            //断开连接
            printf("客户端: %s 断开连接\n",clients[fd].ipaddr);
            debug("客户端: %s 断开连接",clients[fd].ipaddr);
            attr.isdisconnected = 1;
            auto it = users.find(clients[fd].name);
            if(it!=users.end())
                users.erase(it);
            clients.erase(fd);
            msg = "<sep>";
            for(auto &u:users)
            {
                msg += u;
                msg += ',';
            }
            msg.insert(0,Clientread::getIntSize(msg.size()));
            return;
        }
        headsize -= n;
        pos+=n;
    }
}

void Clientread::readText(int fd,std::set<std::string>& users, std::map<int,Client>& clients){
    // attr.isdisconnected = 0;
    int pos=0,n;
    int datasize = attr.datasize;
    while(datasize>0)
    {
        n = read(fd,buffer+pos,datasize);
        if(n<0)
        {
            //err
            return;
        }
        else if(n==0)
        {
            //断开连接
            attr.isdisconnected = 1;

            auto it = users.find(clients[fd].name);
            if(it!=users.end())
                users.erase(it);
            clients.erase(fd);

            msg = "<sep>";
            for(auto &u:users)
            {
                msg += u;
                msg += ',';
            }
            return;
        }
        datasize -= n;
        pos+=n;  
    }

    //正确读取完处理一下msg
    msg = std::string(buffer,strlen(buffer));
    msg = '['+clients[fd].name+']'+':'+msg+"<sep>";
    msg = Clientread::getIntSize(msg.size())+msg;
}

void Clientread::readList(int fd,std::set<std::string>& users, std::map<int,Client>& clients){
    int pos=0,n;
    while(attr.datasize>0)
    {
        n = read(fd,buffer+pos,attr.datasize);
        if(n<0)
        {
            //err
            return;
        }
        else if(n==0)
        {
            //断开连接
            attr.isdisconnected = 1;

            auto it = users.find(clients[fd].name);
            if(it!=users.end())
                users.erase(it);
            clients.erase(fd);

            msg = "<sep>";
            for(auto &u:users)
            {
                msg += u;
                msg += ',';
            }
            return;
        }
        attr.datasize -= n;
        pos+=n;  
    }

    msg = std::string(buffer,strlen(buffer));
    clients[fd].name = msg; //用户名称获取
    users.insert(msg);

    msg = "<sep>";
    for(auto &u:users)
    {
        msg += u;
        msg += ',';
    }
    msg.insert(0,getIntSize(msg.size()));
}

char* Clientread::readImage(int fd,std::set<std::string>& users, std::map<int,Client>& clients){
    // int statu_disconneted = 0;
    int n,pos=0;
    int datasize = attr.datasize;
    while(datasize>0)
    {
        n = read(fd,buffer+pos,datasize);
        if(n<0)
        {
            //err
            return NULL;
        }
        else if(n==0)
        {
            //断开连接
            attr.isdisconnected = 1;

            auto it = users.find(clients[fd].name);
            if(it!=users.end())
                users.erase(it);
            clients.erase(fd);

            msg = "<sep>";
            for(auto &u:users)
            {
                msg += u;
                msg += ',';
            }
            return NULL;
        }
        pos += n;
        datasize -= n;
    }

    //buffer拿到的是读取的图片字节流 现在需要在图片头部添加长度返回即可
    //datasize是字节流长度 数据长度为5个字节 申请datasize+6 末尾填\0
    char s[] = "<sep>";
    char* ret = (char*)malloc(attr.datasize+11);
    memcpy(ret,getIntSize(attr.datasize+strlen(s)).c_str(),5);
    memcpy(ret+5,s,5);
    memcpy(ret+10,buffer,attr.datasize);
    ret[attr.datasize+10] = '\0';

    //注意这里是以写二进制方式打开文件
    std::fstream fst("imgs/t.jpg",std::ios::out|std::ios::binary);
    if(!fst.is_open())
    {
        debug("图片文件打开失败!");
        return NULL;
    }
    fst.write(buffer,attr.datasize);
    fst.close();

    return ret;
}
#include <iostream>

void Clientread::readLoginInfo(int fd,std::set<std::string>& users, std::map<int,Client>& clients){
    int pos=0,n;
    int datasize = attr.datasize;
    while(datasize>0)
    {
        n = read(fd,buffer+pos,datasize);
        if(n<0)
        {
            //err
            return;
        }
        else if(n==0)
        {
            //断开连接
            attr.isdisconnected = 1;

            auto it = users.find(clients[fd].name);
            if(it!=users.end())
                users.erase(it);
            clients.erase(fd);

            msg = "<sep>";
            for(auto &u:users)
            {
                msg += u;
                msg += ',';
            }
            return;
        }
        datasize -= n;
        pos+=n;  
    }
    msg = std::string(buffer);
    int index = msg.find('|');
    user.username = msg.substr(0,index);
    user.password = msg.substr(index+1,attr.datasize);
}

void Clientread::refresh(){
    //状态重置
    memset(buffer,'\0',sizeof(buffer));
    attr.datasize = 0;
    attr.flag = 0;
    attr.isdisconnected = 0;
    user = {0,"",""};
    msg = "";
}

void Clientread::parseHead(){
    char* t = buffer;
    attr.flag = t[0] - '0';
    attr.datasize = atoi(t+1);

    //接下来要读取数据部分 刷新buffer、msg
    msg = "";
    memset(buffer,'\0',sizeof(buffer));
}

char* Clientread::getBuffer(){
    return this->buffer;
}

Attr Clientread::getconnectedStatu(){
    return this->attr;
}

std::string Clientread::getMsg(){
    return this->msg;
}

User Clientread::getUser(){
    return this->user;
}

std::string Clientread::getIntSize(int src_size)
{
    //获取传输数据长度保存在头部串中
    char temp[64];
    memset(temp,'0',HEADSIZE);
    temp[HEADSIZE] = '\0';
    std::string res(temp);
    int i=0,j=HEADSIZE-1;
    std::string des = std::to_string(src_size);
    for(i=des.size()-1;i>=0;i--)
    {
        res[j--]=des[i];
    }
    return res;
}
