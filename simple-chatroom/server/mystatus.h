#ifndef MYSTATUS_H__
#define MYSTATUS_H__

#define C_TEXT      1
#define C_LIST      2
#define C_IMAGE     3
#define C_LOGIN     4
#define C_REGISTER  5
#define C_ERROR     6

#define MAXBUF 1024*1024*6
#define HEADSIZE 5

struct Client{
    int socketfd;
    char ipaddr[64];
    std::string name;
};

struct Attr{
    int isdisconnected;
    int flag;
    int datasize;
};

typedef struct User{
    int state;
    std::string username;
    std::string password;
}User;

#endif