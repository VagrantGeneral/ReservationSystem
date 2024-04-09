#include <iostream>
#include <string>
#include <cstring>
#include <string.h>
#include <sys/socket.h>
#include <jsoncpp/json/json.h>
// #include "JTP.h"

class JTP
{
private:
    Json::Value m_Jval;
    Json::Reader m_Jread;

public:
    Json::Value GetVNum()
    {
        return m_Jval;
    }
    Json::Reader GetRNum()
    {
        return m_Jread;
    }

public:
    // add the member
    void JTP_Add(std::string k_str, std::string v_str)
    {
        this->m_Jval[k_str] = v_str;
    }

    void JTP_Add(std::string K_str, int v_num)
    {
        this->m_Jval[K_str] = v_num;
    }

    void JTP_Add(std::string K_str, JTP mj)
    {
        this->m_Jval[K_str].append(mj.m_Jval);
    }

    // reverse string
    void JTP_Parse(std::string str)
    {
        this->m_Jread.parse(str, this->m_Jval);
    }

    // to string
    const std::string JTP_ToStyString()
    {
        std::string str = this->m_Jval.toStyledString();
        return str;
    }

    // to one string
    std::string operator[](std::string str)
    {
        std::string rstr = this->m_Jval[str].asString();
        return rstr;
    }

    int JTP_toInt(std::string str)
    {
        int n = this->m_Jval[str].asInt();
        return n;
    }

    // clear to 0
    void JTP_Clear()
    {
        this->m_Jval.clear();
    }

    // send json_string to fd
    void JTP_Send(int fd, JTP mj)
    {
        send(fd, mj.JTP_ToStyString().c_str(), strlen(mj.JTP_ToStyString().c_str()), 0);
    }

    // recv json_string from fd
    int JTP_Recv(int fd, JTP *rmj)
    {
        char buff[256] = {0};
        recv(fd, buff, 255, 0);
        rmj->JTP_Clear();
        rmj->JTP_Parse(buff);
        return strlen(buff);
    }
};