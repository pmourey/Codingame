#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
#include <chrono>

using namespace std;

/**
 * Remove connected regions of the same color to obtain the best score.
 **/


const int n = 15; // lignes
const int m = 15; // colonnes

// stores information about which cell
// are already visited in a particular BFS
int visited[n][m];

// result stores the final result grid
int result[n][m];
int x, y;

// stores the count of cells in the largest
// connected component
int COUNT;

// Function checks if a cell is valid i.e it
// is inside the grid and equal to the key
bool is_valid(int x, int y, int key, int input[n][m])
{
    if (x < n && y < m && x >= 0 && y >= 0 && input[x][y] != -1) {
        if (visited[x][y] == false && input[x][y] == key)
            return true;
        else
            return false;
    }
    else
        return false;
}

// BFS to find all cells in
// connection with key = input[i][j]
void BFS(int x, int y, int i, int j, int input[n][m])
{
    // terminating case for BFS
    if (x != y)
        return;
    
    visited[i][j] = 1;
    COUNT++;
    
    // x_move and y_move arrays
    // are the possible movements
    // in x or y direction
    int x_move[] = { 0, 0, 1, -1 };
    int y_move[] = { 1, -1, 0, 0 };
    
    // checks all four points connected with input[i][j]
    for (int u = 0; u < 4; u++)
        if (is_valid(i + y_move[u], j + x_move[u], x, input))
            BFS(x, y, i + y_move[u], j + x_move[u], input);
}

// called every time before a BFS
// so that visited array is reset to zero
void reset_visited()
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            visited[i][j] = 0;
        }
}

// If a larger connected component
// is found this function is called
// to store information about that component.
void reset_result(int key, int input[n][m])
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (visited[i][j] && input[i][j] == key)
                result[i][j] = visited[i][j];
            else
                result[i][j] = 0; //false
        }
    }
}


// function to get a tile in one of biggest region
void get_result(int res)
{
    cerr << "The largest connected "
    << "component of the grid is :" << res << "\n";
    
    // prints the largest component
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (result[j][n-1-i]) {
                cerr << result[j][n-1-i] << " ";
                x = j; y = n-1-i;
                //cerr << i << " " << j;
            }
            else
                //cerr << result[i][j] << " ";
                cerr << ". ";
        }
        cerr << "\n";
    }
}

// function to calculate the largest connected
// component
int computeLargestConnectedGrid(int input[n][m])
{
    int current_max = INT_MIN;

    //cerr << "INT_MIN = " << INT_MIN <<endl;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            reset_visited();
            COUNT = 0;
            
            // checking cell to the right
            if (j + 1 < m)
                BFS(input[i][j], input[i][j + 1], i, j, input);
            
            // updating result
            if (COUNT >= current_max) {
                current_max = COUNT;
                reset_result(input[i][j], input);
            }
            reset_visited();
            COUNT = 0;
            
            // checking cell downwards
            if (i + 1 < n)
                BFS(input[i][j], input[i + 1][j], i, j, input);
            
            // updating result
            if (COUNT >= current_max) {
                current_max = COUNT;
                reset_result(input[i][j], input);
            }
        }
    }
    cerr << "current_max = " << current_max <<endl;
    get_result(current_max);    
    return(current_max);
}


int main()
{
    bool start = true;
    int input[n][m];
    int max_tiles = 0;
    // game loop
    while (1) {
        auto begin = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                int color; // Color of the tile
                cin >> color; cin.ignore();
                input[j][n-1-i] = color;
            }
            cerr << endl;
        }
        max_tiles = computeLargestConnectedGrid(input);
        auto end = chrono::steady_clock::now();
        auto time = chrono::duration_cast<chrono::milliseconds>(end - begin).count();
        string message = "time: " + std::to_string(time) + " ms";

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
        cout << x << " " << y << " " << alert << "\\n" << message << endl; // Selected tile "x y [message]".
        //cout << x << " " << y; // Selected tile "x y [message]".
        //cout << " x=" << x << " y=" << y << "\\n" << "lrl=" << max_tiles << endl; 
    }
}