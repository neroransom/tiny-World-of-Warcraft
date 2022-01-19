//
// Created by nero on 4/7/2020.
//

#ifndef SERVER_CPP_FUNCITONALUNITY_H
#define SERVER_CPP_FUNCITONALUNITY_H

#include <iostream>
#include "packetStructs.h"
#include "messages.h"
#include "constants.h"
#include <arpa/inet.h>
#include <string>
#include <fstream>
#include <ctime>
using namespace std;


bool is_emptyfile(std::fstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();

};

void readUserFile(string name,int& hp, int& exp,int&  x, int& y)
{
    ifstream f;
    f.open("user/" + name);
    string data;
    std::getline(f, data);
    int count = 0;
    int l = 0;
    for (int r = 0; r < data.length(); ++r) {
        if (data[r] == ' ') {
            switch (count) {
                case 0: {
                    hp = atoi(data.substr(l, r).c_str());
                    count += 1;
                    l = r + 1;
                    break;
                }
                case 1: {
                    exp = atoi(data.substr(l, r).c_str());
                    count += 1;
                    l = r + 1;
                    break;
                }
                case 2: {
                    x = atoi(data.substr(l, r).c_str());
                    count += 1;
                    l = r + 1;
                    break;
                }
                case 3: {
                    y = atoi(data.substr(l, r).c_str());
                    count += 1;
                    l = r + 1;
                    break;
                }
            }
        }
    }
}

void writeUserFile(string name,int& hp, int& exp,int&  x, int& y)
{
    ofstream f;
    f.open("user/" + name, ios::trunc);
    string inject = to_string(hp) + " " + to_string(exp) + " " + to_string(x) + " " + to_string(y) + " ";
    f << inject;
    f.flush();
    f.close();
}

unsigned char* userInform(int fd)
{
    move_notify* move_r = new move_notify();
    int hp = -1, exp = -1, x = -1, y = -1;
    string name = user_map_r.find(fd)->second;
    readUserFile(name,hp,exp,x,y);
    int len = move_r->length;
    strcpy(reinterpret_cast<char*>(move_r->name),name.c_str());
    move_r->hp = hp;
    move_r->exp = exp;
    move_r->x = x;
    move_r->y = y;
    unsigned char* buff = new unsigned char[len];
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
    delete move_r;
    return buff;
}
#endif //SERVER_CPP_FUNCITONALUNITY_H
