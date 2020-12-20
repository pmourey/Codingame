#pragma GCC optimize("O3")
#pragma GCC optimize("inline")
#pragma GCC optimize("omit-frame-pointer")
#pragma GCC optimize("unroll-loops") //Optimization flags
//#pragma GCC optimize("ffast-math")
#pragma GCC optimize("Ofast")

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <bits/stdc++.h>
#include <exception>


using namespace std;

#define DEBUG

#ifdef DEBUG
#include <fstream>

ifstream Inputfile;

#define cin     Inputfile

#endif  //#ifdef DEBUG

int timeoutMS = 1000;

uint64_t start;

int loop_elapsed = 0;
int maxDepth = 7;
long nodeCount = 0;

void debug(string message)
{
    cerr << message << endl;
}

struct Action {
    int lastActionId;
    int score;
    vector<int> seeds;
};

  
// Represents a node of an n-ary tree 
struct Node {
    vector<int> seeds;
    int lastActionId;
    int depth;
    int score;
    int value;
    vector<Node *> children;
};

 // Utility function to create a new tree node 
Node *newNode(vector<int> seeds, int lastActionId, int depth, int score, int value) 
{ 
    Node *temp = new Node; 
    temp->seeds = seeds; 
    temp->lastActionId = lastActionId; 
    temp->depth = depth; 
    temp->score = score; 
    temp->value = value; 
    return temp; 
} 
  
// Prints the n-ary tree level wise 
void LevelOrderTraversal(Node * root) 
{ 
    if (root==NULL) 
        return; 
   
    // Standard level order traversal code 
    // using queue 
    queue<Node *> q;  // Create a queue 
    q.push(root); // Enqueue root  
    while (!q.empty()) 
    { 
        int n = q.size(); 
  
        // If this node has children 
        while (n > 0) 
        { 
            // Dequeue an item from queue and print it 
            Node * p = q.front(); 
            q.pop();
            cerr << p->lastActionId << " -> ";
            for (int i=0;i<12;i++)
                cerr << p->seeds[i] << " "; 
            cerr << "| ";
            // Enqueue all children of the dequeued item 
            for (int i=0; i<p->children.size(); i++) 
                q.push(p->children[i]); 
            n--; 
        } 
   
        cerr << endl; // Print new line between two levels 
    } 
} 

int miniMax(Node *node, int depth, bool maximizingPlayer)
{
    int value;
    if (depth == 0 || node->children.empty())
        return node->score;

    if (maximizingPlayer)
    {
        value = -10000;
        for (int i=0; i < node->children.size(); i++)
            value = max(value, miniMax(node->children[i], depth - 1, false));
        return value;
    }
    else  // (* minimizing player *)
    {
        value = 10000;
        for (int i=0; i < node->children.size(); i++)
            value = min(value, miniMax(node->children[i], depth - 1, true));
        return value;
    }
}


int alphaBeta(Node *node, int i) {
    // si nœud est une feuille alors
    //     retourner la valeur de nœud
    //  if not node.children:
    // if node.children is not None:
    int j;
    if (node->children.empty())
        return node->score;
    else
    {
        j = -10000;
        for (int i=0; i < node->children.size(); i++)
        {
            j = max(j, alphaBeta(node->children[i], j));
            if (-j <= i)
                return -j;
            return -j;
        }
    }
}

uint64_t millis()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void throwIfTimeout(int curMaxDepth)
{
    uint64_t elapsed = millis() - start;

    //if (elapsed >= TIMEOUT_MS || curMaxDepth > maxDepth)
    if (elapsed >= timeoutMS)
    {
        debug("Elapsed ( " + (to_string(elapsed)) + " > " + to_string(timeoutMS) + " ms) with finished depth " + to_string(curMaxDepth - 1));
        //throw runtime_error("No more time or maxDepth reached: " + to_string(maxDepth));
        throw runtime_error("No more time");
    }
}

int maxValue(vector<Node *> const& nodeList) {
    int maxValue = 0, value;
    for (int i = 0; i < nodeList.size(); i++) {
        value = nodeList[i]->value;
        if (value > maxValue)
            maxValue = value;
    }
    return maxValue;
}

