#include "client.h"

enum OP_TYPE
{
    DL = 1,
    ZC = 2,
    CKYY = 3,
    YD = 4,
    YDXX = 5,
    QXYD = 6,
    TC = 7
};

bool Client::Connect()
{
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("create socket failed\n");
        return false;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ips.c_str());

    int res = connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (res == -1)
    {
        printf("connect err\n");
        return false;
    }
    return true;
}

void Client::User_zc()
{
    cout << "请输入手机号" << endl;
    cin >> user_tel;
    cout << "请输入user_name" << endl;
    cin >> user_name;

    string password1, password2;
    cout << "请输入密码" << endl;
    cin >> password1;
    cout << "请输入密码_again" << endl;
    cin >> password2;

    if (user_tel.empty() || user_name.empty() || password1.empty() || password2.empty())
    {
        cout << "the information don't empty" << endl;
        return;
    }

    if (password1.compare(password2) != 0)
    {
        cout << "two password not same" << endl;
        return;
    }

    JTP jmj;
    jmj.JTP_Add("type", (int)ZC);
    jmj.JTP_Add("user_tel", user_tel);
    jmj.JTP_Add("user_name", user_name);
    jmj.JTP_Add("user_password", password1);

    jmj.JTP_Send(sockfd, jmj);

    int n = jmj.JTP_Recv(sockfd, &jmj);
    if (n <= 0)
    {
        cout << "ERR" << endl;
    }

    if (jmj["status"].compare("OK") != 0)
    {
        cout << "ZC Failed" << endl;
        return;
    }
    cout << "ZC SUCCESSFUL" << endl;

    dl_status = true;
}

void Client::User_dl()
{
    cout << "请输入手机号" << endl;
    cin >> user_tel;
    cout << "请输入密码" << endl;
    string password;
    cin >> password;

    JTP jmj;
    jmj.JTP_Add("type", (int)DL);
    jmj.JTP_Add("user_tel", user_tel);
    jmj.JTP_Add("user_password", password);

    jmj.JTP_Send(sockfd, jmj);

    int n = jmj.JTP_Recv(sockfd, &jmj);
    if (n <= 0)
    {
        cout << "ERR" << endl;
    }

    if (jmj["status"].compare("OK") != 0)
    {
        cout << "登陆失败" << endl;
        return;
    }
    cout << "登陆成功" << endl;

    dl_status = true;
    user_name = jmj["user_name"];

    return;
}

void Client::Show_YuYue()
{
    JTP jmj;
    jmj.JTP_Add("type", (int)CKYY);
    jmj.JTP_Send(sockfd, jmj);

    int n = jmj.JTP_Recv(sockfd, &jmj);
    if (n <= 0)
    {
        cout << "ERR" << endl;
        return;
    }

    if (jmj["status"].compare("OK") != 0)
    {
        cout << "CKYY ERR" << endl;
        return;
    }

    // show;
    // cout << jmj.JTP_ToStyString() << endl;

    int num = jmj.JTP_toInt("num");
    if (num == 0)
    {
        cout << "no information" << endl;
    }
    m_map.clear();
    cout << "[id]   [name]  [max]  [count]  [date]" << endl;
    for (int i = 0; i < num; i++)
    {
        m_map.insert({i, (jmj.GetVNum())["arr"][i]["tk_id"].asString()});
        cout << i << "  ";
        cout << (jmj.GetVNum())["arr"][i]["tk_name"].asString() << "   ";
        cout << (jmj.GetVNum())["arr"][i]["tk_max"].asString() << "   ";
        cout << (jmj.GetVNum())["arr"][i]["tk_count"].asString() << "   ";
        cout << (jmj.GetVNum())["arr"][i]["tk_date"].asString() << endl;
    }
}

