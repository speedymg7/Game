#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <filesystem>
#include <fstream>
#include "header.h"
using namespace std;


int main() {
    string mapName = initializeMap();
    Room* head = new Room(nullptr, nullptr, nullptr, nullptr, "", {0,0});
    Room* cur = head;
    int curX = 0;
    int curY = 0;
    random_device rd;
    mt19937 randGen(rd());
    string in;
    optionsMenu();
    while(in != "q") {
        
        cout << ">> ";
        getline(cin, in);
        
        // removes all whitespace
        in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
    
        if(in.length() == 0) {
            cout << endl << "No input" << endl << endl;
            continue;
        }
        if(in == "c") {
            cout << endl << "n - north" << endl << "s - south" << endl << "e - east" << endl << "w - west" << endl << endl << ">> ";
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            if(in == "q") {
                break;
            }
            createRoom(in, cur, curX, curY);
        } else if(in.substr(0,1) == "c") {
            if(in.substr(1,1) == "m") {
                if(in.length() == 2) {
                    cout << endl << "n - north" << endl << "s - south" << endl << "e - east" << endl << "w - west" << endl << endl << ">> ";
                    getline(cin, in);
                    // removes all whitespace
                    in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                    if(in == "q") {
                        break;
                    }
                    int tempRet = createRoom(in, cur, curX, curY);
                    if(tempRet == -1) {
                        cout << endl << "Not moved, current position: " << "(" << curX << "," << curY << ")" << endl << endl;
                    } else if(tempRet == 1) {
                        moveRoom(in, cur, curX, curY);
                        cout << endl << "Current Position: " << "(" << curX << "," << curY << ")" << endl << endl;
                    }
                } else if(in.length() == 3) {
                    int tempRet = createRoom(in.substr(2,1), cur, curX, curY);
                    if(tempRet == -1) {
                        cout << endl << "Not moved, current position: " << "(" << curX << "," << curY << ")" << endl << endl;
                    } else if(tempRet == 1) {
                        moveRoom(in.substr(2,1), cur, curX, curY);
                        cout << endl << "Current Position: " << "(" << curX << "," << curY << ")" << endl << endl;
                    }
                }
            } else if(in.length() > 2) {
                cout << endl << "Invalid input" << endl << endl;
                continue;
            }
            else if(in[1] == 'n' || in[1] == 's' || in[1] == 'e' || in[1] == 'w') {
                createRoom(in.substr(1,1), cur, curX, curY);
            } else {
                cout << endl << "Invalid input" << endl << endl;
                continue;
            }
        } else if(contains(in, "mv")) {
            if(in.length() == 3) {
                moveRoom(in.substr(2,1), cur, curX, curY);
                cout << endl << "Current Position: " << "(" << curX << "," << curY << ")" << endl << endl;
            } else if(in.length() == 2) {
                cout << endl << "n - north" << endl << "s - south" << endl << "e - east" << endl << "w - west" << endl << endl << ">> ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                if(in == "q") {
                    break;
                }
                moveRoom(in, cur, curX, curY);
                cout << endl << "Current Position: " << "(" << curX << "," << curY << ")" << endl << endl;
            } else {
                cout << endl << "Invalid input" << endl << endl;
                continue;
            }
        } else if(in == "d") {
            if(in.length() == 1) {
                cout << endl << "Enter description for current room: ";
                getline(cin, in);
                if(in == "") {
                    cout << endl << "No description given" << endl << endl;
                }
                else if(cur->description != "") {
                    cout << endl << "Description already exists, do you want to overwrite? (y/n): ";
                    string tempIn;
                    getline(cin, tempIn);
                    if(tempIn == "y") {
                        cur->description = in;
                    } else if(tempIn != "n") {
                        cout << endl << "Invalid input" << endl << endl;
                        continue;
                    }
                        
                } else {
                    cur->description = in;
                }
                cout << endl;
                
            } else {
                cout << endl << "Invalid input" << endl << endl;
                continue;
            }
        } else if(in == "pd") {
            if(cur->description == "") {
                cout << endl << "Room does not have a description" << endl << endl;
                
            } else {
                cout << endl << cur->description << endl << endl;
            }
        } else if(in == "o") {
            optionsMenu();
        } else if(in == "pc") {
            cout << endl << "Current Position: " << "(" << curX << "," << curY << ")" << endl << endl;
        } else if(in.substr(0,1) == "j") {
            if(in.length() == 1) {
                cout << endl << "Enter coordinate: ";
                string tempStr;
                getline(cin, tempStr);
                in += tempStr;
            }
            in = in.substr(1, in.length() - 1);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            
            in.erase(remove(in.begin(), in.end(), '('), in.end());
            in.erase(remove(in.begin(), in.end(), ')'), in.end());
            
            if(!coordValidator(in)) {
                cout << endl << "Invalid input" << endl << endl;
                continue;
            }
            
            try {
                int tempX;
                int tempY;
                str_to_coord(in, tempX, tempY);
                cur = Room::coordinates.at({tempX, tempY});
                curX = tempX;
                curY = tempY;
                
            } catch (const out_of_range& e) {
                cout << endl << "Not moved; room does not exist" << endl;
            }
            cout << endl << "Current Position: " << "(" << curX << "," << curY << ")" << endl << endl;
        } else if(in == "pm") {
            drawMap(mapName, {curX,curY});
            openHTML(mapName);
        } else if(in == "s") {
            string filename;
            cout << endl << "Enter file name (without extension) to save current game state: ";
            getline(cin, filename);
            if(!fileNameValidator(filename)) {
                continue;
            }
            filename += ".bin";
            if(!filesystem::exists(filename)) {
                cout << endl << "File does not exist, would you like to make new file (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                while(in != "y" && in != "n") {
                    cout << endl << "Invalid input" << endl;
                    cout << endl << "File does not exist, would you like to make new file (y/n)? ";
                    getline(cin, in);
                    // removes all whitespace
                    in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                }
                if(in == "n") {
                    cout << endl << "File not saved" << endl << endl;
                    continue;
                }
                
            } else {
                cout << endl << "A file with that name already exists, would you like to overwrite (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                while(in != "y" && in != "n") {
                    cout << endl << "Invalid input" << endl;
                    cout << endl << "File does not exist, would you like to make new file (y/n)? ";
                    getline(cin, in);
                    // removes all whitespace
                    in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                }
                if(in == "n") {
                    cout << endl << "File not saved" << endl << endl;
                    continue;
                }
            }
            saveState(filename);
        } else if(in == "l") {

            cout << endl << "Loading a new state will erase any unsaved work. Do you want to continue (y/n)? ";
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            while(in != "y" && in != "n") {
                cout << endl << "Invalid input" << endl;
                cout << endl << "Loading a new state will erase any unsaved work. Do you want to continue (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            }
            if(in == "n") {
                cout << endl << "File load aborted" << endl << endl;
                continue;
            }
            string filename;
            cout << endl << "Enter name of file to load (without extension): ";
            getline(cin, filename);
            if(!fileNameValidator(filename)) {
                continue;
            }
            filename += ".bin";
            if(!filesystem::exists(filename)) {
                cout << endl << "File does not exist" << endl;
                cout << endl << "File load aborted" << endl << endl;
                continue;
            }
            destroyCoordinates();
            readState(filename, cur, curX, curY);
        } else if(in == "r") {

            cout << endl << "Any unsaved work will be lost. Do you want to continue (y/n)? ";
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            while(in != "y" && in != "n") {
                cout << endl << "Invalid input" << endl;
                cout << endl << "Any unsaved work will be lost. Do you want to continue (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            }
            if(in == "n") {
                cout << endl << "Exited random map generation" << endl << endl;
                continue;
            }

            cout << endl << "Enter the number of rooms: ";
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            while(!isNumber(in) || stoi(in) == 0) {
                if(in == "q") {
                    break;
                }
                cout << endl << "Invalid input" << endl;
                cout << endl << "Enter the number of rooms: ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            }
            if(in != "q") {
                head = randomMap(stoi(in), randGen);
                cur = head;
                curX = 0;
                curY = 0;
                cout << endl;
            }
        } else if(in == "e") {
            cout << endl << "Enter coordinates of room to delete (cannot be current room): ";
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());

            in.erase(remove(in.begin(), in.end(), '('), in.end());
            in.erase(remove(in.begin(), in.end(), ')'), in.end());
            
            while(!coordValidator(in)) {
                cout << endl << "Invalid input" << endl << endl;
                cout << endl << "Enter coordinates of room to delete (cannot be current room): ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());

                in.erase(remove(in.begin(), in.end(), '('), in.end());
                in.erase(remove(in.begin(), in.end(), ')'), in.end());
            }
            int tempX;
            int tempY;
            str_to_coord(in, tempX, tempY);
            deleteRoom({tempX, tempY}, {curX, curY});
        } else if(in == "lk") {
            cout << endl << "This will only lock doors for players moving into current room but does not lock the doors going out of current room!" << endl << endl;
            cout << endl << "Which door do you want to lock in room (" << curX << "," << curY << ") (Enter n,s,e, or w): ";
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());

            while(in != "n" && in != "s" && in != "e" && in != "w" && in != "o" && in != "q") {
                cout << endl << "Invalid input" << endl << endl;
                cout << endl << "From which direction Do you want to lock the room at (" << curX << "," << curY << ") (Enter n,s,e, or w): ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            }
            if(in == "q") {
                break;
            }
            if(in == "o") {
                cout << endl << "Room not locked" << endl;
                optionsMenu();
                continue;
            }
            string dir = in;
            string direction;
            if(in == "n") {
                direction = "north";
            } else if(in == "s") {
                direction = "south";
            } else if(in == "e") {
                direction = "east";
            } else if(in == "w") {
                direction = "west";
            } else {
                cout << endl << "Error" << endl;
                optionsMenu();
                continue;
            }

            if((dir == "n" && cur->north == nullptr) || (dir == "s" && cur->south == nullptr) || (dir == "e" && cur->east == nullptr) || (dir == "w" && cur->west == nullptr)) {
                cout << endl << "Room does not exist in " << direction << " direction. Do you still want to lock the door (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                while(in != "y" && in != "n" && in != "o" && in != "q") {
                    cout << endl << "Invalid input" << endl << endl;
                    cout << endl << "Room does not exist in " << direction << " direction. Do you still want to lock the door (y/n)? ";
                    getline(cin, in);
                    // removes all whitespace
                    in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                }
                if(in == "q") {
                    break;
                }
                if(in == "o") {
                    cout << endl << "Door not locked" << endl;
                    optionsMenu();
                    continue;
                }
                if(in == "n") {
                    cout << endl << "Door not locked" << endl;
                    continue;
                }
            }
            bool alreadyLocked = false;
            if(contains(cur->locked, dir)) {
                alreadyLocked = true;
                cout << endl << "A lock and key already exist for the door in the " << direction << " direction; do you want to place a duplicate key for this lock in another room (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                while(in != "y" && in != "n" && in != "o" && in != "q") {
                    cout << endl << "Invalid input" << endl << endl;
                    cout << endl << "A lock and key already exist for the door in the " << direction << " direction; do you want to place a duplicate key for this lock in another room (y/n)? ";
                    getline(cin, in);
                    // removes all whitespace
                    in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                }
                if(in == "q") {
                    break;
                }
                if(in == "o") {
                    cout << endl << "Door not locked" << endl;
                    optionsMenu();
                    continue;
                }
                if(in == "n") {
                    cout << endl << "Door not locked" << endl;
                    continue;
                }
            }
            if(!alreadyLocked) {
                cur->locked += dir;
                cout << endl << "The " << direction << " door has been locked. Enter the coordinates of the room to store the key: ";
            } else {
                cout << endl << "Enter the coordinates of the room to store the key: ";
            }
            
            
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            
            in.erase(remove(in.begin(), in.end(), '('), in.end());
            in.erase(remove(in.begin(), in.end(), ')'), in.end());
            
            while(!coordValidator(in) && in != "o" && in != "q") {
                cout << endl << "Invalid input" << endl << endl;
                cout << endl << "Enter the coordinates of the room to store the key: ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
                
                in.erase(remove(in.begin(), in.end(), '('), in.end());
                in.erase(remove(in.begin(), in.end(), ')'), in.end());
            }
            if(in == "q") {
                break;
            }
            if(in == "o") {
                if(!alreadyLocked) {
                    cur->locked = cur->locked.substr(0, cur->locked.length() - 1);
                    cout << endl << "Key not generated, door unlocked" << endl;
                } else {
                    cout << endl << "Key not generated" << endl;
                }
                
                optionsMenu();
                continue;
            }
            try {
                int tempX;
                int tempY;
                str_to_coord(in, tempX, tempY);
                Room* tempRoom = Room::coordinates.at({tempX, tempY});
                pair<Coord, char> tempKey;
                if(tempRoom->chest.size() == 0) {
                    tempRoom->chest.push_back(new Key(0, {curX,curY}, dir[0]));
                    cout << endl << "Door locked and key stored successfully" << endl << endl;
                    continue;
                }
                for(size_t r = 0; r < tempRoom->chest.size(); r++) {
                    if(tempRoom->chest[r]->getType() == 'K') {
                        tempKey = tempRoom->chest[r]->getKey();
                        if(tempKey.first.x == curX && tempKey.first.y == curY && tempKey.second == dir[0]) {
                            if(!alreadyLocked) {
                                cur->locked = cur->locked.substr(0, cur->locked.length() - 1);
                                cout << endl << "Same key already exists in this room's chest; key not generated, door unlocked" << endl;
                            } else {
                                cout << endl << "Same key already exists in this room's chest; key not generated" << endl;
                            }
                            optionsMenu();
                            break;
                        }
                    }
                    if(r == tempRoom->chest.size() - 1) {
                        tempRoom->chest.push_back(new Key(0, {curX,curY}, dir[0]));
                        cout << endl << "Door locked and key stored successfully" << endl << endl;
                        break;
                    }
                }
            } catch (const out_of_range& e) {
                if(!alreadyLocked) {
                    cur->locked = cur->locked.substr(0, cur->locked.length() - 1);
                    cout << endl << "Room does not exist; key not generated, door unlocked" << endl;
                } else {
                    cout << endl << "Room does not exist; key not generated" << endl;
                }
                optionsMenu();
                continue;
            }
        }
    }
    destroyCoordinates();

    return 0;
}