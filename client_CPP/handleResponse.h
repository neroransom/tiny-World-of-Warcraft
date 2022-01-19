//
// Created by nero on 29/6/2020.
//

#ifndef CLIENT_CPP_HANDLERESPONSE_H
#define CLIENT_CPP_HANDLERESPONSE_H

#include <iostream>
#include "packetStructs.h"
#include "constants.h"
#include <arpa/inet.h>
#include "multiPlayer.h"

using namespace std;
static int length;

class handleResponse {
    unsigned char msgType;
    unsigned char *arg = nullptr;
    int p = EE122_SOCKET_BUFFER_SIZE;
    short len = 0;   //recv_msg_len

    login_reply *login = nullptr;
    move_notify *move = nullptr;
    attack_notify *attack = nullptr;
    speak_notify *speak = nullptr;
    logout_notify *logout = nullptr;
    invalid_state *invalid = nullptr;
public:

    handleResponse() {

    }

    int initial(unsigned char *buff) {
        if (buff[0] != EE122_VALID_VERSION) { //version
            on_malformed_message_from_server();
        }
        memcpy(&len + 0, buff + 1, 2);
        len = htons(len);
        if(len%4!=0){                              //aligned
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
        switch (msgType) {
            case (LOGIN_REPLY): {
                login = new login_reply();
                memcpy(&login->error_code, arg + 0, 1);
                memcpy(&login->hp, arg + 1, 4);
                memcpy(&login->exp, arg + 5, 4);
                memcpy(&login->x, arg + 9, 1);
                memcpy(&login->y, arg + 10, 1);
                memcpy(&login->padding, arg + 11, 1);
                login->hp = htonl(login->hp);
                login->exp = htonl(login->exp);
                break;
            }
            case (MOVE_NOTIFY): {
                move = new move_notify();
                memcpy(&move->name, arg + 0, 10);
                memcpy(&move->x, arg + 10, 1);
                memcpy(&move->y, arg + 11, 1);
                memcpy(&move->hp, arg + 12, 4);
                memcpy(&move->exp, arg + 16, 4);
                move->hp = htonl(move->hp);
                move->exp = htonl(move->exp);
                break;
            }
            case (ATTACK_NOTIFY): {
                attack = new attack_notify();
                memcpy(&attack->attacker_name, arg + 0, 10);
                memcpy(&attack->victim_name, arg + 10, 10);
                memcpy(&attack->damage, arg + 20, 1);
                memcpy(&attack->hp, arg + 21, 4);
                memcpy(&attack->padding, arg + 25, 3);
                attack->hp = htonl(attack->hp);
                break;
            }
            case (SPEAK_NOTIFY): {
                if(len<=14){return -1;}
                speak = new speak_notify();
                speak->msg = new unsigned char[len-14];
                speak->speaker = new unsigned char[10];
                memcpy(speak->speaker, arg + 0, 10);
                memcpy(speak->msg, arg + 10, len - 14);
                string get = reinterpret_cast<const char*>(speak->msg);
                speak->len = get.size();
                if(get=="speak_message")exit(1);
                break;
            }
            case (LOGOUT_NOTIFY): {
                logout = new logout_notify();
                memcpy(&logout->name, arg + 0, 10);
            }
            case (INVALID_STATE): {
                invalid = new invalid_state();
                memcpy(&invalid->error_code, arg + 0, 1);
            }
        }
        return 0;
    }

    int processResponse() {
        switch (msgType) {
            case (LOGIN_REPLY): {
                PLAYER.login(login->error_code,login->x,login->y,login->hp,login->exp);
                break;
            }
            case (MOVE_NOTIFY): {
                string name(reinterpret_cast<const char *>(move->name));
                if(!check_player_name(name))
                {
                    on_malformed_message_from_server();
                }
                if (PLAYER.name == name) {
                   PLAYER.updatePlayer(reinterpret_cast<const char *>(move->name), move->x, move->y, move->hp, move->exp);
                   catchInSight();
                } else{
                    PLAYER_LIST.updatePlayerList(reinterpret_cast<const char *>(move->name), move->x, move->y, move->hp, move->exp);
                }
                break;
            }
            case (ATTACK_NOTIFY): {
                attackWitness(reinterpret_cast<const char *>(&attack->attacker_name),
                                 reinterpret_cast<const char *>(&attack->victim_name), attack->damage, attack->hp);
                break;
            }
            case (SPEAK_NOTIFY): {
                string msgStr = reinterpret_cast<const char *>(speak->msg);
                if(!check_player_name(reinterpret_cast<const char *>(speak->speaker)))
                {
                    on_malformed_message_from_server();
                }
                if(!check_player_message(reinterpret_cast<const char *>(speak->speaker)))
                {
                    on_malformed_message_from_server();
                }
                if(speak->len>EE122_MAX_MSG_LENGTH||speak->len==0||msgStr.length()==len)
                {
                    on_malformed_message_from_server();
                    break;
                }
                on_speak_notify(reinterpret_cast<const char *>(speak->speaker),
                                reinterpret_cast<const char *>(speak->msg));
                break;
            }
            case (LOGOUT_NOTIFY): {
                string temp;
                PLAYER_LIST.deletePlayer(temp = reinterpret_cast<const char *>(logout->name));
                break;
            }
            case (INVALID_STATE): {
                on_invalid_state(invalid->error_code);
                break;
            }
        }
        return 1;
    };

    ~handleResponse() {
        delete[]arg;
        switch (msgType) {
            case (LOGIN_REPLY): {
                delete login;
                break;
            }
            case (MOVE_NOTIFY): {
                delete move;
                break;
            }
            case (ATTACK_NOTIFY): {
                delete attack;
                break;
            }
            case (SPEAK_NOTIFY): {
                delete[]speak->speaker;
                delete[]speak->msg;
                delete speak;
                break;
            }
            case (LOGOUT_NOTIFY): {
                delete logout;
                break;
            }
        }
    }

    int getP() { return p; }

};
#endif //CLIENT_CPP_HANDLERESPONSE_H