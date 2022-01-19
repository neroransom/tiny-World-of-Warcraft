//
// Created by nero on 29/6/2020.
//

#ifndef CLIENT_CPP_MULTIPLAYER_H
#define CLIENT_CPP_MULTIPLAYER_H

#include <cstring>
#include "messages.h"
#include "constants.h"

class Player {
public:
    std::string name = "";
    int x = 0;
    int y = 0;
    int hp = 0;
    int exp = 0;
    bool visible = 0;

    bool isVisible(int x, int y) {
        if (x <= (this->x + EE122_VISION_RANGE / 2) && x >= (this->x - EE122_VISION_RANGE / 2) &&
            y <= (this->y + EE122_VISION_RANGE / 2) && y >= (this->y - EE122_VISION_RANGE / 2)) {
            return 1;
        } else {
            return 0;
        }
    }

    bool updatePlayer(const char *name, int x, int y, int hp, int exp) {
        if (x >= EE122_SIZE_X || y >= EE122_SIZE_Y || x < 0 || y < 0) {
            on_malformed_message_from_server();
            return 1;
        }
        /* Checks the HP and EXP. */
        if (hp < 0 || exp < 0) {
            on_malformed_message_from_server();
            return 1;
        }
        this->x = x;
        this->y = y;
        this->hp = hp;
        this->exp = exp;
        on_move_notify(name, x, y, hp, exp);
    }

    bool login(const int error_code, int x, int y, int hp, int exp) {
        if (x >= EE122_SIZE_X || y >= EE122_SIZE_Y || x < 0 || y < 0) {
            on_malformed_message_from_server();
            return 1;
        }
        /* Checks the HP and EXP. */
        if (hp < 0 || exp < 0) {
            on_malformed_message_from_server();
            return 1;
        }
        if (error_code == 0) {
            this->x = x;
            this->y = y;
            this->hp = hp;
            this->exp = exp;
            on_login_reply(error_code);
            return 0;
        } else if (error_code > 1) {
            on_malformed_message_from_server();
        } else {
            on_login_reply(error_code);
        }
        return 0;
    }


} PLAYER;

class players {
public:
    int used = 0;
    Player list[9];

    bool ifExist(std::string name) {
        for (int i = 0; i < 9; ++i) {
            if (list[i].name == name) {
                return 1;
            };
        }
        return 0;
    }

    bool updatePlayerList(std::string name, int x, int y, int hp, int exp) {
        if (x >= EE122_SIZE_X || y >= EE122_SIZE_Y || x < 0 || y < 0) {
            on_malformed_message_from_server();
            return 1;
        }
        /* Checks the HP and EXP. */
        if (hp < 0 || exp < 0) {
            on_malformed_message_from_server();
            return 1;
        }
        for (int i = 0; i < used; ++i) {
            if (list[i].name == name) {
                list[i].x = x;
                list[i].y = y;
                list[i].hp = hp;
                list[i].exp = exp;
                if (list[i].visible = PLAYER.isVisible(x, y)) {
                    on_move_notify(list[i].name.c_str(), list[i].x, list[i].y, list[i].hp, list[i].exp);
                }
                return 1;
            }
        }
        while (used > 8) {
            srand(time(NULL));
            used = rand() % 10;
        }       //if user are tpo much
        list[used].name = name;
        list[used].x = x;
        list[used].y = y;
        list[used].hp = hp;
        list[used].exp = exp;
        if (list[used].visible = PLAYER.isVisible(x, y)) {
            on_move_notify(list[used].name.c_str(), list[used].x, list[used].y, list[used].hp, list[used].exp);
        }
        ++used;
        return 1;
    }


    bool deletePlayer(std::string name) {
        if (!check_player_name(name)) {
            on_malformed_message_from_server();
            return 1;
        }
        bool ifFound = 0;
        for (int i = 0; i < (used + 1 > 9 ? 9 : used + 1); ++i) {
            if (!ifFound) {
                if (list[i].name == name) {
                    ifFound = 1;
                    list[i].name = "";
                    list[i].x = 0;
                    list[i].y = 0;
                    list[i].hp = 0;
                    list[i].exp = 0;
                    list[i].visible = 0;
                };
            } else {
                list[i - 1].name = list[i].name;
                list[i - 1].x = list[i].x;
                list[i - 1].y = list[i].y;
                list[i - 1].hp = list[i].hp;
                list[i - 1].exp = list[i].exp;
                list[i - 1].visible = list[i].visible;
            }
        }
        on_exit_notify(name.c_str()); //TODO
    };

    bool isVisible(std::string name) {
        for (int i = 0; i < 9; ++i) {
            if (list[i].name == name) {
                return list[i].visible;
            }
        }
        return 0;
    }
} PLAYER_LIST;

void catchInSight() {
    for (int i = 0; i < PLAYER_LIST.used; ++i) {
        if (PLAYER.isVisible(PLAYER_LIST.list[i].x, PLAYER_LIST.list[i].y) && PLAYER_LIST.list[i].visible == 0) {
            on_move_notify(PLAYER_LIST.list[i].name.c_str(), PLAYER_LIST.list[i].x, PLAYER_LIST.list[i].y,
                           PLAYER_LIST.list[i].hp, PLAYER_LIST.list[i].exp);
        }
        PLAYER_LIST.list[i].visible = PLAYER.isVisible(PLAYER_LIST.list[i].x, PLAYER_LIST.list[i].y);
    }
};

bool attackWitness(const char *attacker_name, const char *victim_name, const int damage, const int updated_hp) {

    if (!check_player_name(attacker_name) ||
        !check_player_name(victim_name)) {
        on_malformed_message_from_server();
        return 1;
    }
    /* Checks the HP and EXP. */
    if (updated_hp < 0 || damage < 0) {
        on_malformed_message_from_server();
        return 1;
    }
    if (PLAYER.name == victim_name) {
        PLAYER.hp = updated_hp;
        on_attack_notify(reinterpret_cast<const char *>(attacker_name), reinterpret_cast<const char *>(victim_name),
                         damage, updated_hp);
    } else {
        bool aSeen = 0, bSeen = 0;
        if (PLAYER.name == attacker_name) {
            aSeen = 1;
        }
        for (int i = 0; i < 9; ++i) {
            if (PLAYER_LIST.list[i].name == attacker_name) {
                aSeen = 1;
            };
            if (PLAYER_LIST.list[i].name == victim_name) {
                PLAYER_LIST.list[i].hp = updated_hp;
                bSeen = 1;
            }
        }
        if (aSeen && bSeen) {
            on_attack_notify(reinterpret_cast<const char *>(attacker_name), reinterpret_cast<const char *>(victim_name),
                             damage, updated_hp);
        }
    }
};
#endif //CLIENT_CPP_MULTIPLAYER_H
