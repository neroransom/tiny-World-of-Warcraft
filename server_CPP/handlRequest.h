//
// Created by nero on 4/7/2020.
//

#ifndef SERVER_CPP_HANDLREQUEST_H
#define SERVER_CPP_HANDLREQUEST_H

#include "funcitonalUnity.h"

using namespace std;

class handleRequest {
    int fd;
    unsigned char msgType;
    unsigned char *arg = nullptr;

    login_request *login = nullptr;
    move_request *move = nullptr;
    attack_request *attack = nullptr;
    speak_request *speak = nullptr;

    login_reply *login_r = nullptr;
    move_notify *move_r = nullptr;
    attack_notify *attack_r = nullptr;
    speak_notify *speak_r = nullptr;
    logout_notify *logout_r = nullptr;
    invalid_state *invalid = nullptr;

    int p = EE122_SOCKET_BUFFER_SIZE;
    short len = 0;   //recv_msg_len

public:
    int brcst = 0;

    handleRequest(int fd) {
        this->fd = fd;
    }

    int initial(unsigned char *buff) {
        if (buff[0] != EE122_VALID_VERSION) {
            on_malformed_message_from_server();
        }
        memcpy(&len + 0, buff + 1, 2);
        len = htons(len);
        if (len % 4 != 0) {                              //aligned
            on_malformed_message_from_server();
        }
        if (len > p) {
            return p;
        }
        msgType = buff[3];
        arg = new unsigned char[len - 4];
        memcpy(arg + 0, buff + 4, len - 4);
        for (int i = 0; i < EE122_SOCKET_BUFFER_SIZE; ++i) {
            if (i < EE122_SOCKET_BUFFER_SIZE - len) {
                buff[i] = buff[len + i];
            } else {
                buff[i] = '\0';
            }
        }
        p -= len;
        return 0;
    }

    int handle() {
        if(user_map_r.count(fd)==0&&msgType!=LOGIN_REQUEST){
            msgType = INVALID_STATE;
        }
        switch (msgType) {
            case (LOGIN_REQUEST): {
                login = new login_request();
                memcpy(&login->name, arg + 0, 10);
                break;
                //TODO
            }
            case (MOVE): {
                move = new move_request();
                memcpy(&move->direction, arg + 0, 1);
                brcst = 1;
                break;
            }
            case (ATTACK): {
                attack = new attack_request();
                memcpy(&attack->victimName, arg + 0, 10);
                return 1;
                break;
            }
            case(LOGOUT):{
                brcst =  1;
                return -1;
            }
        }
        return 0;
    };

