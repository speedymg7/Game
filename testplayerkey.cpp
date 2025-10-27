#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <filesystem>
#include <fstream>
#include <vector>
#include <utility>
#include "header.h"
using namespace std;

int main() {
    string mapName = initializeMap();
    Room* cur;
    int curX;
    int curY;
    string in;
    readState("default.bin", cur, curX, curY);
    Player* player = new Player();

    while(in != "q") {
        cout << ">> ";
        getline(cin, in);
        // removes all whitespace
        in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
        if(contains(in, "mv")) {
            if(in.length() == 2) {
                cout << endl << "n - north" << endl << "s - south" << endl << "e - east" << endl << "w - west" << endl << endl << ">> ";
                string t;
                getline(cin, t);
                // removes all whitespace
                t.erase(remove_if(t.begin(), t.end(), ::isspace), t.end());
                while(t != "n" && t != "s" && t != "e" && t != "w" && t != "q" && t != "o") {
                    cout << endl << "Invalid input" << endl << endl;
                    cout << endl << "n - north" << endl << "s - south" << endl << "e - east" << endl << "w - west" << endl << endl << ">> ";
                    getline(cin, t);
                    // removes all whitespace
                    t.erase(remove_if(t.begin(), t.end(), ::isspace), t.end());
                }
                if(t == "q") {
                    break;
                }
                if(t == "o") {
                    cout << endl << "Not moved" << endl;
                    playerMenu();
                    continue;
                }
                in += t;

            } else if(in.length() > 3) {
                cout << endl << "Invalid Input" << endl;
                continue;
            }
            if(in == "mvn") {
                if(cur->north == nullptr) {
                    cout << endl << "Room does not exist in north direction; not moved" << endl;
                } else if(contains(cur->north->locked, "s")) {
                    if(player->inventory.size() == 0) {
                        cout << endl << "Door is locked and key is not in inventory; not moved" << endl;
                    }
                    for(int i = 0; i < player->inventory.size(); i++) {
                        if(player->inventory[i]->getType() == 'K') {
                            pair tpair = player->inventory[i]->getKey();
                            if(tpair.first.x == curX && tpair.first.y == curY+1 && tpair.second == 's') {
                                cur = cur->north;
                                curY++;
                                cout << endl << "Door unlocked with key in inventory" << endl;
                                cout << endl << "Current position: (" << curX << "," << curY << ")" << endl;
                                break;
                            }
                        }
                        if(i == player->inventory.size() - 1) {
                            cout << endl << "Door is locked and key is not in inventory; not moved" << endl;
                        }
                    }
                } else {
                    cur = cur->north;
                    curY++;
                    cout << endl << "Current position: (" << curX << "," << curY << ")" << endl;
                }
            } else if(in == "mvs") {
                if(cur->south == nullptr) {
                    cout << endl << "Room does not exist in south direction; not moved" << endl;
                } else if(contains(cur->south->locked, "n")) {
                    if(player->inventory.size() == 0) {
                        cout << endl << "Door is locked and key is not in inventory; not moved" << endl;
                    }
                    for(int i = 0; i < player->inventory.size(); i++) {
                        if(player->inventory[i]->getType() == 'K') {
                            pair tpair = player->inventory[i]->getKey();
                            if(tpair.first.x == curX && tpair.first.y == curY-1 && tpair.second == 'n') {
                                cur = cur->south;
                                curY--;
                                cout << endl << "Door unlocked with key in inventory" << endl;
                                cout << endl << "Current position: (" << curX << "," << curY << ")" << endl;
                                break;
                            }
                        }
                        if(i == player->inventory.size() - 1) {
                            cout << endl << "Door is locked and key is not in inventory; not moved" << endl;
                        }
                    }
                } else {
                    cur = cur->south;
                    curY--;
                    cout << endl << "Current position: (" << curX << "," << curY << ")" << endl;
                }
            } else if(in == "mve") {
                if(cur->east == nullptr) {
                    cout << endl << "Room does not exist in east direction; not moved" << endl;
                } else if(contains(cur->east->locked, "w")) {
                    if(player->inventory.size() == 0) {
                        cout << endl << "Door is locked and key is not in inventory; not moved" << endl;
                    }
                    for(int i = 0; i < player->inventory.size(); i++) {
                        if(player->inventory[i]->getType() == 'K') {
                            pair tpair = player->inventory[i]->getKey();
                            if(tpair.first.x == curX+1 && tpair.first.y == curY && tpair.second == 'w') {
                                cur = cur->east;
                                curX++;
                                cout << endl << "Door unlocked with key in inventory" << endl;
                                cout << endl << "Current position: (" << curX << "," << curY << ")" << endl;
                                break;
                            }
                        }
                        if(i == player->inventory.size() - 1) {
                            cout << endl << "Door is locked and key is not in inventory; not moved" << endl;
                        }
                    }
                } else {
                    cur = cur->east;
                    curX++;
                    cout << endl << "Current position: (" << curX << "," << curY << ")" << endl;
                }
            } else if(in == "mvw") {
                if(cur->west == nullptr) {
                    cout << endl << "Room does not exist in west direction; not moved" << endl;
                } else if(contains(cur->west->locked, "e")) {
                    if(player->inventory.size() == 0) {
                        cout << endl << "Door is locked and key is not in inventory; not moved" << endl;
                    }
                    for(int i = 0; i < player->inventory.size(); i++) {
                        if(player->inventory[i]->getType() == 'K') {
                            pair tpair = player->inventory[i]->getKey();
                            if(tpair.first.x == curX-1 && tpair.first.y == curY && tpair.second == 'e') {
                                cur = cur->west;
                                curX--;
                                cout << endl << "Door unlocked with key in inventory" << endl;
                                cout << endl << "Current position: (" << curX << "," << curY << ")" << endl;
                                break;
                            }
                        }
                        if(i == player->inventory.size() - 1) {
                            cout << endl << "Door is locked and key is not in inventory; not moved" << endl;
                        }
                    }
                } else {
                    cur = cur->west;
                    curX--;
                    cout << endl << "Current position: (" << curX << "," << curY << ")" << endl;
                }
            } else {
                cout << endl << "Invalid input; not moved" << endl;
            }
        } else if(in == "pm") {
            drawMap(mapName, {curX,curY});
            openHTML(mapName);
        } else if(in == "pc") {
            cout << endl << curX << "," << curY << endl;
        } else if(in == "s") {
            if(cur->chest.size() != 0) {
                for(size_t i = 0; i < cur->chest.size(); i++) {
                    player->inventory.push_back(cur->chest[i]);
                }
                cout << endl << cur->chest.size() << " key(s) added to inventory" << endl;
                cur->chest.clear();

            } else {
                cout << endl << "The room is empty" << endl;
            }
        } else if(in =="pi") {
            cout << endl << "You have " << player->inventory.size() << " key(s)" << endl;
        } else if(in == "o") {
            playerMenu();
        }
        cout << endl;
    }
    destroyCoordinates();
    delete player;
    return 0;
}
