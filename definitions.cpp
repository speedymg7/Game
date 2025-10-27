#include <iostream>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <filesystem>
#include <fstream>
#include <utility>
#include "header.h"
using namespace std;


unordered_map<Coord, Room*, CoordHash> Room::coordinates;

bool Coord::operator==(const Coord& t) const {
    return x == t.x && y == t.y;
}


size_t CoordHash::operator()(const Coord& c) const {
    return hash<int>{}(c.x) ^ (hash<int>{}(c.y) << 1);
}

Item::Item(int w) {
    weight = w;
}

pair<Coord, char> Item::getKey() {}

Room::Room(Room* n, Room* s, Room* e, Room* w, string d, Coord c) {
    north = n;
    south = s;
    east = e;
    west = w;
    description = d;
    coordinates[c] = this;
}

Room::~Room() {
    for(size_t i = 0; i < chest.size(); i++) {
        delete chest[i];
    }
}


Key::Key(int w, Coord c, char d) : Item(w) {
    coordinate = c;
    direction = d;
}

char Key::getType() {
    return 'K';
}

pair<Coord, char> Key::getKey() {
    return make_pair(coordinate, direction);
}

bool contains(string s1, string s2) {
    for(int i = 0; i < s1.length(); i++) {
        int w = i;
        for(int k = 0; k < s2.length(); k++) {
            if(w == s1.length()) {
                return false;
            }
            if(s1[w] != s2[k]) {
                break;
            }
            if(k == s2.length() - 1) {
                return true;
            }
            w++;
        }
    }
    return false;
}

void deleteRoom(Coord c, Coord pos) {
    if(Room::coordinates.empty()) {
        cout << endl << "Cannot delete room; hashmap is empty" << endl << endl;
        return;
    }
    if(c == pos) {
        cout << endl << "Cannot delete room; must move to different room" << endl << endl;
        return;
    }
    try {
        Room* tempRoom = Room::coordinates.at(c);
        if(tempRoom->north != nullptr) {
            tempRoom->north->south = nullptr;
        }
        if(tempRoom->south != nullptr) {
            tempRoom->south->north = nullptr;
        }
        if(tempRoom->east != nullptr) {
            tempRoom->east->west = nullptr;
        }
        if(tempRoom->west != nullptr) {
            tempRoom->west->east = nullptr;
        }
        delete tempRoom;
        Room::coordinates.erase(c);
        cout << endl << "Room at (" << c.x << "," << c.y << ") deleted" << endl << endl;

    } catch(const out_of_range& e) {
        cout << endl << "Cannot delete room; room does not exist" << endl << endl;
        return;
    }
}

void destroyCoordinates() {
    if(Room::coordinates.empty()) {
        cout << endl << "Cannot delete rooms; hashmap is empty" << endl << endl;
        return;
    }
    for(const auto& pair : Room::coordinates) {
        delete pair.second;
    }
    Room::coordinates.clear();
}


// prunes out rooms that are connected in all four directions; can create floating islands but no floating island of single room
void destroyStronglyConnected(Coord pos) {
    if(Room::coordinates.empty()) {
        cout << endl << "Cannot delete rooms; hashmap is empty" << endl << endl;
        return;
    }
    for(const auto& pair : Room::coordinates) {
        if(pair.first == pos) {
            continue;
        }
        if(pair.second->north != nullptr && pair.second->south != nullptr && pair.second->east != nullptr && pair.second->west != nullptr) {
            if(pair.second->north->north == nullptr && pair.second->north->east == nullptr && pair.second->north->west == nullptr) {
                continue;
            }
            if(pair.second->south->south == nullptr && pair.second->south->east == nullptr && pair.second->south->west == nullptr) {
                continue;
            }
            if(pair.second->east->north == nullptr && pair.second->east->south == nullptr && pair.second->east->east == nullptr) {
                continue;
            }
            if(pair.second->west->north == nullptr && pair.second->west->south == nullptr && pair.second->west->west == nullptr) {
                continue;
            }
            pair.second->north->south = nullptr;
            pair.second->south->north = nullptr;
            pair.second->east->west = nullptr;
            pair.second->west->east = nullptr;
            delete pair.second;
            Room::coordinates.erase(pair.first);
        }
    }
}

