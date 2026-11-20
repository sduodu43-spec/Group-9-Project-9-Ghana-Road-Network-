
#include <SFML/Graphics.hpp>
#include "Graph.h"

#include <iostream>
#include <string>
#include <vector>

using namespace sf;
using namespace std;

// ==========================================
// WINDOW AND COLOURS
// ==========================================

const int WIDTH = 1150;
const int HEIGHT = 750;

Color background(13, 20, 35);
Color panelColor(23, 34, 53);
Color accentColor(45, 140, 220);
Color roadColor(90, 110, 135);
Color cityColor(255, 190, 65);
Color whiteColor(240, 245, 250);
Color mutedColor(155, 170, 190);

// ==========================================
// OPERATION MODES
// ==========================================

enum ActionMode
{
    IDLE,
    ADD_ROAD,
    REMOVE_ROAD,
    CHECK_CONNECTION,
    RUN_BFS,
    RUN_DFS
};

enum InputMode
{
    NO_INPUT,
    CITY_INPUT,
    SEARCH_INPUT
};

// ==========================================
// DRAWING FUNCTIONS
// ==========================================

void drawText(RenderWindow& window, Font& font,
              string content, int size,
              float x, float y, Color color)
{
    Text text;

    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);

    window.draw(text);
}

void drawRoad(RenderWindow& window,
              Vector2f start, Vector2f end)
{
    VertexArray road(Lines, 2);

    road[0].position = start;
    road[0].color = roadColor;

    road[1].position = end;
    road[1].color = roadColor;

    window.draw(road);
}

void drawCity(RenderWindow& window, Font& font,
              CityNode* city, CityNode* highlightedCity)
{
    CircleShape marker(8);

    marker.setOrigin(8, 8);
    marker.setPosition(city->x, city->y);

    if (city == highlightedCity)
        marker.setFillColor(Color(70, 220, 150));
    else
        marker.setFillColor(cityColor);

    marker.setOutlineThickness(2);
    marker.setOutlineColor(whiteColor);

    window.draw(marker);

    drawText(window, font, city->name, 15,
             city->x + 13, city->y - 10,
             whiteColor);
}

// ==========================================
// GRAPH VISUALIZATION
// ==========================================

void drawNetwork(RenderWindow& window, Graph& graph)
{
    CityNode* current = graph.getHead();

    while (current != NULL)
    {
        RoadNode* road = current->roads;

        while (road != NULL)
        {
            CityNode* destination = road->destination;

            // Draw each undirected road only once
            if (current->name < destination->name)
            {
                drawRoad(
                    window,
                    Vector2f(current->x, current->y),
                    Vector2f(destination->x, destination->y)
                );
            }

            road = road->next;
        }

        current = current->next;
    }
}

void drawAllCities(RenderWindow& window,
                   Font& font, Graph& graph,
                   CityNode* highlightedCity)
{
    CityNode* current = graph.getHead();

    while (current != NULL)
    {
        drawCity(window, font, current, highlightedCity);

        current = current->next;
    }
}

// ==========================================
// GRAPH INFORMATION
// ==========================================

int countCities(Graph& graph)
{
    int count = 0;

    CityNode* current = graph.getHead();

    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    return count;
}

int countRoads(Graph& graph)
{
    int connections = 0;

    CityNode* current = graph.getHead();

    while (current != NULL)
    {
        RoadNode* road = current->roads;

        while (road != NULL)
        {
            connections++;
            road = road->next;
        }

        current = current->next;
    }

    // Each undirected road is stored twice
    return connections / 2;
}

// ==========================================
// CITY SELECTION
// ==========================================

bool insideMap(float x, float y)
{
    return x >= 365 && x <= 750 &&
           y >= 145 && y <= 625;
}

CityNode* findCityAt(Graph& graph, float x, float y)
{
    CityNode* current = graph.getHead();

    while (current != NULL)
    {
        float dx = current->x - x;
        float dy = current->y - y;

        if (dx * dx + dy * dy <= 20 * 20)
            return current;

        current = current->next;
    }

    return NULL;
}

// ==========================================
// TRAVERSAL DISPLAY
// ==========================================

string formatTraversal(vector<string> result)
{
    string output = "";

    for (int i = 0; i < (int)result.size(); i++)
    {
        if (i > 0)
            output += " -> ";

        output += result[i];
    }

    return output;
}

// ==========================================
// ADJACENCY LIST DISPLAY
// ==========================================

