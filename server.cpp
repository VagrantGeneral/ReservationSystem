#include "server.h"

void G_CALLBACK_FUN(int fd, short ev, void *arg)
{
    Sock_Obj *obj = (Sock_Obj *)arg;
    if (obj == NULL)
    {
        return;
    }

    if (ev & EV_READ)
    {
        obj->CallBack_Fun();
    }
}

bool Socket::Socket_Init()
{
    m_socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (m_socketfd == -1)
    {
        return false;
    }

    struct sockaddr_in saddr; //
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(m_port);
    saddr.sin_addr.s_addr = inet_addr(m_ips.c_str());

    if (bind(m_socketfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
    {
        return false;
    }

    if (listen(m_socketfd, lis_Max) == -1)
    {
        return false;
    }

    return true;
}

bool MySQLdb::Connect_toDb()
{
    MYSQL *mysql = mysql_init(&mysql_con);
    if (mysql == NULL)
    {
        return false;
    }

    mysql = mysql_real_connect(mysql, ips.c_str(), mysql_username.c_str(), mysql_userpasswd.c_str(), mysql_dbname.c_str(), port, NULL, 0);
    if (mysql == NULL)
    {
        cout << "DataBase connect failed" << endl;
        return false;
    }

    return true;
}

bool MySQLdb::Db_user_zc(string name, string tel, string pwd)
{
    string sql = string("insert into user_info value(0, '") + tel + string("', '") + name + string("', '") + pwd + string("', '1')");
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }
    return true;
}

bool MySQLdb::Db_user_dl(string &name, string tel, string pwd)
{

    string sql = string("select user_name, user_passwd from user_info where user_tel = ") + tel;
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }

    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    int num = mysql_num_rows(r);
    if (num != 1)
    {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(r);

    name = row[0];
    string passwd = row[1];
    if (passwd.compare(pwd) != 0)
    {
        return false;
    }
    return true;
}

bool MySQLdb::Db_show_yuyue(JTP &mj)
{
    string sql = string("select * from ticket_info");
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }
    //
    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    int num = mysql_num_rows(r);
    mj.JTP_Add("num", num); // num
    if (num == 0)
    {
        return true;
    }

    for (int i = 0; i < num; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(r);
        JTP xmj;
        xmj.JTP_Add("tk_id", row[0]);
        xmj.JTP_Add("tk_name", row[1]);
        xmj.JTP_Add("tk_max", row[2]);
        xmj.JTP_Add("tk_count", row[3]);
        xmj.JTP_Add("tk_date", row[4]);

        mj.JTP_Add("arr", xmj); // xmj
    }
    return true;
}

bool MySQLdb::Db_user_yd(string tel, string tk_id)
{
    // select the info
    string sql_read = string("select tk_max, tk_count from ticket_info where tk_id = ") + tk_id;
    if (mysql_query(&mysql_con, sql_read.c_str()) != 0)
    {
        return false;
    }

    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    int num = mysql_num_rows(r);
    if (num != 1)
    {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(r);
    int max_num = atoi(row[0]);
    int count = atoi(row[1]);
    if (count >= max_num)
    {
        return false;
    }
    count++;

    // Begin();
    // update info
    string sql_update = string("update ticket_info set tk_count = ") + to_string(count) + string(" where tk_id = ") + tk_id;
    if (mysql_query(&mysql_con, sql_update.c_str()) != 0)
    {
        // RollBack();
        return false;
    }

    // insert info
    string sql_res = string("insert into ticket_res values(0,") + tk_id + string(",") + tel + string(",now())");
    if (mysql_query(&mysql_con, sql_res.c_str()) != 0)
    {
        // RollBack();
        return false;
    }
    // Commit();

    return true;
}

bool MySQLdb::Db_show_user_yd(JTP &mj, string tel)
{
    string sql = string("select res_id, tk_name, yd_time from ticket_info, ticket_res where ticket_info.tk_id = ticket_res.tk_id and ticket_res.user_tel = ") + tel;
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }
    //
    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    int num = mysql_num_rows(r);
    mj.JTP_Add("num", num); // num
    if (num == 0)
    {
        return true;
    }

    for (int i = 0; i < num; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(r);
        JTP xmj;
        xmj.JTP_Add("res_id", row[0]);
        xmj.JTP_Add("tk_name", row[1]);
        xmj.JTP_Add("yd_time", row[2]);

        mj.JTP_Add("arr", xmj); // xmj
    }
    return true;
}