    int process() {
        int len = 0;
        switch (msgType) {
            case (LOGIN_REQUEST): {
                login_r = new login_reply();
                int hp = -1, exp = -1, x = -1, y = -1;
                string name = (reinterpret_cast<const char *>(login->name));
                if (user_map_l.count(name) == 0) {
                    fstream f;
                    f.open("user/" + name, ios::app | ios::in | ios::out);
                    if (is_emptyfile(f)) {
                        srand(time(NULL));
                        hp = rand() % 20 + 100;
                        exp = 0;
                        x = rand() % 100;
                        y = rand() % 100;
                        writeUserFile(name, hp, exp, x, y);;
                    } else {
                        readUserFile(name, hp, exp, x, y);
                    }
                    cout << hp << exp << x << y << endl;
                    msgType = LOGIN_REPLY;
                    login_r->error_code = 0;
                    login_r->hp = hp;
                    login_r->exp = exp;
                    login_r->x = x;
                    login_r->y = y;
                    len = login_r->length;
                    user_map_l[name] = fd;
                    user_map_r[fd] = name;
                    brcst = -1;
                } else {
                    if (fd == user_map_l.find(name)->second) {
                        login_r = nullptr;
                        msgType = INVALID_STATE;
                        invalid = new invalid_state();
                        invalid->error_code = 1;
                        len = invalid->length;
                    } else {
                        msgType = LOGIN_REPLY;
                        login_r->error_code = 1;
                        len = login_r->length;
                    }
                }
                break;
            }
            case (MOVE): {
                msgType = MOVE_NOTIFY;
                move_r = new move_notify();
                int hp = -1, exp = -1, x = -1, y = -1;
                string name = user_map_r.find(fd)->second;
                readUserFile(name, hp, exp, x, y);
                srand(time(NULL));
                switch (move->direction) {
                    case NORTH: {
                        y -= 3;
                        if (y < 0) { y += 100; }
                        break;
                    }
                    case SOUTH: {
                        y += 3;
                        if (y >= 100) { y -= 100; }
                        break;
                    }
                    case WEST: {
                        x -= 3;
                        if (x < 0) { x += 100; }
                        break;
                    }
                    case EAST: {
                        x += 3;
                        if (x >= 100) { x -= 100; }
                        break;
                    }
                }
                writeUserFile(name, hp, exp, x, y);
                len = move_r->length;
                strcpy(reinterpret_cast<char*>(move_r->name),name.c_str());
                move_r->hp = hp;
                move_r->exp = exp;
                move_r->x = x;
                move_r->y = y;
                break;
            }
            case (ATTACK): {
                msgType = ATTACK_NOTIFY;
                attack_r = new attack_notify();
                int hp = -1, exp = -1, x = -1, y = -1;
                string victimName = (reinterpret_cast<const char *>(attack->victimName));
                readUserFile(victimName, hp, exp, x, y);
                if (user_map_l.count(victimName) == 0) {return -1;}
                if (user_map_r.find(fd)->second == victimName) {return -1;}
                strcpy(reinterpret_cast<char *>(attack_r->attacker_name), user_map_r.find(fd)->second.c_str());
                strcpy(reinterpret_cast<char *>(attack_r->victim_name), victimName.c_str());
                srand(time(NULL));
                attack_r->damage = rand() % 10 +11;
                hp -= attack_r->damage;
                exp+= attack_r->damage;
                attack_r->hp = hp;
                writeUserFile(victimName, hp, exp, x, y);
                len = attack_r->length;
                break;
            }
            case(LOGOUT):{
                msgType = LOGOUT_NOTIFY;
                logout_r = new logout_notify();
                string name = user_map_r.find(fd)->second;
                user_map_l.erase(user_map_r.find(fd)->second);
                user_map_r.erase(fd);
                strcpy(reinterpret_cast<char *>(logout_r->name), name.c_str());
                break;
            }
            case(INVALID_STATE):
            {
                invalid = new invalid_state();
                invalid->error_code = 0;
                len = invalid->length;
                break;
            }
        }
        return len;
    }


