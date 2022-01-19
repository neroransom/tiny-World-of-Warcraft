//
// Created by nero on 27/6/2020.
//
#ifndef CLIENT_CPP_MAKEREQUEST_H
#define CLIENT_CPP_MAKEREQUEST_H

#include <iostream>
#include "packetStructs.h"
#include <arpa/inet.h>
#include "multiPlayer.h"

using namespace std;

class makeRequest {
    string command;
    string msgType;
    string arg;
    login_request *login = nullptr;
    move_request *move = nullptr;
    attack_request *attack = nullptr;
    speak_request *speak = nullptr;
    logout_request *logout = nullptr;
    unsigned char *buff = nullptr;
    unsigned char name[10];


public:
    makeRequest(string command) {
        this->command = command;
    }

    int handle() {
        int len = 0;
        // split command
        msgType = command;
        for (int i = 0; i < command.size(); ++i) {
            if (command[i] == ' ') {
                msgType = command.substr(0, i);
                arg = command.substr(i + 1);
                break;
            }
        }
        //initialize;
        if (msgType == "login") {
            if(!check_player_name(arg)||arg.length()==0)
            {
                on_invalid_name();
                return -1;
            }
            login = new login_request();
            strcpy(reinterpret_cast<char *> (login->name), arg.c_str());
            len = login->length;
            buff = new unsigned char[len];
            login->length = htons(login->length);
            PLAYER.name = arg;
        } else if (msgType == "move") {
            move = new move_request();
            if (arg == "north") {
                move->direction = NORTH;
            } else if (arg == "south") {
                move->direction = SOUTH;
            } else if (arg == "east") {
                move->direction = EAST;
            } else if (arg == "west") {
                move->direction = WEST;
            } else{
                return -1;
            }
            len = move->length;
            buff = new unsigned char[len];
            move->length = htons(move->length);
        } else if (msgType == "attack") {
            if(!PLAYER_LIST.isVisible(arg.c_str()))
            {
                on_not_visible();
                return -1;
            }
            attack = new attack_request();
            strcpy(reinterpret_cast<char *> (attack->victimName), arg.c_str());
            len = attack->length;
            buff = new unsigned char[len];
            attack->length = htons(attack->length);
        } else if (msgType == "speak") {
            if (!(check_player_message(arg.c_str()))||arg.length()==0) {
                on_invalid_message();
                return -1;
            }
            speak = new speak_request();
            int mlen = arg.size();
            int plen = 4 - mlen % 4;
            speak->msg = new unsigned char[mlen];
            strcpy(reinterpret_cast<char *> (speak->msg), arg.c_str());
            speak->padding = new unsigned char[plen];
            speak->length = mlen + plen + 4;
            speak->mlen = mlen;
            speak->plen = plen;
            len = speak->length;
            buff = new unsigned char[len];
            speak->length = htons(speak->length);
        } else if (msgType == "logout") {
            logout = new logout_request();
            len = logout->length;
            buff = new unsigned char[len];
            logout->length = htons(logout->length);
        }

        return len;
    }

    unsigned char *createPacket() {
        if (msgType == "login") {
            memcpy(buff + 0, reinterpret_cast<unsigned char *>(&login->version), 1);
            memcpy(buff + 1, reinterpret_cast<unsigned char *>(&login->length), 2);
            memcpy(buff + 3, reinterpret_cast<unsigned char *>(&login->msgType), 1);
            memcpy(buff + 4, login->name, 10);
            memcpy(buff + 14, login->padding, 2);
        } else if (msgType == "move") {
            memcpy(buff + 0, reinterpret_cast<unsigned char *>(&move->version), 1);
            memcpy(buff + 1, reinterpret_cast<unsigned char *>(&move->length), 2);
            memcpy(buff + 3, reinterpret_cast<unsigned char *>(&move->msgType), 1);
            memcpy(buff + 4, reinterpret_cast<unsigned char *>(&move->direction), 1);
            memcpy(buff + 5, move->padding, 2);
        } else if (msgType == "attack") {
            memcpy(buff + 0, reinterpret_cast<unsigned char *>(&attack->version), 1);
            memcpy(buff + 1, reinterpret_cast<unsigned char *>(&attack->length), 2);
            memcpy(buff + 3, reinterpret_cast<unsigned char *>(&attack->msgType), 1);
            memcpy(buff + 4, attack->victimName, 10);
            memcpy(buff + 14, attack->padding, 2);
        } else if (msgType == "speak") {
            memcpy(buff + 0, reinterpret_cast<unsigned char *>(&speak->version), 1);
            memcpy(buff + 1, reinterpret_cast<unsigned char *>(&speak->length), 2);
            memcpy(buff + 3, reinterpret_cast<unsigned char *>(&speak->msgType), 1);
            memcpy(buff + 4, speak->msg, speak->mlen);
            for (int i = 0; i < speak->plen; ++i) { speak->padding[i] ='\0'; }
            memcpy(buff + 4 + speak->mlen, speak->padding, speak->plen);
        } else if (msgType == "logout") {
            memcpy(buff + 0, reinterpret_cast<unsigned char *>(&logout->version), 1);
            memcpy(buff + 1, reinterpret_cast<unsigned char *>(&logout->length), 2);
            memcpy(buff + 3, reinterpret_cast<unsigned char *>(&logout->msgType), 1);
        }
        return buff;
    }

    ~makeRequest() {
        delete[]buff;
        buff = nullptr;

        if (login != nullptr) {
            delete[]login;
            login = nullptr;
        } else if (move != nullptr) {
            delete[]move;
            move = nullptr;
        } else if (attack != nullptr) {
            delete[]attack;
            attack = nullptr;
        } else if (speak != nullptr) {
            delete[]speak->msg;
            delete[]speak->padding;
            delete[]speak;
            speak = nullptr;
        } else if (logout != nullptr) {
            delete[]logout;
        }
    }
};


#endif //CLIENT_CPP_MAKEREQUEST_H