bool MySQLdb::Db_delete_user_yd(string res_id)
{
    string sql_tkid = string("select tk_id from ticket_res where res_id = ") + res_id;
    if (mysql_query(&mysql_con, sql_tkid.c_str()) != 0)
    {
        return false;
    }
    //
    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }
    int num = mysql_num_rows(r);
    if (num == 0)
    {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(r);
    string tk_id = row[0];
    mysql_free_result(r);

    // select
    string sql_count = string("select tk_max, tk_count from ticket_info where tk_id = ") + tk_id;
    if (mysql_query(&mysql_con, sql_count.c_str()) != 0)
    {
        return false;
    }

    r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    num = mysql_num_rows(r);
    if (num != 1)
    {
        return false;
    }

    row = mysql_fetch_row(r);
    int tk_max = atoi(row[0]);
    int tk_count = atoi(row[1]);
    if (tk_count != 0)
    {
        tk_count--;
    }

    mysql_free_result(r);

    // update
    string sql_update = string("update ticket_info set tk_count = ") + to_string(tk_count) + string(" where tk_id = ") + tk_id;
    if (mysql_query(&mysql_con, sql_count.c_str()) != 0)
    {
        return false;
    }

    // delete
    string sql_del = string("delete from ticket_res where res_id = ") + res_id;
    if (mysql_query(&mysql_con, sql_count.c_str()) != 0)
    {
        return false;
    }

    return true;
}

void Recv_Obj::User_dl()
{
    string user_tel = m_jmj["user_tel"];
    string user_password = m_jmj["user_password"];

    cout << m_jmj.JTP_ToStyString() << endl;

    string user_name;

    MySQLdb mysqlcli;
    if (!mysqlcli.Connect_toDb())
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    bool tmp = mysqlcli.Db_user_dl(user_name, user_tel, user_password);
    if (!tmp)
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    m_jmj.JTP_Clear();
    m_jmj.JTP_Add("status", "OK");
    m_jmj.JTP_Add("user_name", user_name);
    m_jmj.JTP_Send(c, m_jmj);

    return;
}

void Recv_Obj::User_zc()
{
    string user_tel = m_jmj["user_tel"];
    string user_name = m_jmj["user_name"];
    string user_password = m_jmj["user_password"];

    cout << m_jmj.JTP_ToStyString() << endl;

    MySQLdb mysqlcli;
    if (!mysqlcli.Connect_toDb())
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    bool tmp = mysqlcli.Db_user_zc(user_name, user_tel, user_password);
    if (!tmp)
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    m_jmj.JTP_Clear();
    m_jmj.JTP_Add("status", "OK");
    m_jmj.JTP_Send(c, m_jmj);

    return;
}

void Recv_Obj::Show_YuYue()
{
    JTP rjmj;
    MySQLdb clidb;
    if (!clidb.Connect_toDb())
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    if (!clidb.Db_show_yuyue(rjmj))
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    rjmj.JTP_Add("status", "OK");
    rjmj.JTP_Send(c, rjmj);
    return;
}

void Recv_Obj::User_yd()
{
    string user_tel = m_jmj["user_tel"];
    string tk_id = m_jmj["tk_id"];

    MySQLdb clidb;
    if (!clidb.Connect_toDb())
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERRS");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    if (!clidb.Db_user_yd(user_tel, tk_id))
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    m_jmj.JTP_Clear();
    m_jmj.JTP_Add("status", "OK");
    m_jmj.JTP_Send(c, m_jmj);

    return;
}

