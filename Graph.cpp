

#include "Graph.h"
#include <iostream>

using namespace std;

// Custom queue node for BFS
struct QueueNode
{
    CityNode* city;
    QueueNode* next;
};

// Custom linked-list queue
class CityQueue
{
private:
    QueueNode* front;
    QueueNode* rear;

public:
    CityQueue()
    {
        front = NULL;
        rear = NULL;
    }

    ~CityQueue()
    {
        while (!isEmpty())
            dequeue();
    }

    bool isEmpty()
    {
        return front == NULL;
    }

    void enqueue(CityNode* city)
    {
        QueueNode* newNode = new QueueNode;

        newNode->city = city;
        newNode->next = NULL;

        if (rear == NULL)
        {
            front = rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
    }

    CityNode* dequeue()
    {
        if (isEmpty())
            return NULL;

        QueueNode* temp = front;
        CityNode* city = temp->city;

        front = front->next;

        if (front == NULL)
            rear = NULL;

        delete temp;

        return city;
    }
};

// Constructor
Graph::Graph()
{
    head = NULL;
}

// Destructor
Graph::~Graph()
{
    CityNode* current = head;

    while (current != NULL)
    {
        RoadNode* road = current->roads;

        while (road != NULL)
        {
            RoadNode* tempRoad = road;
            road = road->next;
            delete tempRoad;
        }

        CityNode* tempCity = current;
        current = current->next;

        delete tempCity;
    }
}

// Add a city
bool Graph::addCity(string name, float x, float y)
{
    if (findCity(name) != NULL)
        return false;

    CityNode* newCity = new CityNode;

    newCity->name = name;
    newCity->x = x;
    newCity->y = y;
    newCity->roads = NULL;
    newCity->next = NULL;

    if (head == NULL)
    {
        head = newCity;
    }
    else
    {
        CityNode* current = head;

        while (current->next != NULL)
            current = current->next;

        current->next = newCity;
    }

    return true;
}

// Search for a city
CityNode* Graph::findCity(string name)
{
    CityNode* current = head;

    while (current != NULL)
    {
        if (current->name == name)
            return current;

        current = current->next;
    }

    return NULL;
}

// Add an undirected road
bool Graph::addRoad(string city1, string city2)
{
    CityNode* first = findCity(city1);
    CityNode* second = findCity(city2);

    if (first == NULL || second == NULL)
        return false;

    if (first == second)
        return false;

    // Check for an existing connection
    RoadNode* check = first->roads;

    while (check != NULL)
    {
        if (check->destination == second)
            return false;

        check = check->next;
    }

    // First direction
    RoadNode* road1 = new RoadNode;

    road1->destination = second;
    road1->next = first->roads;
    first->roads = road1;

    // Reverse direction
    RoadNode* road2 = new RoadNode;

    road2->destination = first;
    road2->next = second->roads;
    second->roads = road2;

    return true;
}

// Remove a connection from one adjacency list
void Graph::removeConnection(
    CityNode* city,
    CityNode* destination)
{
    RoadNode* current = city->roads;
    RoadNode* previous = NULL;

    while (current != NULL)
    {
        if (current->destination == destination)
        {
            if (previous == NULL)
                city->roads = current->next;
            else
                previous->next = current->next;

            delete current;
            return;
        }

        previous = current;
        current = current->next;
    }
}

// Remove an undirected road
bool Graph::removeRoad(string city1, string city2)
{
    CityNode* first = findCity(city1);
    CityNode* second = findCity(city2);

    if (first == NULL || second == NULL)
        return false;

    RoadNode* check = first->roads;

    while (check != NULL)
    {
        if (check->destination == second)
        {
            removeConnection(first, second);
            removeConnection(second, first);
            return true;
        }

        check = check->next;
    }

    return false;
}

// Display the adjacency list
void Graph::display()
{
    CityNode* current = head;

    cout << "\nGHANA CITY ROAD NETWORK\n";

    while (current != NULL)
    {
        cout << current->name << " -> ";

        RoadNode* road = current->roads;

        while (road != NULL)
        {
            cout << road->destination->name;

            if (road->next != NULL)
                cout << " -> ";

            road = road->next;
        }

        cout << endl;
        current = current->next;
    }
}

// Breadth First Search
vector<string> Graph::BFS(string start)
{
    vector<string> result;
    vector<CityNode*> visited;

    CityNode* startCity = findCity(start);

    if (startCity == NULL)
        return result;

    CityQueue queue;

    visited.push_back(startCity);
    queue.enqueue(startCity);

    while (!queue.isEmpty())
    {
        CityNode* current = queue.dequeue();

        result.push_back(current->name);

        RoadNode* road = current->roads;

        while (road != NULL)
        {
            CityNode* neighbour = road->destination;

            bool alreadyVisited = false;

            for (size_t i = 0; i < visited.size(); i++)
            {
                if (visited[i] == neighbour)
                {
                    alreadyVisited = true;
                    break;
                }
            }

            if (!alreadyVisited)
            {
                visited.push_back(neighbour);
                queue.enqueue(neighbour);
            }

            road = road->next;
        }
    }

    return result;
}

// Recursive Depth First Search
void Graph::depthFirstSearch(
    CityNode* city,
    vector<CityNode*>& visited,
    vector<string>& result)
{
    visited.push_back(city);
    result.push_back(city->name);

    RoadNode* road = city->roads;

    while (road != NULL)
    {
        CityNode* neighbour = road->destination;

        bool alreadyVisited = false;

        for (size_t i = 0; i < visited.size(); i++)
        {
            if (visited[i] == neighbour)
            {
                alreadyVisited = true;
                break;
            }
        }

        if (!alreadyVisited)
        {
            depthFirstSearch(neighbour, visited, result);
        }

        road = road->next;
    }
}

// DFS starting from a given city
vector<string> Graph::DFS(string start)
{
    vector<string> result;
    vector<CityNode*> visited;

    CityNode* startCity = findCity(start);

    if (startCity != NULL)
    {
        depthFirstSearch(startCity, visited, result);
    }

    return result;
}

// Check connectivity between two cities
bool Graph::areConnected(
    string start,
    string destination)
{
    vector<string> traversal = BFS(start);

    for (size_t i = 0; i < traversal.size(); i++)
    {
        if (traversal[i] == destination)
            return true;
    }

    return false;
}

// Give access to the city list for SFML rendering
CityNode* Graph::getHead()
{
    return head;
}