string getNeighbours(CityNode* city)
{
    string result = "";

    RoadNode* road = city->roads;

    while (road != NULL)
    {
        if (!result.empty())
            result += ", ";

        result += road->destination->name;

        road = road->next;
    }

    if (result.empty())
        result = "No connections";

    return result;
}

// ==========================================
// MAIN PROGRAM
// ==========================================

int main()
{
    RenderWindow window(
        VideoMode(WIDTH, HEIGHT),
        "Ghana City Road Network Visualizer"
    );

    window.setFramerateLimit(60);

    Font font;

    if (!font.loadFromFile(
        "C:/Users/sam/Desktop/New folder/ff/assets/arial.ttf"))
    {
        cout << "Font could not be loaded!" << endl;
        return 1;
    }

    // ======================================
    // CREATE GRAPH
    // ======================================

    Graph ghanaNetwork;

    ghanaNetwork.addCity("Accra", 670, 510);
    ghanaNetwork.addCity("Tema", 705, 505);
    ghanaNetwork.addCity("Kumasi", 555, 400);
    ghanaNetwork.addCity("Sunyani", 500, 315);
    ghanaNetwork.addCity("Tamale", 590, 200);

    ghanaNetwork.addRoad("Accra", "Tema");
    ghanaNetwork.addRoad("Accra", "Kumasi");
    ghanaNetwork.addRoad("Kumasi", "Sunyani");
    ghanaNetwork.addRoad("Sunyani", "Tamale");

    // ======================================
    // INTERACTION VARIABLES
    // ======================================

    ActionMode actionMode = IDLE;
    InputMode inputMode = NO_INPUT;

    bool placingCity = false;
    bool showAdjacency = false;

    string inputText = "";
    string statusMessage = "Select an operation.";

    float newCityX = 0;
    float newCityY = 0;

    CityNode* firstSelectedCity = NULL;
    CityNode* highlightedCity = NULL;

    // ======================================
    // MAIN APPLICATION LOOP
    // ======================================

    while (window.isOpen())
    {
        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            // ==================================
            // KEYBOARD INPUT
            // ==================================

            if (event.type == Event::KeyPressed &&
                event.key.code == Keyboard::Escape)
            {
                placingCity = false;
                inputMode = NO_INPUT;
                actionMode = IDLE;

                firstSelectedCity = NULL;
                inputText = "";

                statusMessage = "Operation cancelled.";
            }

            if (event.type == Event::TextEntered &&
                inputMode != NO_INPUT)
            {
                // BACKSPACE
                if (event.text.unicode == 8)
                {
                    if (!inputText.empty())
                        inputText.pop_back();
                }

                // ENTER
                else if (event.text.unicode == 13)
                {
                    if (inputMode == CITY_INPUT)
                    {
                        if (inputText.empty())
                        {
                            statusMessage =
                                "Enter a city name first.";
                        }
                        else if (ghanaNetwork.findCity(inputText) != NULL)
                        {
                            statusMessage =
                                "City already exists.";
                        }
                        else
                        {
                            bool added = ghanaNetwork.addCity(
                                inputText,
                                newCityX,
                                newCityY
                            );

                            if (added)
                            {
                                statusMessage =
                                    inputText + " added successfully.";
                            }
                            else
                            {
                                statusMessage =
                                    "Could not add city.";
                            }
                        }
                    }

                    // SEARCH CITY
                    else if (inputMode == SEARCH_INPUT)
                    {
                        CityNode* found =
                            ghanaNetwork.findCity(inputText);

                        if (found != NULL)
                        {
                            highlightedCity = found;

                            statusMessage =
                                "City found: " + found->name;
                        }
                        else
                        {
                            highlightedCity = NULL;

                            statusMessage =
                                "City not found.";
                        }
                    }

                    inputText = "";
                    inputMode = NO_INPUT;
                }

                // NORMAL CHARACTERS
                else if (event.text.unicode >= 32 &&
                         event.text.unicode < 127)
                {
                    if (inputText.length() < 24)
                    {
                        inputText +=
                            static_cast<char>(event.text.unicode);
                    }
                }
            }

            // ==================================
            // MOUSE INPUT
            // ==================================

            if (event.type == Event::MouseButtonPressed &&
                event.mouseButton.button == Mouse::Left)
            {
                float mouseX = event.mouseButton.x;
                float mouseY = event.mouseButton.y;

                // IGNORE CLICKS WHILE TYPING
                if (inputMode != NO_INPUT)
                    continue;

                // ==================================
                // LEFT PANEL BUTTONS
                // ==================================

                if (mouseX >= 20 && mouseX <= 225 &&
                    mouseY >= 145 && mouseY <= 188)
                {
                    placingCity = true;
                    actionMode = IDLE;

                    firstSelectedCity = NULL;
                    highlightedCity = NULL;

                    showAdjacency = false;

                    statusMessage =
                        "Click an empty position on the map.";
                }

                else if (mouseX >= 20 && mouseX <= 225 &&
                         mouseY >= 200 && mouseY <= 243)
                {
                    placingCity = false;
                    inputMode = NO_INPUT;
                    actionMode = ADD_ROAD;

                    firstSelectedCity = NULL;
                    highlightedCity = NULL;

                    showAdjacency = false;

                    statusMessage =
                        "Select the first city for the road.";
                }

                else if (mouseX >= 20 && mouseX <= 225 &&
                         mouseY >= 255 && mouseY <= 298)
                {
                    placingCity = false;
                    inputMode = NO_INPUT;
                    actionMode = REMOVE_ROAD;

                    firstSelectedCity = NULL;
                    highlightedCity = NULL;

                    showAdjacency = false;

                    statusMessage =
                        "Select the first city of the road.";
                }

                else if (mouseX >= 20 && mouseX <= 225 &&
                         mouseY >= 310 && mouseY <= 353)
                {
                    placingCity = false;
                    actionMode = IDLE;
                    inputMode = SEARCH_INPUT;

                    firstSelectedCity = NULL;
                    highlightedCity = NULL;

                    showAdjacency = false;

                    inputText = "";

                    statusMessage =
                        "Type a city name and press Enter.";
                }

                else if (mouseX >= 20 && mouseX <= 225 &&
                         mouseY >= 365 && mouseY <= 408)
                {
                    placingCity = false;
                    inputMode = NO_INPUT;
                    actionMode = CHECK_CONNECTION;

                    firstSelectedCity = NULL;
                    highlightedCity = NULL;

                    showAdjacency = false;

                    statusMessage =
                        "Select the first city.";
                }

                else if (mouseX >= 20 && mouseX <= 225 &&
                         mouseY >= 420 && mouseY <= 463)
                {
                    placingCity = false;
                    inputMode = NO_INPUT;
                    actionMode = RUN_BFS;

                    firstSelectedCity = NULL;
                    highlightedCity = NULL;

                    showAdjacency = false;

                    statusMessage =
                        "Select the BFS starting city.";
                }

                else if (mouseX >= 20 && mouseX <= 225 &&
                         mouseY >= 475 && mouseY <= 518)
                {
                    placingCity = false;
                    inputMode = NO_INPUT;
                    actionMode = RUN_DFS;

                    firstSelectedCity = NULL;
                    highlightedCity = NULL;

                    showAdjacency = false;

                    statusMessage =
                        "Select the DFS starting city.";
                }

                else if (mouseX >= 20 && mouseX <= 225 &&
                         mouseY >= 530 && mouseY <= 573)
                {
                    placingCity = false;
                    inputMode = NO_INPUT;
                    actionMode = IDLE;

                    firstSelectedCity = NULL;
                    highlightedCity = NULL;

                    showAdjacency = true;

                    cout << endl;
                    cout << "GHANA ROAD NETWORK" << endl;
                    cout << "==================" << endl;

                    ghanaNetwork.display();

                    statusMessage =
                        "Adjacency list displayed on the right.";
                }

                // ==================================
                // MAP INTERACTIONS
                // ==================================

                else if (placingCity && insideMap(mouseX, mouseY))
                {
                    if (findCityAt(ghanaNetwork, mouseX, mouseY)
                        != NULL)
                    {
                        statusMessage =
                            "Position occupied. Choose another.";
                    }
                    else
                    {
                        newCityX = mouseX;
                        newCityY = mouseY;

                        placingCity = false;
                        inputMode = CITY_INPUT;

                        inputText = "";

                        statusMessage =
                            "Type the city name and press Enter.";
                    }
                }

                else if (actionMode != IDLE)
                {
                    CityNode* selectedCity =
                        findCityAt(ghanaNetwork, mouseX, mouseY);

                    if (selectedCity == NULL)
                    {
                        statusMessage =
                            "Click an existing city.";
                    }

                    // BFS AND DFS
                    else if (actionMode == RUN_BFS)
                    {
                        vector<string> result =
                            ghanaNetwork.BFS(selectedCity->name);

                        string traversal =
                            formatTraversal(result);

                        cout << endl;
                        cout << "BFS TRAVERSAL" << endl;
                        cout << traversal << endl;

                        statusMessage =
                            "BFS: " + traversal;

                        highlightedCity = selectedCity;
                        actionMode = IDLE;
                    }

                    else if (actionMode == RUN_DFS)
                    {
                        vector<string> result =
                            ghanaNetwork.DFS(selectedCity->name);

                        string traversal =
                            formatTraversal(result);

                        cout << endl;
                        cout << "DFS TRAVERSAL" << endl;
                        cout << traversal << endl;

                        statusMessage =
                            "DFS: " + traversal;

                        highlightedCity = selectedCity;
                        actionMode = IDLE;
                    }

                    // FIRST CITY SELECTION
                    else if (firstSelectedCity == NULL)
                    {
                        firstSelectedCity = selectedCity;

                        statusMessage =
                            "Selected: " + selectedCity->name +
                            ". Select the second city.";
                    }

                    // SECOND CITY SELECTION
                    else
                    {
                        if (firstSelectedCity == selectedCity)
                        {
                            statusMessage =
                                "Select a different second city.";
                        }
                        else
                        {
                            // ADD ROAD
                            if (actionMode == ADD_ROAD)
                            {
                                bool added = ghanaNetwork.addRoad(
                                    firstSelectedCity->name,
                                    selectedCity->name
                                );

                                if (added)
                                {
                                    statusMessage =
                                        "Road added: " +
                                        firstSelectedCity->name +
                                        " - " + selectedCity->name;
                                }
                                else
                                {
                                    statusMessage =
                                        "Road exists or operation failed.";
                                }
                            }

                            // REMOVE ROAD
                            else if (actionMode == REMOVE_ROAD)
                            {
                                bool removed = ghanaNetwork.removeRoad(
                                    firstSelectedCity->name,
                                    selectedCity->name
                                );

                                if (removed)
                                {
                                    statusMessage =
                                        "Road removed successfully.";
                                }
                                else
                                {
                                    statusMessage =
                                        "Road does not exist.";
                                }
                            }

                            // CHECK CONNECTIVITY
                            else if (actionMode == CHECK_CONNECTION)
                            {
                                bool connected =
                                    ghanaNetwork.areConnected(
                                        firstSelectedCity->name,
                                        selectedCity->name
                                    );

                                if (connected)
                                {
                                    statusMessage =
                                        firstSelectedCity->name +
                                        " and " + selectedCity->name +
                                        " are connected.";
                                }
                                else
                                {
                                    statusMessage =
                                        firstSelectedCity->name +
                                        " and " + selectedCity->name +
                                        " are not connected.";
                                }
                            }

                            firstSelectedCity = NULL;
                            actionMode = IDLE;
                        }
                    }
                }
            }
        }

        // ======================================
        // DRAW APPLICATION
        // ======================================

        window.clear(background);

        // ======================================
        // LEFT PANEL
        // ======================================

        RectangleShape leftPanel(Vector2f(245, 750));

        leftPanel.setFillColor(panelColor);
        window.draw(leftPanel);

        drawText(window, font, "GHANA NETWORK", 23,
                 20, 30, accentColor);

        drawText(window, font, "NETWORK OPERATIONS", 13,
                 20, 110, mutedColor);

        string operations[] = {
            "Add City",
            "Add Road",
            "Remove Road",
            "Search City",
            "Check Connectivity",
            "BFS Traversal",
            "DFS Traversal",
            "Display Network"
        };

        for (int i = 0; i < 8; i++)
        {
            RectangleShape button(Vector2f(205, 43));

            button.setPosition(20, 145 + i * 55);

            bool active =
                (i == 0 && placingCity) ||
                (i == 1 && actionMode == ADD_ROAD) ||
                (i == 2 && actionMode == REMOVE_ROAD) ||
                (i == 3 && inputMode == SEARCH_INPUT) ||
                (i == 4 && actionMode == CHECK_CONNECTION) ||
                (i == 5 && actionMode == RUN_BFS) ||
                (i == 6 && actionMode == RUN_DFS) ||
                (i == 7 && showAdjacency);

            if (active)
                button.setFillColor(accentColor);
            else
                button.setFillColor(Color(35, 51, 75));

            window.draw(button);

            drawText(window, font, operations[i], 15,
                     34, 157 + i * 55, whiteColor);
        }

        drawText(window, font, "DATA STRUCTURE", 13,
                 20, 610, mutedColor);

        drawText(window, font, "Undirected Graph", 15,
                 20, 642, whiteColor);

        drawText(window, font, "Adjacency List", 15,
                 20, 672, whiteColor);

        // ======================================
        // MAIN MAP AREA
        // ======================================

        drawText(window, font, "GHANA ROAD NETWORK", 24,
                 290, 35, whiteColor);

        drawText(window, font,
                 "Illustrative city connections",
                 14, 290, 72, mutedColor);

        ConvexShape ghana;

        ghana.setPointCount(10);

        ghana.setPoint(0, Vector2f(390, 145));
        ghana.setPoint(1, Vector2f(735, 145));
        ghana.setPoint(2, Vector2f(750, 310));
        ghana.setPoint(3, Vector2f(735, 470));
        ghana.setPoint(4, Vector2f(690, 590));
        ghana.setPoint(5, Vector2f(570, 625));
        ghana.setPoint(6, Vector2f(465, 590));
        ghana.setPoint(7, Vector2f(420, 470));
        ghana.setPoint(8, Vector2f(365, 310));
        ghana.setPoint(9, Vector2f(375, 200));

        ghana.setFillColor(Color(25, 55, 75));
        ghana.setOutlineColor(Color(65, 125, 160));
        ghana.setOutlineThickness(3);

        window.draw(ghana);

        // Draw roads and cities from graph
        drawNetwork(window, ghanaNetwork);

        drawAllCities(
            window,
            font,
            ghanaNetwork,
            highlightedCity
        );

        // ======================================
        // INPUT BOX
        // ======================================

        if (inputMode != NO_INPUT)
        {
            RectangleShape inputBox(Vector2f(300, 55));

            inputBox.setPosition(420, 650);
            inputBox.setFillColor(panelColor);
            inputBox.setOutlineColor(accentColor);
            inputBox.setOutlineThickness(2);

            window.draw(inputBox);

            string label;

            if (inputMode == CITY_INPUT)
                label = "City: ";
            else
                label = "Search: ";

            drawText(window, font,
                     label + inputText + "_",
                     18, 435, 665, whiteColor);
        }

        // ======================================
        // RIGHT INFORMATION PANEL
        // ======================================

        RectangleShape rightPanel(Vector2f(245, 750));

        rightPanel.setPosition(905, 0);
        rightPanel.setFillColor(panelColor);

        window.draw(rightPanel);

        drawText(window, font, "NETWORK INFO", 20,
                 930, 35, accentColor);

        drawText(window, font, "Total Cities", 14,
                 930, 115, mutedColor);

        drawText(window, font,
                 to_string(countCities(ghanaNetwork)),
                 27, 930, 145, whiteColor);

        drawText(window, font, "Total Roads", 14,
                 930, 210, mutedColor);

        drawText(window, font,
                 to_string(countRoads(ghanaNetwork)),
                 27, 930, 240, whiteColor);

        drawText(window, font, "Graph Type", 14,
                 930, 310, mutedColor);

        drawText(window, font, "Undirected", 17,
                 930, 340, whiteColor);

        // ======================================
        // CITY LIST / ADJACENCY LIST
        // ======================================

        if (!showAdjacency)
        {
            drawText(window, font, "CITY LIST", 17,
                     930, 420, accentColor);

            CityNode* current = ghanaNetwork.getHead();

            int position = 460;
            int number = 1;

            while (current != NULL && position < 680)
            {
                drawText(window, font,
                         to_string(number) + ". " + current->name,
                         15, 930, position, whiteColor);

                position += 35;
                number++;

                current = current->next;
            }
        }
        else
        {
            drawText(window, font, "ADJACENCY LIST", 17,
                     930, 420, accentColor);

            CityNode* current = ghanaNetwork.getHead();

            int position = 455;

            while (current != NULL && position < 680)
            {
                drawText(window, font,
                         current->name + ":",
                         13, 930, position, accentColor);

                drawText(window, font,
                         getNeighbours(current),
                         12, 930, position + 19, whiteColor);

                position += 45;

                current = current->next;
            }
        }

        drawText(window, font, "PROJECT 9 | INF311", 13,
                 930, 710, mutedColor);

        // ======================================
        // STATUS MESSAGE
        // ======================================

        drawText(window, font, statusMessage, 12,
                 290, 710, accentColor);

        window.display();
    }

    return 0;
}