Room* randomMap(int rooms, mt19937& randGen) {
    destroyCoordinates();
    uniform_int_distribution<> range(0, 3);
    Room* head = new Room(nullptr, nullptr, nullptr, nullptr, "", {0,0});
    Room* cur = head;
    int count = 1;
    int curX = 0;
    int curY = 0;
    while(count < rooms) {
        if(cur->north != nullptr && cur->south != nullptr && cur->east != nullptr && cur->west != nullptr) {
            int temp = range(randGen);
            if(temp == 0) {
                cur = cur->north;
                curY++;
            } else if(temp == 1) {
                cur = cur->south;
                curY--;
            } else if(temp == 2) {
                cur = cur->east;
                curX++;
            } else {
                cur = cur->west;
                curX--;
            }
            continue;
        }
        //create+move
        Room* tempRoom;
        if(range(randGen) >= 2) {
            int temp = range(randGen);
            if(temp == 0) {
                try {
                    tempRoom = Room::coordinates.at({curX,curY+1});
                    if(range(randGen) >= 2) {
                        cur->north = tempRoom;
                        tempRoom->south = cur;
                        cur = tempRoom;
                        curY++;
                    }
                } catch(const out_of_range& e) {
                    curY++;
                    cur->north = new Room(nullptr, cur, nullptr, nullptr, "", {curX,curY});
                    cur = cur->north;
                    count++;
                }
                
            } else if(temp == 1) {
                try {
                    tempRoom = Room::coordinates.at({curX,curY-1});
                    if(range(randGen) >= 2) {
                        cur->south = tempRoom;
                        tempRoom->north = cur;
                        cur = tempRoom;
                        curY--;
                    }
                } catch(const out_of_range& e) {
                    curY--;
                    cur->south = new Room(cur, nullptr, nullptr, nullptr, "", {curX,curY});
                    cur = cur->south;
                    count++;
                }

            } else if(temp == 2) {

                try {
                    tempRoom = Room::coordinates.at({curX+1,curY});
                    if(range(randGen) >= 2) {
                        cur->east = tempRoom;
                        tempRoom->west = cur;
                        cur = tempRoom;
                        curX++;
                    }
                } catch(const out_of_range& e) {
                    curX++;
                    cur->east = new Room(nullptr, nullptr, nullptr, cur, "", {curX,curY});
                    cur = cur->east;
                    count++;
                }

            } else {
                try {
                    tempRoom = Room::coordinates.at({curX-1,curY});
                    if(range(randGen) >= 2) {
                        cur->west = tempRoom;
                        tempRoom->east = cur;
                        cur = tempRoom;
                        curX--;
                    }
                } catch(const out_of_range& e) {
                    curX--;
                    cur->west = new Room(nullptr, nullptr, cur, nullptr, "", {curX,curY});
                    cur = cur->west;
                    count++;
                } 
            }
        // no move
        } else {
            int temp = range(randGen);
            if(temp == 0) {
                try {
                    tempRoom = Room::coordinates.at({curX,curY+1});
                    if(range(randGen) >= 2) {
                        cur->north = tempRoom;
                        tempRoom->south = cur;
                    }
                } catch(const out_of_range& e) {
                    cur->north = new Room(nullptr, cur, nullptr, nullptr, "", {curX,curY+1});
                    count++;
                }
            } else if(temp == 1) {
                try {
                    tempRoom = Room::coordinates.at({curX,curY-1});
                    if(range(randGen) >= 2) {
                        cur->south = tempRoom;
                        tempRoom->north = cur;
                    }
                } catch(const out_of_range& e) {
                    cur->south = new Room(cur, nullptr, nullptr, nullptr, "", {curX,curY-1});
                    count++;
                }
            } else if(temp == 2) {
                try {
                    tempRoom = Room::coordinates.at({curX+1,curY});
                    if(range(randGen) >= 2) {
                        cur->east = tempRoom;
                        tempRoom->west = cur;
                    }
                } catch(const out_of_range& e) {
                    cur->east = new Room(nullptr, nullptr, nullptr, cur, "", {curX+1,curY});
                    count++;
                }

            } else {
                try {
                    tempRoom = Room::coordinates.at({curX-1,curY});
                    if(range(randGen) >= 2) {
                        cur->west = tempRoom;
                        tempRoom->east = cur;
                    }
                } catch(const out_of_range& e) {
                    cur->west = new Room(nullptr, nullptr, cur, nullptr, "", {curX-1,curY});
                    count++;
                }   
            }
        }
    }
    return head;
}


