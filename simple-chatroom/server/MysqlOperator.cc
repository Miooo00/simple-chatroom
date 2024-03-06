#include "MysqlOperator.h"


UserManager::UserManager(){
    con = mysql_init(NULL);
    mysql_options(con,MYSQL_SET_CHARSET_NAME, "GBK");
    con = mysql_real_connect(con,host,user,pwd,database_name,port,NULL,0);
    if(con == NULL)
    {
        exit(1);
    }
}

UserManager::~UserManager(){
    mysql_close(con);
}

bool UserManager::insert_user(User&& t){
    char sql[256];
    sprintf(sql,"insert into user(username,password) values(\"%s\",\"%s\")", t.username.c_str(),t.password.c_str());
    if(mysql_query(con,sql) != 0)
    {
        //插入失败
        return false;
    }
    return true;
}

bool UserManager::update_user(User&& t, std::string new_pwd){
    char sql[256];
    sprintf(sql,"update user set password=%s where username=%s and password=%s",new_pwd.c_str(),t.username.c_str(),t.password.c_str());
    if(mysql_query(con,sql) != 0)
    {
        //修改失败
        return false;
    }
    return true;
}

bool UserManager::delete_user(User&& t){
    char sql[256];
    sprintf(sql,"delete from user where username=%s",t.username.c_str());
    if(mysql_query(con,sql) != 0)
    {
        //删除失败
        return false;
    }
    return true;
}
int UserManager::checkUserStatu(User&& t){
    char sql[256];
    //这里查询的时候转义一下双引号
    sprintf(sql, "select * from user where username=\"%s\"",t.username.c_str());
    if(mysql_query(con,sql) != 0)
    {
        return -1;   //查询失败 查询如果没数据是查询成功
    }
    MYSQL_RES* res = mysql_store_result(con);
    MYSQL_ROW row = mysql_fetch_row(res);
    if(row == NULL)     //用户不存在
        return -1;
    if(t.password == row[1])
        return 1;
    return 0;
}

std::vector<User> UserManager::get_students(){
    std::vector<User> users;
    char sql[256];
    sprintf(sql,"select * from user");
    if(mysql_query(con,sql) == 0)
    {
        //删除失败
        return {};
    }
    MYSQL_RES* res = mysql_store_result(con);

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res)))
    {
        User user;
        user.username = row[1];
        user.password = row[2];
        users.push_back(user);
    }
    return users;
}