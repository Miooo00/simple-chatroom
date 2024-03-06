#pragma once

#include <set>
#include <string>
#include <map>
#include "logger/logger.h"
#include "mystatus.h"

/*
    头部解析
    数据读取
*/

using namespace uname::utility;

class Clientread{
public:
    Clientread();
    ~Clientread();

public:
    void readHead(int,std::set<std::string>&, std::map<int,Client>&);
    void readText(int,std::set<std::string>&, std::map<int,Client>&);
    void readList(int,std::set<std::string>&, std::map<int,Client>&);
    char* readImage(int,std::set<std::string>&, std::map<int,Client>&);
    void readLoginInfo(int,std::set<std::string>&, std::map<int,Client>&);
    std::string getIntSize(int);
    void refresh();
    void parseHead();

public:
    char* getBuffer();
    Attr getconnectedStatu();
    std::string getMsg();
    User getUser();

private:
    char buffer[MAXBUF];
    Attr attr;
    User user;
    std::string msg;

};