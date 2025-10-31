#ifndef GAME_HEADER_H
#define GAME_HEADER_H

struct Coord {
    int x;
    int y;
    bool operator==(const Coord& t) const;
};


struct CoordHash {
    size_t operator()(const Coord& c) const;
};

class Item {
public:

    int weight;

    Item(int w);

    virtual char getType() = 0;
    virtual std::pair<Coord, char> getKey();
    virtual ~Item() {}
};

class Room {
public:
    
    Room* north;
    Room* south;
    Room* east;
    Room* west;
    std::string locked;
    std::vector<Item*> chest;
    std::string description;

    static std::unordered_map<Coord, Room*, CoordHash> coordinates;

    Room(Room* n, Room* s, Room* e, Room* w, std::string d, Coord c);
    ~Room();
};

class Key : public Item {
public:

    Coord coordinate;
    char direction;

    Key(int w, Coord c, char d);
    char getType() override;
    std::pair<Coord, char> getKey() override;

};

class Player {
public:

    int curWeight = 0;
    int maxWeight = 100;
    std::vector<Item*> inventory;
    std::vector<Room*> map;

    ~Player();
};

bool contains(std::string s1, std::string s2);

void deleteRoom(Coord c, Coord pos);

void destroyCoordinates();

void destroyStronglyConnected(Coord pos);

Room* randomMap(int rooms, std::mt19937& randGen);

bool fileNameValidator(std::string s);

void saveState(std::string fileName);

void readState(std::string fileName, Room*& h, int& cx, int& cy);

std::string int_to_coord(int x, int y);

void str_to_coord(std::string coord, int& xc, int& yc);

bool isNumber(std::string s);

bool coordValidator(std::string t);

std::string initializeMap();

void drawMap(std::string fileName, Coord center = {0,0}, int roomSide = 30, int hallLength = 10, int hallWidth = 4, int wLine = 1, std::string color = "blue");

int createRoom(std::string dir, Room* curRoom, int xc, int yc);

void moveRoom(std::string dir, Room*& curRoom, int& xc, int& yc);

void openHTML(std::string fileName);

void optionsMenu();

void playerMenu();

#endif