int sum(vector<int> const& seeds, int start, int end)
{
    int somme = 0;
    for (int i = start; i <= end; i++)
        somme += seeds[i];
    return somme;
}

 vector<int> reverse(vector<int> const& seeds) {
    vector<int> revSeeds(12);
    for (int i = 0; i < 12; i++) {
        revSeeds[i] = seeds[11 - i];
    }
    return revSeeds;
}

int evaluate(vector<int> const& seeds) {
    int score = 0;
    for (int i = 0; i < 6; i++) {
        if (1 <= seeds[i] && seeds[i] <= 2)
            score += 3;
        if (seeds[i] == 0)
            score += 4;
        if (seeds[i] >= 12)
            score += 2;
    }
    return score;
}

Action getActionScore(int id, vector<int> const& seeds)
{
    Action actionScoreResult;
    actionScoreResult.lastActionId = id;
    vector<int> newSeeds(12);
    newSeeds = seeds;
    int s = newSeeds[id];
    newSeeds[id] = 0;
    int score = 0;
    int captures = 0;
    int consecutiveCaptures = 0;
    int j = id;
    for (int i = 0; i < s; i++) {
        j = (i == 11) ? (j + 2) % 12 : (j + 1) % 12;
        newSeeds[j] += 1;
    }
    while (6 <= j and j < 12 and (newSeeds[j] == 2 or newSeeds[j] == 3)) {
        captures += newSeeds[j];
        newSeeds[j] = 0;
        j += 1;
        consecutiveCaptures += 1;
    }
    int mySum, opSum, myNewSum, opNewSum;
    mySum = sum(seeds, 0, 5);
    myNewSum = sum(newSeeds, 0, 5);
    opSum = sum(seeds, 6, 11);
    opNewSum = sum(newSeeds, 6, 11); 
    score -= opNewSum - opSum - captures;
    if (opNewSum == 0) {
        actionScoreResult.score = -1000;
        actionScoreResult.seeds = newSeeds;
        return actionScoreResult;
    }
    if ((opSum == 0 && opNewSum > 0) || (mySum > 0 and myNewSum == 0))
        score += 100;
    score += captures * 2 + consecutiveCaptures;
    score += evaluate(newSeeds) - evaluate(reverse(newSeeds));
    score += 1;
    actionScoreResult.score = score;
    actionScoreResult.seeds = newSeeds;
    return actionScoreResult;
}


void buildGraph(Node *node) {
    try {
        //throwIfTimeout(node->depth);
        if (node->depth > maxDepth)
            return;
        vector<Action> actions;
        vector<int> evalResult;
        for (int i = 0; i < 6; i++)
            if (node->seeds[i] > 0) {
                Action action = getActionScore(i, node->seeds);
                if (action.score >= -100)
                    actions.push_back(action);
            }
        if (actions.empty())
            return;
        for (int i = 0; i < actions.size(); i++) {
            Node *childNode = newNode(reverse(actions[i].seeds), actions[i].lastActionId, node->depth + 1, actions[i].score, actions[i].score);
            nodeCount += 1;
            (node->children).push_back(childNode); 
            buildGraph(childNode);
        }
    }
    catch (const runtime_error &e)
    {
        // Do nothing
    }
}