bool fileNameValidator(string s) {
    if(s.length() == 0) {
        cout << endl << "Filename not given; file not created/opened" << endl << endl;
        return false;
    }
    if(s.length() > 255) {
        cout << endl << "Filename too long; file not created/opened" << endl << endl;
        return false;
    }
    if(isspace(s[0]) || isspace(s[s.length() - 1])) {
        cout << endl << "Filename cannot begin or end with space; file not created/opened" << endl << endl;
        return false;
    }
    string avoid = "<>:\"/\\|?*.";
    for(int i = 0; i < s.length(); i++) {
        if(s[i] < 32) {
            cout << endl << "Filename can only consist of letters and numbers; file not created/opened" << endl << endl;
            false;
        }
        for(int k = 0; k < avoid.length(); k++) {
            if(s[i] == avoid[k]) {
                cout << endl << "Filename can only consist of letters and numbers; file not created/opened" << endl << endl;
                return false;
            }
        }
    }
    return true;
}


void saveState(string fileName) {
    ofstream ofile(fileName, ios::binary);
    if(!ofile) {
        cout << endl << "Error opening file" << endl << endl;
        ofile.close();
        return;
    }

    for(const auto& pr : Room::coordinates) {
        ofile.write(reinterpret_cast<const char*>(&pr.first.x), sizeof(pr.first.x));
        ofile.write(reinterpret_cast<const char*>(&pr.first.y), sizeof(pr.first.y));

        uint8_t t;

        if(pr.second->north != nullptr) {
            t = 1;
        } else {
            t = 0;
        }
        ofile.write(reinterpret_cast<char*>(&t), sizeof(t));

        if(pr.second->south != nullptr) {
            t = 1;
        } else {
            t = 0;
        }
        ofile.write(reinterpret_cast<char*>(&t), sizeof(t));

        if(pr.second->east != nullptr) {
            t = 1;
        } else {
            t = 0;
        }
        ofile.write(reinterpret_cast<char*>(&t), sizeof(t));

        if(pr.second->west != nullptr) {
            t = 1;
        } else {
            t = 0;
        }
        ofile.write(reinterpret_cast<char*>(&t), sizeof(t));

        size_t chestSize = pr.second->chest.size();
        ofile.write(reinterpret_cast<char*>(&chestSize), sizeof(chestSize));

        Item* itemPtr;
        char type;
        

        for(size_t i = 0; i < chestSize; i++) {
            
            itemPtr = pr.second->chest[i];
            type = itemPtr->getType();
            if(type == 'K') {
                ofile.write(&type, 1);
                pair<Coord, char> tempPair = itemPtr->getKey();
                ofile.write(reinterpret_cast<const char*>(&tempPair.first.x), sizeof(tempPair.first.x));
                ofile.write(reinterpret_cast<const char*>(&tempPair.first.y), sizeof(tempPair.first.y));
                ofile.write(&tempPair.second, 1);
            }
        }

        size_t lockedSize = pr.second->locked.length();
        ofile.write(reinterpret_cast<char*>(&lockedSize), sizeof(lockedSize));
        ofile.write(pr.second->locked.c_str(), lockedSize);

        size_t len = pr.second->description.length();
        ofile.write(reinterpret_cast<char*>(&len), sizeof(len));
        ofile.write(pr.second->description.c_str(), len);

        if(!ofile) {
            cout << endl << "Error writing to file" << endl << endl;
            ofile.close();
            return;
        }
        
    }
    ofile.close();

    cout << endl << "State saved to " << fileName << endl << endl;
}
void readState(string fileName, Room*& h, int& cx, int& cy) {

    if(!Room::coordinates.empty()) {
        cout << endl << "Hashmap still contains rooms; hashmap must be empty before readState()" << endl << endl;
        return;
    }
    ifstream ifile(fileName, ios::binary);

    if(!ifile) {
        cout << endl << "Error opening file" << endl << endl;
        ifile.close();
        return;
    }
    int c1;
    int c2;
    int k1;
    int k2;
    uint8_t n;
    uint8_t s;
    uint8_t e;
    uint8_t w;
    Item* itemPtr;
    char type;
    size_t chestSize;
    size_t lockedSize;
    size_t len;
    Room* tempRoom;

    while(ifile.read(reinterpret_cast<char*>(&c1), sizeof(c1))) {
        ifile.read(reinterpret_cast<char*>(&c2), sizeof(c2));
        ifile.read(reinterpret_cast<char*>(&n), sizeof(n));
        ifile.read(reinterpret_cast<char*>(&s), sizeof(s));
        ifile.read(reinterpret_cast<char*>(&e), sizeof(e));
        ifile.read(reinterpret_cast<char*>(&w), sizeof(w));
        ifile.read(reinterpret_cast<char*>(&chestSize), sizeof(chestSize));

        if(!ifile) {
            cout << endl << "Error reading the file" << endl << endl;
            ifile.close();
            return;
        }
        try {
            tempRoom = Room::coordinates.at({c1,c2});
        } catch(const out_of_range& e) {
            tempRoom = new Room(nullptr, nullptr, nullptr, nullptr, "", {c1,c2});
        }
        if(tempRoom->north == nullptr && n == 1) {
            try {
                tempRoom->north = Room::coordinates.at({c1,c2+1});
                tempRoom->north->south = tempRoom;
            } catch(const out_of_range& e) {
                tempRoom->north = new Room(nullptr, tempRoom, nullptr, nullptr, "", {c1,c2+1});
            }
        }
        if(tempRoom->south == nullptr && s == 1) {
            try {
                tempRoom->south = Room::coordinates.at({c1,c2-1});
                tempRoom->south->north = tempRoom;
            } catch(const out_of_range& e) {
                tempRoom->south = new Room(tempRoom, nullptr, nullptr, nullptr, "", {c1,c2-1});
            }
        }
        if(tempRoom->east == nullptr && e == 1) {
            try {
                tempRoom->east = Room::coordinates.at({c1+1,c2});
                tempRoom->east->west = tempRoom;
            } catch(const out_of_range& e) {
                tempRoom->east = new Room(nullptr, nullptr, nullptr, tempRoom, "", {c1+1,c2});
            }
        }
        if(tempRoom->west == nullptr && w == 1) {
            try {
                tempRoom->west = Room::coordinates.at({c1-1,c2});
                tempRoom->west->east = tempRoom;
            } catch(const out_of_range& e) {
                tempRoom->west = new Room(nullptr, nullptr, tempRoom, nullptr, "", {c1-1,c2});
            }
        }


        for(size_t i = 0; i < chestSize; i++) {
            ifile.read(&type, 1);

            if(type == 'K') {
                char direction;
                ifile.read(reinterpret_cast<char*>(&k1), sizeof(k1));
                ifile.read(reinterpret_cast<char*>(&k2), sizeof(k2));
                ifile.read(&direction, 1);

                tempRoom->chest.push_back(new Key(0, {k1, k2}, direction));
            }
            if(!ifile) {
                cout << endl << "Error reading the file" << endl << endl;
                ifile.close();
                return;
            }
        }
        ifile.read(reinterpret_cast<char*>(&lockedSize), sizeof(lockedSize));
        string locked(lockedSize, '\0');
        ifile.read(&locked[0], lockedSize);
        tempRoom->locked = locked;


        ifile.read(reinterpret_cast<char*>(&len), sizeof(len));
        string s(len, '\0');
        ifile.read(&s[0], len);
        tempRoom->description = s;

        if(!ifile) {
            cout << endl << "Error reading the file" << endl << endl;
            ifile.close();
            return;
        }
    }
    if(!ifile.eof()) {
        cout << endl << "File reading error occurred" << endl << endl;
        return;
    }
    ifile.close();
    try {
        h = Room::coordinates.at({0,0});
        cx = 0;
        cy = 0;
    } catch(const out_of_range& e) {
        h = tempRoom;
        cx = c1;
        cy = c2;
    }
    cout << endl << fileName << " loaded successfully" << endl;
    cout << endl << "Current Position: (" << cx << "," << cy << ")" << endl << endl;
}

