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

using namespace std;

//#define DEBUG

#ifdef DEBUG
#include <fstream>

ifstream Inputfile;

#define cin     Inputfile

#endif  //#ifdef DEBUG

const int TIMEOUT_MS = 50;

uint64_t start;

int loop_elapsed = 0;
int maxDepth = 5;
int nodeCount = 0;

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
            cout << p->lastActionId << " -> ";
            for (int i=0;i<12;i++)
                cout << p->seeds[i] << " "; 
   
            // Enqueue all children of the dequeued item 
            for (int i=0; i<p->children.size(); i++) 
                q.push(p->children[i]); 
            n--; 
        } 
   
        cout << endl; // Print new line between two levels 
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
            value = min(value, miniMax(node->children[i], depth - 1, false));
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
            if (-j < i)
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

    if (elapsed >= TIMEOUT_MS)
    {
        debug("Elapsed ( " + (to_string(elapsed)) + " > " + to_string(TIMEOUT_MS) + " ms) with finished depth " + to_string(curMaxDepth - 1));
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
    vector<int> newSeeds(12);
    newSeeds = seeds;
    int s = newSeeds[id];
    newSeeds[id] = 0;
    int score = 0;
    int captures = 0;
    int consecutiveCaptures = 0;
    // holes_hit = []
    int j = id;
    for (int i = 0; i < s; i++) {
        j = (i == 11) ? (j + 2) % 12 : (j + 1) % 12;
        newSeeds[j] += 1;
    }
    // if (std::binary_search(v.begin(), v.end(), key))
    while (6 <= j and j < 12 and (newSeeds[j] == 2 or newSeeds[j] == 3)) {
        captures += newSeeds[j];
        newSeeds[j] = 0;
        j += 1;
        consecutiveCaptures += 1;
        // holes_hit.append(j)
    }
    score += captures * 2 + consecutiveCaptures;
    // Attaquer simultanément deux trous ou plus de l'adversaire; afin que l adversaire ne puisse pas éviter la capture ultérieure de ses graines.
    // score += consec_captures
    // Gagner du temps est une tactique importante dans la gestion consistant à forcer l'adversaire à fournir des graines au joueur afin d'obtenir un avantage stratégique.
    // seeds_given = sum(new_seeds[6:]) - sum(seeds[6:])
    score -= sum(newSeeds, 6, 11) - sum(seeds, 6, 11) - captures;
    // Amasser des graines sur le propre territoire du joueur en les semant dans les fosses de l'adversaire uniquement en petites quantités.
    // Forçant ainsi l'adversaire à faire des gestes non favorables à ses intérêts.'''
    // old_small_holes = sum([i for i, v in enumerate(seeds) if v <= 1 and i in holes_hit and i >= 6])
    // new_small_holes = sum([i for i, v in enumerate(new_seeds) if v <= 1 and i in holes_hit and i >= 6])
    // score += new_small_holes - old_small_holes
    // if (sum(newSeeds, 6, 11) == 0) {
    //     actionScoreResult.score = -1000;
    //     actionScoreResult.seeds = newSeeds;
    //     return actionScoreResult;
    // }
    // else if ((sum(seeds, 6, 11) == 0 && sum(newSeeds, 6, 11)) || (sum(seeds, 0, 5) && sum(newSeeds, 0, 5) == 0))
    //     score += 100;
    //reverse_copy(newSeeds.begin(), newSeeds.end(), newSeeds2.begin());
    score += evaluate(newSeeds) - evaluate(reverse(newSeeds));
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
                // cerr << "getActionScore call" <<endl;
                Action action = getActionScore(i, node->seeds);
                if (action.score >= -100)
                    actions.push_back(action);
            }
        if (actions.empty())
            return;
        vector<int> revSeeds = reverse(node->seeds);
        for (int i = 0; i < actions.size(); i++) {
            Node *childNode = newNode(revSeeds, i, node->depth + 1, actions[i].score, actions[i].score);
            nodeCount += 1;
            (node->children).push_back(childNode); 
            // cerr << "Hello world buildGraph" <<endl;
            buildGraph(childNode);
        }
    }
    catch (const runtime_error &e)
    {
        // nothing to do
    }
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

    uint64_t start = millis();

    int maxDepth = 5;
    // game loop
    bool is_start = true;
    while (is_start) {
        start = millis();
        vector<int> seeds(12);
        for (int i = 0; i < 12; i++) {
            int s;
            cin >> s; cin.ignore();
            cerr << s << " ";
            seeds[i] = s;
        }

        Node *root = newNode(seeds, -1, 0, 0, 0); 
        buildGraph(root);
        //LevelOrderTraversal(root);
        // eprint(tree)

        vector<Node *> actionsCandidates;
        vector<Node *> nodeSelection;
        Node *best_node;
        int value, bestScore;
        int max_value = 0;
        int min_value = -90;
        int action;
        for (int i = 0; i < root->children.size(); i++) {
            Node *node = root->children[i];
            //value = miniMax(node, maxDepth, true);
            value = alphaBeta(node, min_value);
            if (value >= max_value) {
                max_value = value;
                node->value = value;
                actionsCandidates.push_back(node);
            }
        }
        cerr << "Max value: " << max_value <<endl;

        if (actionsCandidates.empty())  // no IA solution given in time, select best move by default
            if (!root->children.empty()) {
                bestScore = maxValue(root->children);
                /* eprint("Min Max Failed - Action : {} - Score = {} - cpu_time: {:.2f} ms".format(best_node.last_action_id, best_node.score, elapsed_time)) */
                for (int i = 0; i < root->children.size(); i++) {
                    if (root->children[i]->score == bestScore)
                        nodeSelection.push_back(root->children[i]);
                Node *bestNode = nodeSelection[rand() % nodeSelection.size()];
                }
            }
            else {
                vector<int> randActionList;
                for (int i = 6; i < 12; i++)
                    if (seeds[i])
                        randActionList.push_back(i);
                int action = randActionList[rand() % randActionList.size()];
            }
        else {
            bestScore = maxValue(actionsCandidates); // à debugger
            vector<Node *> nodeSelectionFinal;
            for (int i = 0; i < actionsCandidates.size(); i++)
                if (actionsCandidates[i]->score == bestScore)
                    nodeSelectionFinal.push_back(actionsCandidates[i]);
            action = nodeSelectionFinal[rand() % nodeSelectionFinal.size()]->lastActionId;
        }
        cout << action <<endl;
        maxDepth = 5;
        #ifdef DEBUG
        is_start = false;
        #endif // #ifdef DEBUG

        uint64_t elapsed = millis() - start;
        cerr << "Node count: " << nodeCount << " - Time: " << elapsed << " ms" <<endl;
    }
}