string vector_to_str(vector<int> const& seeds) {
    string str;
    for (int i=0;i<12;i++)
        str += to_string(seeds[i]) + " ";
    return str;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    #ifdef DEBUG

    Inputfile.open("input.txt");

    #endif // #ifdef DEBUG

    srand(time(0));

    uint64_t start;

    // game loop
    bool is_start = true;
    while (is_start) {
        nodeCount = 0;
        start = millis();
        vector<int> seeds(12);
        for (int i = 0; i < 12; i++) {
            int s;
            cin >> s; cin.ignore();
            seeds[i] = s;
        }
        cerr << vector_to_str(seeds) <<endl;
        Node *root = newNode(seeds, 0, 0, 0, 0); 
        buildGraph(root);
        #ifdef DEBUG
        cerr << "Tree pring disabled" <<endl;
        //LevelOrderTraversal(root);
        #endif // #ifdef DEBUG

        vector<Node *> actionsCandidates;
        vector<Node *> nodeSelection;
        Node *bestNode;
        int value, bestScore, bestValue;
        int max_value = -1000;
        int min_value;
        int action;
        for (int i = 0; i < root->children.size(); i++) {
            Node *node = root->children[i];
            value = miniMax(node, maxDepth, true);
            min_value = -1000;
            //value = alphaBeta(node, min_value);
            if (value > max_value) {
                max_value = value;
                node->value = value;
                actionsCandidates.push_back(node);
                bestNode = node;
            }
        }
        for (int i = 0; i < root->children.size(); i++)
            cerr << "root action: " << root->children[i]->lastActionId << " - Seeds : " << vector_to_str(reverse(root->children[i]->seeds)) << " - Score: " << root->children[i]->score << " - Value: " << root->children[i]->value <<endl;

        cerr << "Max value: " << bestNode->value <<endl;

        //if (actionsCandidates.size() == 0)  // no IA solution given in time, select best move by default
        if (bestNode == nullptr)  // no IA solution given in time, select best move by default
            //if (!root->children.empty()) {
            if (root->children.size() > 0) {
                bestScore = maxValue(root->children);
                cerr << "Min Max Failed - bestScore = " << bestScore <<endl;
                /* eprint("Min Max Failed - Action : {} - Score = {} - cpu_time: {:.2f} ms".format(best_node.last_action_id, best_node.score, elapsed_time)) */
                for (int i = 0; i < root->children.size(); i++) {
                    if (root->children[i]->score == bestScore)
                        nodeSelection.push_back(root->children[i]);
                }
                cerr << "root->children.size() = " << (root->children).size() <<endl;
                cerr << "nodeSelection.size() = " << nodeSelection.size() <<endl;
                bestNode = nodeSelection[rand() % nodeSelection.size()];
                cerr << "PROUT" <<endl;
                action = bestNode->lastActionId;
                cerr << "no IA solution given in time, select best move by default" << endl;
                cerr << "action: " << bestNode->lastActionId << " - Seeds : " << vector_to_str(reverse(bestNode->seeds)) << endl;
            }
            else {
                vector<int> randActionList;
                for (int i = 6; i < 12; i++)
                    if (seeds[i] > 0)
                        randActionList.push_back(i);
                action = randActionList[rand() % randActionList.size()];
                cerr << "There was a bug!" <<endl;
            }
        else
        {
            action = bestNode->lastActionId;
        }
        
        // else {
        //     bestValue = maxValue(actionsCandidates); // à debugger
        //     cerr << "bestValue = " << bestValue <<endl;
        //     vector<Node *> nodeSelectionFinal;
        //     for (int i = 0; i < actionsCandidates.size(); i++) {
        //         //cerr << i << " - score: " << actionsCandidates[i]->score <<endl;
        //         if (actionsCandidates[i]->value == bestValue)
        //             nodeSelectionFinal.push_back(actionsCandidates[i]);
        //     }
        //     //cerr << "nodeSelectionFinal.size() = " << nodeSelectionFinal.size() <<endl;
        //     if (nodeSelectionFinal.empty()) {
        //         vector<int> randActionList;
        //         for (int i = 0; i < 6; i++)
        //             if (seeds[i] > 0)
        //                 randActionList.push_back(i);
        //         action = randActionList[rand() % randActionList.size()];
        //         cerr << "There was a bug!" <<endl;
        //     }
        //     else {
        //         bestNode = nodeSelectionFinal[rand() % nodeSelectionFinal.size()];
        //         action = bestNode->lastActionId;
        //         cerr << "Min Max action: " << bestNode->lastActionId << " - Value: " << bestNode->value << " - Seeds : " << vector_to_str(reverse(bestNode->seeds)) << endl;
        //     }
        // }
        cout << "Best action: " << action <<endl;
        maxDepth = 5;
        timeoutMS = 50;
        #ifdef DEBUG
        is_start = false;
        #endif // #ifdef DEBUG

        uint64_t elapsed = millis() - start;
        cerr << "Node count: " << nodeCount << " - Time: " << elapsed << " ms" <<endl;
    }
}