string int_to_coord(int x, int y) {
    string temp = to_string(x);
    temp.append(",");
    temp.append(to_string(y));
    return temp;
}

void str_to_coord(string coord, int& xc, int& yc) {
    for(int i = 0; i < coord.length(); i++) {
        if(coord[i] == ',') {
            xc = stoi(coord.substr(0,i));
            yc = stoi(coord.substr(i+1, coord.length()-i-1));
            break;
        }
    }
}

bool isNumber(string s) {
    for(int i = 0; i < s.length(); i++) {
        if(!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

bool coordValidator(string t) {
    if(t.length() < 3) {
        return false;
    }
    int count = 0;
    for(int i = 0; i < t.length(); i++) {
        if(t[i] == ',') {
            if(count == 0) {
                if(i == 0 || i == t.length() - 1) {
                    return false;
                }
                count += 1;
                continue;
            } else {
                return false;
            }
        } else if(!(isdigit(t[i]) || t[i] == '-')) {
            return false;
        }
    }
    return true;
}


string initializeMap() {
    if(!filesystem::exists("mapTemplate.html")) {
        cout << "Error: mapTemplate.html does not exist";
        destroyCoordinates();
        exit(EXIT_FAILURE);
    }
    string fileName = "map.html";
    if(filesystem::exists(fileName)) {
        fileName = "map-1.html";
    }
    int count = 1;
    while(filesystem::exists(fileName)) {
        count++;
        fileName = fileName.substr(0, 4) + to_string(count) + ".html";
    }
    try {
        filesystem::copy_file("mapTemplate.html", fileName);
    } catch(const filesystem::filesystem_error& e) {
        cout << "Error duplicating mapTemplate.html";
        destroyCoordinates();
        exit(EXIT_FAILURE);
    }
    return fileName;
}


void drawMap(string fileName, Coord center, int roomSide, int hallLength, int hallWidth, int wLine, string color) {
    if(!filesystem::exists(fileName)) {
        cout << endl << "Error: " << fileName << " does not exist" << endl << endl;
        return;
    }
    try {
        filesystem::copy_file("mapTemplate.html", fileName, filesystem::copy_options::overwrite_existing);
    } catch(const filesystem::filesystem_error& e) {
        cout << "Error duplicating mapTemplate.html";
        return;
    }
    uintmax_t fileSize = filesystem::file_size(fileName);
    if(fileSize <= 57) {
        cout << endl << "Error: contents of map are invalid" << endl << endl;
        return;
    }
    filesystem::resize_file(fileName, fileSize - 57);

    fstream file(fileName, ios::app);
    if(!file) {
        cout << endl << "Error opening file" << endl << endl;
        file.close();
        return;
    }

    file << "    let roomSide = " << roomSide << ";" << endl;
    file << "    let hallLength = " << hallLength << ";" << endl;
    file << "    let hallWidth = " << hallWidth << ";" << endl;
    file << "    let wLine = " << wLine << ";" << endl;
    file << "    let color = \"" << color << "\";" << endl;
    file << "    let centerX = Math.floor(canvas.width / 2) - roomSide / 2;" << endl;
    file << "    let centerY = Math.floor(canvas.height / 2) - roomSide / 2;" << endl;
    file << "    let fontSize = roomSide / 6;" << endl;
    file << "    ctx.font = fontSize.toString() + \"px Arial\";" << endl;
    file << "    ctx.fillStyle = \"" << color << "\";" << endl;
    file << "    ctx.textAlign = \"center\";" << endl;
    file << "    ctx.textBaseline = \"middle\";" << endl;

    if(!file) {
        cout << endl << "Error writing to map file" << endl << endl;
        file.close();
        return;
    }

    if(Room::coordinates.find(center) == Room::coordinates.end()) {
        cout << endl << "Center coordinates do not exist in coordinates map; map not made" << endl << endl;
        file.close();
        return;
    }
    int offsetX;
    int offsetY;
    for(const auto& pair : Room::coordinates) {
        offsetX = (pair.first.x - center.x) * (2 * hallLength + roomSide);
        offsetY = (pair.first.y - center.y) * (2 * hallLength + roomSide);
        file << "    drawRoom(";
        if(pair.second->north == nullptr) {
            file << "0, ";
        } else {
            file << "1, ";
        }
        if(pair.second->south == nullptr) {
            file << "0, ";
        } else {
            file << "1, ";
        }
        if(pair.second->east == nullptr) {
            file << "0, ";
        } else {
            file << "1, ";
        }
        if(pair.second->west == nullptr) {
            file << "0, ";
        } else {
            file << "1, ";
        }
        //drawRoom(n, s, e, w, cx, cy, roomSide, hallLength, hallWidth, wLine, color)
        file << "centerX + " << offsetX <<  ", " << "centerY - " << offsetY << ", roomSide, ";
        file << "hallLength, hallWidth, wLine, color);" << endl;
        file << "    ctx.fillText(\"(" << pair.first.x << "," << pair.first.y << ")\", ";
        file << "centerX + roomSide / 2 + " << offsetX << ", centerY + roomSide / 2 - " << offsetY << ");" << endl;

        if(!file) {
            cout << endl << "Error writing to map file" << endl << endl;
            file.close();
            return;
        }
    }
    file << "    ctx.stroke();" << endl;
    file << "  }" << endl << "  draw();" << endl;
    file << "</script>" << endl << "</body>" << endl << "</html>";


    if(!file) {
        cout << endl << "Error writing to map file" << endl << endl;
    }
    file.close();
}

int createRoom(string dir, Room* curRoom, int xc, int yc) {
    if(dir == "n") {
        if(curRoom->north != nullptr) {
            cout << endl << "Room already exists at " << "(" << xc << "," << yc + 1 << ")" << endl << endl;
            return -1;
        }
        try {
            Room* tempRoom = Room::coordinates.at({xc, yc+1});
            cout << endl << "Room already exists at " << "(" << xc << "," << yc + 1 << ")" << endl;
            cout << endl << "Do you want to add a path between these rooms (y/n)? ";
            string in;
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            while(in != "y" && in != "n") {
                cout << endl << "Invalid input" << endl;
                cout << endl << "Do you want to add a path between these rooms (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            }
            if(in == "y") {
                curRoom->north = tempRoom;
                tempRoom->south = curRoom;
            } else {
                return -1;
            }

        } catch (const out_of_range& e) {
            curRoom->north = new Room(nullptr, curRoom, nullptr, nullptr, "", {xc, yc+1});
            cout << endl << "Room created at " << "(" << xc << "," << yc + 1 << ")" << endl << endl;
        }
    } else if(dir == "s") {
        if(curRoom->south != nullptr) {
            cout << endl << "Room already exists at " << "(" << xc << "," << yc - 1 << ")" << endl << endl;
            return -1;
        }
        try {
            Room* tempRoom = Room::coordinates.at({xc, yc-1});
            cout << endl << "Room already exists at " << "(" << xc << "," << yc - 1 << ")" << endl;
            cout << endl << "Do you want to add a path between these rooms (y/n)? ";
            string in;
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            while(in != "y" && in != "n") {
                cout << endl << "Invalid input" << endl;
                cout << endl << "Do you want to add a path between these rooms (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            }
            if(in == "y") {
                curRoom->south = tempRoom;
                tempRoom->north = curRoom;
            } else {
                return -1;
            }

        } catch (const out_of_range& e) {
            curRoom->south = new Room(curRoom, nullptr, nullptr, nullptr, "", {xc, yc-1});
            cout << endl << "Room created at " << "(" << xc << "," << yc - 1 << ")" << endl << endl;
        }
        
    } else if(dir == "e"){
        if(curRoom->east != nullptr) {
            cout << endl << "Room already exists at " << "(" << xc + 1 << "," << yc << ")" << endl << endl;
            return -1;
        }
        try {
            Room* tempRoom = Room::coordinates.at({xc+1, yc});
            cout << endl << "Room already exists at " << "(" << xc + 1 << "," << yc << ")" << endl;
            cout << endl << "Do you want to add a path between these rooms (y/n)? ";
            string in;
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            while(in != "y" && in != "n") {
                cout << endl << "Invalid input" << endl;
                cout << endl << "Do you want to add a path between these rooms (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            }
            if(in == "y") {
                curRoom->east = tempRoom;
                tempRoom->west = curRoom;
            } else {
                return -1;
            }

        } catch (const out_of_range& e) {
            curRoom->east = new Room(nullptr, nullptr, nullptr, curRoom, "", {xc+1, yc});
            cout << endl << "Room created at " << "(" << xc + 1 << "," << yc << ")" << endl << endl;
        }
    } else if(dir == "w") {
        if(curRoom->west != nullptr) {
            cout << endl << "Room already exists at " << "(" << xc - 1 << "," << yc << ")" << endl << endl;
            return -1;
        }
        try {
            Room* tempRoom = Room::coordinates.at({xc-1, yc});
            cout << endl << "Room already exists at " << "(" << xc - 1 << "," << yc << ")" << endl;
            cout << endl << "Do you want to add a path between these rooms (y/n)? ";
            string in;
            getline(cin, in);
            // removes all whitespace
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            while(in != "y" && in != "n") {
                cout << endl << "Invalid input" << endl;
                cout << endl << "Do you want to add a path between these rooms (y/n)? ";
                getline(cin, in);
                // removes all whitespace
                in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            }
            if(in == "y") {
                curRoom->west = tempRoom;
                tempRoom->east = curRoom;
            } else {
                return -1;
            }

        } catch (const out_of_range& e) {
            curRoom->west = new Room(nullptr, nullptr, curRoom, nullptr, "", {xc-1, yc});
            cout << endl << "Room created at " << "(" << xc - 1 << "," << yc << ")" << endl << endl;
        }
    } else if(dir != "q"){
        cout << endl << "Invalid input" << endl << endl;
        return -2;
    }
    return 1;
}

void moveRoom(string dir, Room*& curRoom, int& xc, int& yc) {
    if(dir == "n") {
        if(curRoom->north == nullptr) {
            cout << endl << "Not moved, room does not exist in north direction" << endl << endl;
        } else {
            curRoom = curRoom->north;
            yc++;
        }
        
    } else if(dir == "s") {
        if(curRoom->south == nullptr) {
            cout << endl << "Not moved, room does not exist in south direction" << endl << endl;
        } else {
            curRoom = curRoom->south;
            yc--;
        }
        
    } else if(dir == "e") {
        if(curRoom->east == nullptr) {
            cout << endl << "Not moved, room does not exist in east direction" << endl << endl;
        } else {
            curRoom = curRoom->east;
            xc++;
        }
    } else if(dir == "w") {
        if(curRoom->west == nullptr) {
            cout << endl << "Not moved, room does not exist in west direction" << endl << endl;
        } else {
            curRoom = curRoom->west;
            xc--;
        }
        
    } else {
        cout << endl << "Error: direction must be n, s, e, or w" << endl << endl;
    }
}

void openHTML(string fileName) {
    string s;
    #ifdef _WIN32
        s = "start " + fileName;
    #elif __APPLE__
        s = "open " + fileName;
    #elif __linux__
        s = "xdg-open " + fileName;
    #else
        cout << endl << "HTML open error" << endl;
        return;
    #endif
    system(s.c_str());
}

void optionsMenu() {
    cout << endl << "Options: " << endl;
    cout << "\tc - create room" << endl;
    cout << "\tmv - move to adjacent room" << endl;
    cout << "\tcm - create and move to new room" << endl;
    cout << "\td - add description to current room" << endl;
    cout << "\tpd - print description of current room" << endl;
    cout << "\tj - jump to coordinate" << endl;
    cout << "\tpc - print current coordinates" << endl;
    cout << "\tlk - lock current room door and generate key" << endl;
    cout << "\te - erase/delete room" << endl;
    cout << "\tpm - current map" << endl;
    cout << "\tr - generate random map" << endl;
    cout << "\ts - save current state" << endl;
    cout << "\tl - load state from file" << endl;
    cout << "\tq - quit" << endl;
    cout << "\to - options menu" << endl << endl;
}

void playerMenu() {
    cout << endl << "Options: " << endl;
    cout << "\tmv - move to adjacent room" << endl;
    cout << "\tpc - print current coordinates" << endl;
    cout << "\tpm - current map" << endl;
    cout << "\tpi - current inventory" << endl;
    cout << "\tq - quit" << endl;
    cout << "\to - options menu" << endl << endl;
}