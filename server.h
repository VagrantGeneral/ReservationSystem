#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event.h>
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>
#include "../JTP/JTP.h"

using namespace std;

const int lis_Max = 20;
class MyLibevent;

enum OP_TYPE
{
    DL = 1,
    ZC,
    CKYY,
    YD,
    YDXX,
    QXYD,
    TC
};

// Listen_Socket
class Socket
{
private:
    string m_ips;
    short m_port;
    int m_socketfd;

public:
    Socket() : m_ips("127.0.0.1"), m_port(6000), m_socketfd(-1)
    { /**/
    }

    Socket(const string &ips, const short port) : m_ips(ips), m_port(port), m_socketfd(-1)
    { /**/
    }

    bool Socket_Init();

    int Get_Sockfd() const { return m_socketfd; }

    ~Socket() { close(m_socketfd); }
};

// mysql operation
class MySQLdb
{
public:
    MySQLdb()
    {
        ips = "127.0.0.1";
        mysql_username = "root";
        mysql_userpasswd = "Itcast66^";
        mysql_dbname = "c2301db";
        port = 3306;
    }

    //
    bool Connect_toDb();
    bool Db_user_zc(string name, string tel, string pwd);
    bool Db_user_dl(string &name, string tel, string pwd);
    bool Db_show_yuyue(JTP &mj);
    bool Db_user_yd(string tel, string tk_id);
    bool Db_show_user_yd(JTP &mj, string tel);
    bool Db_delete_user_yd(string res_id);

    //

    ~MySQLdb()
    {
        mysql_close(&mysql_con);
    }

private:
    MYSQL mysql_con;

    string ips;
    string mysql_username;
    string mysql_userpasswd;
    string mysql_dbname;
    short port;
};

// Base Obj
class Sock_Obj
{
public:
    virtual void CallBack_Fun() = 0;
    struct event *ev;
    MyLibevent *plib;
};

// Obj to data
class Recv_Obj : public Sock_Obj
{
public:
    Recv_Obj(int fd, MyLibevent *p) : c(fd) { plib = p; }

    virtual void CallBack_Fun() override;

    // the Specific Function
    void User_dl();

    void User_zc();

    void Show_YuYue();

    void User_yd();

    void Show_user_yd();

    void Delete_user_yd();

    ~Recv_Obj() { close(c); }

private:
    int c;
    JTP m_jmj;
};

// Obj to accept
class Accept_Obj : public Sock_Obj
{
public:
    Accept_Obj(int fd, MyLibevent *p) : sockfd(fd) { plib = p; }

    virtual void CallBack_Fun() override;

private:
    int sockfd;
};

// Event to Server
class MyLibevent
{
public:
    MyLibevent() { m_base = NULL; }

    bool MyLibevent_Init();
    bool MyLibevent_Add(int fd, Sock_Obj *pobj);
    void MyLibevent_Del(Sock_Obj *pobj);
    void MyLibevent_Loop();

private:
    struct event_base *m_base;
};