    unsigned char *makepacket() {
        unsigned char *buff;
        switch (msgType) {
            case (LOGIN_REPLY): {
                buff = new unsigned char[login_r->length];
                bzero(buff, login_r->length);
                login_r->length = htons(login_r->length);
                login_r->hp = htons(login_r->hp);
                login_r->exp = htons(login_r->exp);
                memcpy(buff + 0, reinterpret_cast<unsigned char *>(&login_r->version), 1);
                memcpy(buff + 1, reinterpret_cast<unsigned char *>(&login_r->length), 2);
                memcpy(buff + 3, reinterpret_cast<unsigned char *>(&login_r->msgType), 1);
                memcpy(buff + 4, reinterpret_cast<unsigned char *>(&login_r->error_code), 1);
                memcpy(buff + 5, reinterpret_cast<unsigned char *>(&login_r->hp), 4);
                memcpy(buff + 9, reinterpret_cast<unsigned char *>(&login_r->exp), 4);
                memcpy(buff + 13, reinterpret_cast<unsigned char *>(&login_r->x), 1);
                memcpy(buff + 14, reinterpret_cast<unsigned char *>(&login_r->y), 1);
                memcpy(buff + 15, reinterpret_cast<unsigned char *>(&login_r->padding), 1);
                break;
            }
            case (INVALID_STATE): {
                buff = new unsigned char[invalid->length];
                bzero(buff, invalid->length);
                invalid->length = htons(invalid->length);
                memcpy(buff + 0, reinterpret_cast<unsigned char *>(&invalid->version), 1);
                memcpy(buff + 1, reinterpret_cast<unsigned char *>(&invalid->length), 2);
                memcpy(buff + 3, reinterpret_cast<unsigned char *>(&invalid->msgType), 1);
                memcpy(buff + 4, reinterpret_cast<unsigned char *>(&invalid->error_code), 1);
                break;
            }
            case (MOVE_NOTIFY): {
                buff = new unsigned char[move_r->length];
                bzero(buff, move_r->length);
                move_r->hp = htonl(move_r->hp);
                move_r->exp = htonl(move_r->exp);
                move_r->length = htons(move_r->length);
                memcpy(buff + 0, reinterpret_cast<unsigned char *>(&move_r->version), 1);
                memcpy(buff + 1, reinterpret_cast<unsigned char *>(&move_r->length), 2);
                memcpy(buff + 3, reinterpret_cast<unsigned char *>(&move_r->msgType), 1);
                memcpy(buff + 4, reinterpret_cast<unsigned char *>(move_r->name), 10);
                memcpy(buff + 14, reinterpret_cast<unsigned char *>(&move_r->x), 1);
                memcpy(buff + 15, reinterpret_cast<unsigned char *>(&move_r->y), 1);
                memcpy(buff + 16, reinterpret_cast<unsigned char *>(&move_r->hp), 4);
                memcpy(buff + 20, reinterpret_cast<unsigned char *>(&move_r->exp), 4);
                break;
            }
            case (ATTACK_NOTIFY):{
                if(attack_r->hp<=0){
                    int hp = -1, exp = -1, x = -1, y = -1;
                    string victimName = (reinterpret_cast<const char *>(attack->victimName));
                    srand(time(NULL));
                    hp = rand() % 20 + 30;
                    exp = 0;
                    x = rand() % 100;
                    y = rand() % 100;
                    writeUserFile(victimName,hp,exp,x,y);
                    buff =userInform(user_map_l.find(victimName)->second);
                    brcst = 1;
                }else {
                    buff = new unsigned char[attack_r->length];
                    bzero(buff, attack_r->length);
                    attack_r->hp = htonl(attack_r->hp);
                    attack_r->length = htons(attack_r->length);
                    memcpy(buff + 0, reinterpret_cast<unsigned char *>(&attack_r->version), 1);
                    memcpy(buff + 1, reinterpret_cast<unsigned char *>(&attack_r->length), 2);
                    memcpy(buff + 3, reinterpret_cast<unsigned char *>(&attack_r->msgType), 1);
                    memcpy(buff + 4, reinterpret_cast<unsigned char *>(attack_r->attacker_name), 10);
                    memcpy(buff + 14, reinterpret_cast<unsigned char *>(attack_r->victim_name), 10);
                    memcpy(buff + 24, reinterpret_cast<unsigned char *>(&attack_r->damage), 1);
                    memcpy(buff + 25, reinterpret_cast<unsigned char *>(&attack_r->hp), 4);
                    brcst = 1;
                }
                break;
            }
            case(LOGOUT_NOTIFY):{
                buff = new unsigned char[logout_r->length];
                bzero(buff, logout_r->length);
                logout_r->length = htons(logout_r->length);
                memcpy(buff + 0, reinterpret_cast<unsigned char *>(&logout_r->version), 1);
                memcpy(buff + 1, reinterpret_cast<unsigned char *>(&logout_r->length), 2);
                memcpy(buff + 3, reinterpret_cast<unsigned char *>(&logout_r->msgType), 1);
                memcpy(buff + 4, reinterpret_cast<unsigned char *>(logout_r->name), 10);
                break;
            }
        }
        return buff;
    };

    ~handleRequest() {
        delete[]arg;
        switch (msgType) {
            case (LOGIN_REPLY): {
                delete login;
                delete login_r;
                delete invalid;
                break;
            }
            case (MOVE_NOTIFY): {
                delete move;
                delete move_r;
                break;
            }
            case (ATTACK_NOTIFY): {
                delete attack;
                break;
            }
            case (LOGOUT_NOTIFY): {
                delete logout_r;
                break;
            }
        }
    }
};

#endif //SERVER_CPP_HANDLREQUEST_H
