#pragma once

#include <string>
#include <vector>
#include <mysql/mysql.h>
#include "mystatus.h"



class UserManager
{
public:
    UserManager();
    ~UserManager();

public:
    bool insert_user(User&& t);
    bool update_user(User&& t,std::string new_pwd);
    bool delete_user(User&& t);
    int checkUserStatu(User&& t);
    std::vector<User> get_students();
public:
    //单例模式
    static UserManager* GetInstance()
    {
        static UserManager UserManager;
        return &UserManager;
    }
private:
    MYSQL* con;
    const char* host = "127.0.0.1";
    const char* user = "root";
    const char* pwd = "Lijiaheng6854.";
    const char* database_name = "test";
    const int port = 3306;
};