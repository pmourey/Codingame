#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum PieceColor
{
    NONE,
    WHITE,
    BLACK
};

int boardSize;
vector<vector<PieceColor> > board; // the board

class PieceGroup
{
public:
    PieceGroup(PieceColor col) { color = col; }

    PieceGroup(int x, int y, PieceColor col)
    {
        color = col;
        locations.push_back(y * boardSize + x); // easier than storing two different
                                                // numbers
    }

    ~PieceGroup() {}

    PieceColor getColor() { return color; }

    // return the number of pieces in the group
    int getSize() { return locations.size(); }

    // returns the location at a given index
    int getLocation(int index)
    {
        if (index >= 0 && index < getSize())
        {
            return locations[index];
        }
        else
            return -1;
    }

    // returns whether the group contains a particular spot on the board
    bool contains(int x, int y)
    {
        for (std::vector<int>::iterator it = locations.begin();
             it != locations.end(); ++it)
        {
            if ((y * boardSize + x) == (*it))
                return true;
        }
        return false;
    }

    void addPiece(int x, int y) { locations.push_back(y * boardSize + x); }

    // put the contents of this group to the input group and return the result
    // should I check for duplicates?
    PieceGroup combine(PieceGroup other)
    {
        int tempx, tempy;
        for (std::vector<int>::iterator it = locations.begin();
             it != locations.end(); ++it)
        {
            tempx = (*it) % boardSize;
            tempy = (*it) / boardSize;
            other.addPiece(tempx, tempy);
        }
        return other;
    }

    // returns whether the input piece is connected to this group
    bool isConnected(int x, int y)
    {
        int tempx, tempy;
        for (std::vector<int>::iterator it = locations.begin();
             it != locations.end(); ++it)
        {
            tempx = (*it) % boardSize;
            tempy = (*it) / boardSize;

            // at least one of the coordinates should be the same
            if (tempx != x && tempy != y)
                continue;

            // only reaches this point if tempx equals x or tempy equals y
            // if that's the case, check whether the other coordinate is one away
            if (tempx - x == 1 || x - tempx == 1 || tempy - y == 1 || y - tempy == 1)
                return true;
        }

        return false;
    }

    // debug functions
    void printSelf()
    {
        for (std::vector<int>::iterator it = locations.begin();
             it != locations.end(); ++it)
        {
            std::cout << (*it) % boardSize << " " << (*it) / boardSize << std::endl;
        }
    }

private:
    std::vector<int> locations;
    PieceColor color;
};

class Board
{
public:
    Board()
    {
        for (int i = 0; i < boardSize; ++i)
        {
            vector<PieceColor> v;
            for (int j = 0; j < boardSize; ++j)
            {
                v.push_back(NONE);
            }
            board.push_back(v);
            turn = BLACK;
        }
    }

    ~Board() {}

    // get the color of the piece at location (x, y) on the board
    PieceColor getPiece(int x, int y) { return board[y][x]; }

    // returns the opposite color of the input color
    PieceColor oppositeColor(PieceColor input)
    {
        if (input == BLACK)
            return WHITE;
        else if (input == WHITE)
            return BLACK;
        else
            return NONE;
    }

    // empties the location on the board
    void removePiece(int x, int y) { board[y][x] = NONE; }

    // put a piece of color PieceColor at location (x, y) on the board
    void placePiece(int x, int y, PieceColor c)
    {
        // check if the right color is being placed
        if (c != turn)
            return;

        if (getPiece(x, y) != NONE)
            return; // don't allow piece to be placed on top of another one

        // put the piece on the board
        board[y][x] = c;

        // process groups
        processGroups(x, y, c);

        // check to see if the move captures any pieces
        checkCaptures(x, y, c);

        // switch the turn
        turn = oppositeColor(turn);
    }

    // takes the input piece and sees if it can be added to any groups
    // if so, the piece is added the appropriate groups
    // combines groups if necessary
    void processGroups(int x, int y, PieceColor c)
    {
        // if there are no groups, create one
        if (groups.size() <= 0)
            groups.push_back(PieceGroup(x, y, c));

        else
        {
            std::vector<int> connections; // hold the groups so they can be removed

            // loop through groups and look for whether the piece is connected to
            // them
            for (unsigned int i = 0; i < groups.size(); ++i)
            {
                // make sure the color of the group matches the piece being placed
                // if the piece is connected to the group, we want to add it to that
                // group
                if (groups[i].getColor() == c && groups[i].isConnected(x, y))
                {
                    connections.push_back(i);
                }
            }

            // if there are no connections, add the piece as a group
            if (connections.size() <= 0)
                groups.push_back(PieceGroup(x, y, c));

            // otherwise add it to the groups and combine them
            else
            {
                PieceGroup temp = PieceGroup(c); // initialize an empty group

                temp.addPiece(x, y); // add the piece to the group

                // now loop through and combine all connected groups
                for (std::vector<int>::iterator it = connections.begin();
                     it != connections.end(); ++it)
                {
                    temp = groups[*it].combine(temp);
                }

                // now go through and remove all the groups from the list
                // remove in backwards order to avoid messing up index
                for (int i = connections.size() - 1; i >= 0; --i)
                {
                    groups.erase(groups.begin() + connections[i]);
                }

                // now add the new group to the list
                groups.push_back(temp);
            }
        }
    }