void Client::User_yd()
{
    Show_YuYue();
    cout << "please input the num of YD" << endl;
    int index = -1;
    cin >> index;

    map<int, string>::iterator pos = m_map.find(index);
    if (pos == m_map.end())
    {
        cout << "input err" << endl;
        return;
    }

    string tk_id = pos->second;

    JTP jmj;
    jmj.JTP_Add("type", (int)YD);
    jmj.JTP_Add("user_tel", user_tel);
    jmj.JTP_Add("tk_id", tk_id);

    jmj.JTP_Send(sockfd, jmj);

    int n = jmj.JTP_Recv(sockfd, &jmj);
    if (n <= 0)
    {
        cout << "ERR" << endl;
        return;
    }

    if (jmj["status"].compare("OK") != 0)
    {

        // cout << jmj.JTP_ToStyString() << endl;
        cout << "FAILED" << endl;
        return;
    }

    cout << "YD SUCCESSFUL" << endl;
    return;
}

void Client::Show_user_yd()
{
    JTP jmj;
    jmj.JTP_Add("type", (int)YDXX);
    jmj.JTP_Add("user_tel", user_tel);
    jmj.JTP_Send(sockfd, jmj);

    int n = jmj.JTP_Recv(sockfd, &jmj);
    if (n <= 0)
    {
        cout << "ERR" << endl;
        return;
    }

    if (jmj["status"].compare("OK") != 0)
    {
        cout << "YDXX ERR" << endl;
        return;
    }

    // show;
    // cout << jmj.JTP_ToStyString() << endl;

    int num = jmj.JTP_toInt("num");
    if (num == 0)
    {
        cout << "no information" << endl;
    }
    m_map_yd.clear();
    cout << "[id]   [name]  [date]" << endl;
    for (int i = 0; i < num; i++)
    {
        m_map_yd.insert({i, (jmj.GetVNum())["arr"][i]["res_id"].asString()});
        cout << i << "  ";
        cout << (jmj.GetVNum())["arr"][i]["tk_name"].asString() << "    ";
        cout << (jmj.GetVNum())["arr"][i]["yd_time"].asString() << endl;
    }
}

void Client::Delete_user_yd()
{
    JTP jmj;
    Show_user_yd();
    cout << "input the Number of QX" << endl;
    int index = 0;
    cin >> index;

    map<int, string>::iterator pos = m_map_yd.find(index);
    if (pos == m_map_yd.end())
    {
        cout << "input ERR" << endl;
        return;
    }
    string res_id = pos->second;

    jmj.JTP_Add("type", (int)QXYD);
    jmj.JTP_Add("res_id", res_id);

    jmj.JTP_Send(sockfd, jmj);

    int n = jmj.JTP_Recv(sockfd, &jmj);
    if (n <= 0)
    {
        cout << "ERR" << endl;
        return;
    }

    if (jmj["status"].compare("OK") != 0)
    {

        // cout << jmj.JTP_ToStyString() << endl;
        cout << "FAILED" << endl;
        return;
    }

    cout << "SUCCESSFUL" << endl;
    return;
}

void Client::Show_Menu()
{
    if (!dl_status)
    {
        cout << "--------------------------touri----------------------" << endl;
        cout << "1.DL                      2.ZC                   3.TC" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "op:" << endl;
        cin >> user_op;
        if (user_op == 3)
        {
            user_op = TC;
        }
    }
    else
    {
        cout << "----------------------" << user_name << "---------------------" << endl;
        cout << "1.CKYY      2.YD        3.YDXX       4.QX        5.TC" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "op:" << endl;
        cin >> user_op;
        user_op += 2;
    }
}

void Client::Run()
{
    while (1)
    {
        Show_Menu();
        switch (user_op)
        {
        case DL:
            User_dl();
            dl_status = true;
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
    }
}

int main()
{
    Client cli;
    if (!cli.Connect())
    {
        cout << "" << endl;
        cout << "连接失败" << endl;
        exit(0);
    }

    cli.Run();

    return 0;
}