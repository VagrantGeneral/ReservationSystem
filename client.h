#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jsoncpp/json/json.h>
#include "../JTP/JTP.h"

using namespace std;

class Client
{
public:
    Client() : sockfd(-1), ips("127.0.0.1"), port(6000) { dl_status = false; }

    Client(const string &ip, short port) : sockfd(-1), ips(ip), port(port) { dl_status = false; }

    bool Connect();

    void Run();

private:
    void Show_Menu();

    void User_zc();

    void User_dl();

    void Show_YuYue();

    void User_yd();

    void Show_user_yd();

    void Delete_user_yd();

private:
    int sockfd;
    string ips;
    short port;

    bool dl_status = false;

private:
    int user_op;
    string user_name;
    string user_tel;

    map<int, string> m_map;
    map<int, string> m_map_yd;
};