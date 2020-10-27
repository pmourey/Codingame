#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    int numberMineSpots;
    cin >> numberMineSpots; cin.ignore();
    for (int i = 0; i < numberMineSpots; i++) {
        int x;
        int y;
        cin >> x >> y; cin.ignore();
    }

    // game loop
    while (1) {
        int gold;
        cin >> gold; cin.ignore();
        int income;
        cin >> income; cin.ignore();
        int opponentGold;
        cin >> opponentGold; cin.ignore();
        int opponentIncome;
        cin >> opponentIncome; cin.ignore();
        for (int i = 0; i < 12; i++) {
            string line;
            cin >> line; cin.ignore();
        }
        int buildingCount;
        cin >> buildingCount; cin.ignore();
        for (int i = 0; i < buildingCount; i++) {
            int owner;
            int buildingType;
            int x;
            int y;
            cin >> owner >> buildingType >> x >> y; cin.ignore();
        }
        int unitCount;
        cin >> unitCount; cin.ignore();
        for (int i = 0; i < unitCount; i++) {
            int owner;
            int unitId;
            int level;
            int x;
            int y;
            cin >> owner >> unitId >> level >> x >> y; cin.ignore();
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << "WAIT" << endl;
    }
}