void Recv_Obj::Show_user_yd()
{
    JTP rjmj;
    string user_tel = m_jmj["user_tel"];
    MySQLdb clidb;
    if (!clidb.Connect_toDb())
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    if (!clidb.Db_show_user_yd(rjmj, user_tel))
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    rjmj.JTP_Add("status", "OK");
    rjmj.JTP_Send(c, rjmj);
    return;
}

void Recv_Obj::Delete_user_yd()
{
    string res_id = m_jmj["res_id"];

    JTP rjmj;
    MySQLdb clidb;
    if (!clidb.Connect_toDb())
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    if (!clidb.Db_delete_user_yd(res_id))
    {
        m_jmj.JTP_Clear();
        m_jmj.JTP_Add("status", "ERR");
        m_jmj.JTP_Send(c, m_jmj);
        return;
    }

    rjmj.JTP_Add("status", "OK");
    rjmj.JTP_Send(c, rjmj);
    return;
}

void Recv_Obj::CallBack_Fun()
{
    // char buff[128] = {0};
    int n = this->m_jmj.JTP_Recv(c, &m_jmj);
    if (n <= 0)
    {
        plib->MyLibevent_Del(this);
        delete this;
        return;
    }

    int op = m_jmj.JTP_toInt("type");
    switch (op)
    {
    case DL:
        User_dl();
        break;
    case ZC:
        User_zc();
        break;
    case CKYY:
        Show_YuYue();
        break;
    case YD:
        User_yd();
        break;
    case YDXX:
        Show_user_yd();
        break;
    case QXYD:
        Delete_user_yd();
        break;
    case TC:
        break;
    default:
        cout << "无效输入" << endl;
        break;
    }

    /*
    Json::Value val;
    Json::Reader Read;

    Read.parse(buff, val);
    cout << "recv" << val.toStyledString() << endl;

    val.clear();
    val["status"] = "OK";//测试
    val["user_name"] = "小白";//测试
    send(c,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
    */
}

void Accept_Obj::CallBack_Fun()
{
    int c = accept(sockfd, NULL, NULL);
    if (c == -1)
    {
        return;
    }

    Recv_Obj *precv = new Recv_Obj(c, plib);
    if (precv == NULL)
    {
        close(c);
        return;
    }

    plib->MyLibevent_Add(c, precv);
    cout << "accept client" << endl;
}

bool MyLibevent::MyLibevent_Init()
{
    m_base = event_init();
    if (m_base == NULL)
    {
        return false;
    }
    return true;
}

bool MyLibevent::MyLibevent_Add(int fd, Sock_Obj *pobj)
{
    if (pobj == NULL)
    {
        return false;
    }

    struct event *pev = event_new(m_base, fd, EV_READ | EV_PERSIST, G_CALLBACK_FUN, pobj);
    if (pev == NULL)
    {
        return false;
    }

    pobj->ev = pev;
    event_add(pev, NULL);
    return true;
}

void MyLibevent::MyLibevent_Del(Sock_Obj *pobj)
{
    if (pobj != NULL)
    {
        event_free(pobj->ev);
    }
}

void MyLibevent::MyLibevent_Loop()
{
    if (m_base != NULL)
    {
        event_base_dispatch(m_base);
    }
}

int main()
{
    Socket sock;
    if (!sock.Socket_Init())
    {
        exit(1);
    }

    MyLibevent *plib = new MyLibevent();
    if (plib == NULL)
    {
        exit(1);
    }

    plib->MyLibevent_Init();

    Accept_Obj *pobj = new Accept_Obj(sock.Get_Sockfd(), plib);

    plib->MyLibevent_Add(sock.Get_Sockfd(), pobj);

    plib->MyLibevent_Loop();

    delete pobj;
    delete plib;

    return 0;
}