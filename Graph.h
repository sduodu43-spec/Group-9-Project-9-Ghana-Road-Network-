

#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

using namespace std;

// Represents a road connection
struct RoadNode
{
    struct CityNode* destination;
    RoadNode* next;
};

// Represents a city in the network
struct CityNode
{
    string name;
    float x;
    float y;

    RoadNode* roads;
    CityNode* next;
};

class Graph
{
private:
    CityNode* head;

    void removeConnection(CityNode* city,
                          CityNode* destination);

    void depthFirstSearch(
        CityNode* city,
        vector<CityNode*>& visited,
        vector<string>& result
    );

public:
    Graph();
    ~Graph();

    bool addCity(string name, float x, float y);

    bool addRoad(string city1, string city2);

    bool removeRoad(string city1, string city2);

    CityNode* findCity(string name);

    void display();

    bool areConnected(string start, string destination);

    vector<string> BFS(string start);

    vector<string> DFS(string start);

    CityNode* getHead();
};

#endif
