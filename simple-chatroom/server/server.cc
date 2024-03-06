#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <time.h>
#include <string>
#include <iostream>
#include <map>
#include "logger/logger.h"
#include <thread>
#include <mutex>
#include <pthread.h>
#include <set>
#include <fstream>
#include "Clientread.h"
#include "MysqlOperator.h"


using namespace uname::utility;

#define SVPORT  9999
#define MAXCLI  1024


static std::map<int,Client> clients;
static std::set<std::string> users;

int main()
{
    Logger::instance()->open("./logs/test");
    Logger::instance()->level(Logger::DEBUG);
    Logger::instance()->max(10*1024);
    struct sockaddr_in addr;
    int epid = epoll_create(1);
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        perror("socket()");
        return -1;
    }

    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret<0)
    {
        perror("bind()");
        return -1;
    }

    ret = listen(sockfd,1024);
    if(ret<0)
    {
        perror("listen()");
        return -1;
    }

    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = EPOLLIN;
    ret = epoll_ctl(epid,EPOLL_CTL_ADD,sockfd,&ev);
    if(ret<0)
    {
        perror("epoll_ctl()");
        return -1;
    }
    Clientread clientread;
    while(1)
    {
        struct epoll_event evs[MAXCLI];
        int n = epoll_wait(epid,evs,MAXCLI,-1);
        if(n<0)
        {
            printf("epoll_wait error\n");
            break;
        }
        for(int i=0;i<n;i++)
        {
            int fd = evs[i].data.fd;    //将每个事件拿出来处理
            if(fd == sockfd)    //表示有新的连接请求到达
            {
                Client client;
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int client_sockfd = accept(sockfd,(struct sockaddr*)&client_addr,&client_addr_len); //接收客户端的连接请求

                if(inet_ntop(AF_INET,&client_addr.sin_addr,client.ipaddr,INET_ADDRSTRLEN)==NULL)
                {
                    perror("ip转换错误");
                }
                debug("来自客户端: %s 的连接",client.ipaddr);
                if(client_sockfd < 0)
                {
                    printf("连接出错\n");
                    debug("连接出错");
                    continue;
                }

                struct epoll_event ev_client;   //每一个需要监听的文件描述符都需要一个epoll_event来保存
                ev_client.events = EPOLLIN;
                ev_client.data.fd = client_sockfd;
                ret = epoll_ctl(epid,EPOLL_CTL_ADD,client_sockfd,&ev_client);
                if(ret < 0)
                {
                    printf("epoll_ctl error\n");
                    break;
                }
                printf("来自客户端: %s 的连接\n",client.ipaddr);
                client.socketfd = client_sockfd;
                client.name = "";
                clients[client_sockfd] = client;
            }
            else if(evs[i].events & EPOLLIN)
            {
                clientread.readHead(fd,users,clients);
                if(clientread.getconnectedStatu().isdisconnected == 1)
                {
                    close(fd);
                    epoll_ctl(epid,EPOLL_CTL_DEL,fd,0);

                    for(auto &c:clients)
                        write(c.first,clientread.getMsg().c_str(),clientread.getMsg().size());

                    clientread.refresh();
                    continue;
                }
                //读到buffer里面有n个字节 取第一个字节判断是需要返回信息  
                //头部信息解析 数据标识+数据长度
                clientread.parseHead();
                int flag = clientread.getconnectedStatu().flag;
                int datasize = clientread.getconnectedStatu().datasize;
                int login_statu = 0;
                char* ret = NULL;
                std::cout << flag<<"  " <<datasize <<std::endl;
                switch(flag)
                {
                    case C_TEXT:
                        clientread.readText(fd,users,clients);
                        if(clientread.getconnectedStatu().isdisconnected == 1)
                        {
                            clientread.refresh();
                            break;
                        }
                        for(auto &c:clients)
                        {
                            if(c.first!=fd)
                            {
                                debug(clientread.getMsg().c_str());
                                write(c.first, clientread.getMsg().c_str(),clientread.getMsg().size());
                            }
                        }
                        clientread.refresh();
                        break;
                    
                    case C_LIST:
                        clientread.readList(fd,users,clients);
                        if(clientread.getconnectedStatu().isdisconnected == 1)
                        {
                            clientread.refresh();
                            break;
                        }
                        for(auto &c:clients)
                        {
                            // debug(msg.c_str());
                            write(c.first,clientread.getMsg().c_str(),clientread.getMsg().size());
                        }
                        clientread.refresh();
                        break;
                    
                    case C_IMAGE:
                        ret = clientread.readImage(fd,users,clients);
                        if(clientread.getconnectedStatu().isdisconnected == 1 || ret == NULL)
                        {
                            clientread.refresh();
                            break;
                        }
                        for(auto &c:clients)
                        {
                            if(c.first!=fd)
                                write(c.first,ret,datasize+11);     //待修改
                        }
                        clientread.refresh();
                        break;
                    case C_LOGIN:
                        clientread.readLoginInfo(fd,users,clients);
                        if(clientread.getconnectedStatu().isdisconnected == 1)
                        {
                            clientread.refresh();
                            break;
                        }
                        login_statu = UserManager::GetInstance()->checkUserStatu(clientread.getUser());
                        if(login_statu == 1)
                            write(fd,"1",2);
                        else if(login_statu == 0)
                            write(fd,"0",2);
                        else
                            write(fd,"2",2);
                        clientread.refresh();
                        break;
                    case C_REGISTER:
                        clientread.readLoginInfo(fd,users,clients);
                        if(clientread.getconnectedStatu().isdisconnected == 1)
                        {
                            clientread.refresh();
                            break;
                        }
                        login_statu = UserManager::GetInstance()->checkUserStatu(clientread.getUser());
                        if(login_statu == -1)   //可注册
                        {
                            if(UserManager::GetInstance()->insert_user(clientread.getUser()))
                                write(fd,"1",2);    //注册成功
                            else
                                write(fd,"2",2);    //注册失败
                        }
                        else                   
                            write(fd,"0",2);        //用户名存在
                        clientread.refresh();
                        break;
                    case C_ERROR:
                        clientread.readText(fd,users,clients);
                        std::cout << clientread.getBuffer() << std::endl;
                        break;
                    
                }
            }
        }
    
    }
    printf("监听结束...\n");
    close(epid);
    close(sockfd);

}
