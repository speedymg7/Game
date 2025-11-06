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

    virtual uint64_t getByteOffset() {}

    virtual void setByteOffset(uint64_t b) {}

    virtual bool* getDisplayDir() {}

    virtual void setDisplayDir(bool a[2]) {}

    virtual void setNDisplayDir(bool a) {}

    virtual void setEDisplayDir(bool a) {}

    static std::unordered_map<Coord, Room*, CoordHash> coordinates;

    Room(Coord c={0,0}, Room* n=nullptr, Room* s=nullptr, Room* e=nullptr, Room* w=nullptr, std::string d="");
    virtual ~Room();
};

class PlayerRoom : public Room {
public:

    uint64_t byteOffset = 0;
    bool displayDir[2] = {false, false};

    uint64_t getByteOffset() override;
    void setByteOffset(uint64_t b) override;
    bool* getDisplayDir() override;
    void setDisplayDir(bool a[2]) override;
    void setNDisplayDir(bool a) override;
    void setEDisplayDir(bool a) override;

    PlayerRoom(Coord c={0,0}, Room* n=nullptr, Room* s=nullptr, Room* e=nullptr, Room* w=nullptr, std::string d="") : Room(c, n, s, e, w, d) {}
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

void readPlayerState(std::string fileName, Room*& h, int& cx, int& cy);

std::string int_to_coord(int x, int y);

void str_to_coord(std::string coord, int& xc, int& yc);

bool isNumber(std::string s);

bool coordValidator(std::string t);

std::string initializeMap(Player* p=nullptr, Coord c = {0,0}, Room* cur=nullptr, int roomSide = 60, int hallLength = 20, int hallWidth = 8, int wLine = 2, std::string color = "blue");

void drawMap(std::string fileName, Coord center = {0,0});

void updatePlayerHalls(std::string fileName, char dir, Room* cur);

void updatePlayerMap(std::string fileName, Coord c, char dir, Room* cur);

int createRoom(std::string dir, Room* curRoom, int xc, int yc);

void moveRoom(std::string dir, Room*& curRoom, int& xc, int& yc);

void openHTML(std::string fileName);

void optionsMenu();

void playerMenu();

#endif
