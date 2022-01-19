//
// Created by nero on 27/6/2020.
//

#ifndef CLIENT_CPP_PACKETSTRUCTS_H
#define CLIENT_CPP_PACKETSTRUCTS_H

/**************************************************************************
  * request
**************************************************************************/
struct login_request {
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = 16;
    unsigned char msgType = LOGIN_REQUEST;
    unsigned char name[10] = {0};
    unsigned char padding[2] = {0};
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
    //    ~speak_request()
//    {
//        delete[]msg;
//        delete[]padding;
//        msg = nullptr;
//        padding = nullptr;
//    }
}__attribute__((packed));
struct logout_request {
    unsigned char version = EE122_VALID_VERSION;
    unsigned short length = EE122_VALID_VERSION;
    unsigned char msgType = LOGOUT;
}__attribute__((packed));
/**************************************************************************
  * response
**************************************************************************/
struct login_reply {
    unsigned char error_code;
    unsigned int hp;
    unsigned int exp;
    unsigned char x;
    unsigned char y;
    unsigned char padding;
}__attribute__((packed));

struct move_notify {
    unsigned char name[10];
    unsigned char x;
    unsigned char y;
    unsigned int hp;
    unsigned int exp;
}__attribute__((packed));

struct attack_notify {
    unsigned char attacker_name[10];
    unsigned char victim_name[10];
    unsigned char damage;
    unsigned int hp; // Specifies the updated HP of the victim in network-byte-order. Cannot be negative
    unsigned char padding[3];
}__attribute__((packed));

struct speak_notify {
    unsigned int len = 0;
    unsigned char* speaker = nullptr;
    unsigned char* msg = nullptr;
}__attribute__((packed));

struct logout_notify{
    unsigned char name[10];
};


struct invalid_state{
    unsigned char error_code;
}__attribute__((packed));

#endif //CLIENT_CPP_PACKETSTRUCTS_H
