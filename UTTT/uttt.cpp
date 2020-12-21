#pragma GCC optimize("O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops") //Optimization flags
#pragma GCC optimize("ffast-math")
#pragma GCC optimize("Ofast")

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

const int TIMEOUT_MS = 40;

uint64_t start;

void debug(string message)
{
    cerr << message << endl;
}

struct Node{
    int player = 0;                 // Either 0 or 1
    int appliedMove = -1;           // what move was applied to reach this node
    Node *parent = nullptr;         // what is the parent node
    GameEngine ge;                  // the game engine at the state of node.
    vector<Node> unvisitedChildren; // vector of unvisited children.
    vector<Node> visitedChildren;   // vector of visited children.
};

uint64_t millis()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void throwIfTimeout(int curMaxDepth)
{
    auto elapsed = millis() - start;

    if (elapsed >= TIMEOUT_MS)
    {
        debug("Elapsed ( " + (to_string(elapsed)) + " > " + to_string(TIMEOUT_MS) + " ms) with finished depth " + to_string(curMaxDepth - 1));
        throw runtime_error("No more time");
    }
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{

    // game loop
    while (1) {
        int opponentRow;
        int opponentCol;
        cin >> opponentRow >> opponentCol; cin.ignore();
        int validActionCount;
        cin >> validActionCount; cin.ignore();

        start = millis();

        for (int i = 0; i < validActionCount; i++) {
            int row;
            int col;
            cin >> row >> col; cin.ignore();
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << "0 0" << endl;
    }
}