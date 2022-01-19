//
// Created by nero on 27/6/2020.
//

#ifndef CLIENT_CPP_PACKETSTRUCTS_H
#define CLIENT_CPP_PACKETSTRUCTS_H
#include "constants.h"
#include <map>
#include <list>
#include <string>
using namespace std;
/**************************************************************************
  * request
**************************************************************************/
struct login_request {
    unsigned char name[10] = {0};
}__attribute__((packed));

struct move_request {
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 8;
    unsigned char msgType = MOVE;
    unsigned char direction{};
    unsigned char padding[3] = {0};
}__attribute__((packed));

struct attack_request {
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 16;
    unsigned char msgType = ATTACK;
    unsigned char victimName[10] = {0};
    unsigned char padding[2] = {0};
}__attribute__((packed));


struct speak_request {
public:
    unsigned short length{};
    unsigned char msgType = SPEAK;
    unsigned char *msg = nullptr;
    unsigned char *padding = nullptr;
    int mlen = 0;
    int plen = 0;
    unsigned char version = EE122_VALID_VERSION;
}__attribute__((packed));

struct logout_request {
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 4;
    unsigned char msgType = LOGOUT;
}__attribute__((packed));
/**************************************************************************
  * response
**************************************************************************/
struct login_reply {
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 16;
    unsigned char msgType = LOGIN_REPLY;
    unsigned char error_code = 0;
    unsigned int hp = 0;
    unsigned int exp = 0;
    unsigned char x = 0;
    unsigned char y = 0;
    unsigned char padding = 0;
}__attribute__((packed));

struct move_notify {
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 24;
    unsigned char msgType = MOVE_NOTIFY;
    unsigned char name[10];
    unsigned char x = 0;
    unsigned char y = 0;
    unsigned int hp = 0;
    unsigned int exp = 0;
}__attribute__((packed));

struct attack_notify {
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 32;
    unsigned char msgType = ATTACK_NOTIFY;
    unsigned char attacker_name[10];
    unsigned char victim_name[10];
    unsigned char damage;
    int hp; // Specifies the updated HP of the victim in network-byte-order. Cannot be negative
    unsigned char padding[3];
}__attribute__((packed));

struct speak_notify {
    unsigned int len = 0;
    unsigned char* speaker = nullptr;
    unsigned char* msg = nullptr;
}__attribute__((packed));

struct logout_notify{
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 16;
    unsigned char msgType = LOGOUT_NOTIFY;
    unsigned char name[10];
};


struct invalid_state{
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 8;
    unsigned char msgType = INVALID_STATE;
    unsigned char error_code;
}__attribute__((packed));

struct pack{
    int fd;
    unsigned char* buff;
    unsigned char* vicebuf;
    int len;
    int brcst = 0;
    pack(int fd,unsigned char* buff,int len,int brcst)
    {
        this->fd = fd;
        this->buff = buff;
        this->len = len;
        this->brcst = brcst;
    }
}__attribute__((packed));

static map<int,string> user_map_r;
static map<string,int> user_map_l;
static list<int> client_fd;                //用于装连接进来的客户端的套接字文件描述符
#endif //CLIENT_CPP_PACKETSTRUCTS_H