    // checks whether each adjacent piece to the placed piece is dead
    void checkCaptures(int x, int y, PieceColor c)
    {
        if (getPiece(x - 1, y) == oppositeColor(c))
        {
            if (calcLiberties(x - 1, y) == 0)
                removeGroup(x - 1, y);
        }
        if (getPiece(x + 1, y) == oppositeColor(c))
        {
            if (calcLiberties(x + 1, y) == 0)
                removeGroup(x + 1, y);
        }
        if (getPiece(x, y - 1) == oppositeColor(c))
        {
            if (calcLiberties(x, y - 1) == 0)
                removeGroup(x, y - 1);
        }
        if (getPiece(x, y + 1) == oppositeColor(c))
        {
            if (calcLiberties(x, y + 1) == 0)
                removeGroup(x, y + 1);
        }
    }

    // removes the group that contains the input piece
    void removeGroup(int x, int y)
    {
        int ind = -1;
        for (int i = 0; i < (int)groups.size(); ++i)
        {
            // if the input piece is found in a group, remove it and return
            if (groups[i].contains(x, y))
            {
                ind = i;
                break;
            }
        }

        if (ind == -1)
        {
            std::cout << "SOMETHING WENT WRONG" << std::endl;
            return;
        }

        // clear the pieces in the group from the board
        int tempx, tempy;
        for (int i = 0; i < groups[ind].getSize(); ++i)
        {
            tempx = groups[ind].getLocation(i) % boardSize;
            tempy = groups[ind].getLocation(i) / boardSize;
            removePiece(tempx, tempy);
        }

        // now delete the group
        groups.erase(groups.begin() + ind);
    }

    // calculate the number of liberties remaining for a piece group
    int calcLiberties(PieceGroup grp)
    {
        int sum = 0;
        for (int i = 0; i < grp.getSize(); ++i)
        {
            sum += calcLiberties(grp.getLocation(i) % boardSize, grp.getLocation(i) / boardSize);
        }
        return sum;
    }

    // calculate liberties for a single piece
    int calcLiberties(int x, int y)
    {
        int sum = 0;

        // check the four adjacent locations for a piece and add a liberty if it
        // is empty
        if (getPiece(x - 1, y) == NONE)
            sum += 1;
        if (getPiece(x + 1, y) == NONE)
            sum += 1;
        if (getPiece(x, y - 1) == NONE)
            sum += 1;
        if (getPiece(x, y + 1) == NONE)
            sum += 1;
        return sum;
    }

    // debug functions

    void printSelf() { std::cout << turn << std::endl; }

    void printDebug()
    {
        std::cout << "Num groups: " << groups.size() << std::endl
                  << std::endl;
        for (int i = 0; i < boardSize; ++i)
        {
            for (int j = 0; j < boardSize; ++j)
            {
                std::cout << getPiece(j, i) << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "GROUPS" << std::endl;
        for (std::vector<PieceGroup>::iterator it = groups.begin();
             it != groups.end(); ++it)
        {
            it->printSelf();
            std::cout << std::endl;
        }
    }

private:
    std::vector<PieceGroup> groups;
    // PieceColor board[19][19];
    PieceColor turn;
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    // cin >> S; cin.ignore();
    boardSize = 5; // the size of the board
    Board b;
    // BB...
    // B....
    // .....
    // ...W.
    // .....

    vector<string> grid_rows;
    grid_rows.push_back("BB...");
    grid_rows.push_back("B....");
    grid_rows.push_back(".....");
    grid_rows.push_back("...W.");
    grid_rows.push_back(".....");

    int M; // the number of moves to be made
    M = 7;
    // cin >> M; cin.ignore();

    // B 3 2
    // W 0 2
    // B 3 4
    // W 2 0
    // B 2 3
    // W 1 1
    // B 4 3
    vector<string> moves;
    moves.push_back("B 3 2");
    moves.push_back("W 0 2");
    moves.push_back("B 3 4");
    moves.push_back("W 2 0");
    moves.push_back("B 2 3");
    moves.push_back("W 1 1");
    moves.push_back("B 4 3");

    char x, y, color, data;
    string move;

    for (int y = 0; y < boardSize; ++y)
    {
        // getline(cin, row); // a single row in the input board
        for (int x = 0; x < boardSize; ++x)
        {
            color = grid_rows[y][x];
            if (color == 'B')
                b.placePiece(x, y, BLACK);
            else if (color == 'W')
                b.placePiece(x, y, WHITE);
        }
    }
    for (int i = 0; i < moves.size(); i++)
    {
        move = moves[i];
        // getline(cin, move); // a strng representation of a move
        color = move[0];
        x = move[2];
        y = move[4];
        if (color == 'B')
            b.placePiece(x, y, BLACK);
        else if (color == 'W')
            b.placePiece(x, y, WHITE);
    }

    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;
    b.printDebug();
    return 0;
    cout << "NOT_VALID | <<the_board_after_the_moves>>" << endl;
}
