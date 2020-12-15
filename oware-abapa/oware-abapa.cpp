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

const int MAX_DEPTH = 6;
const int TIMEOUT_MS = 40;

uint64_t start;

int loop_elapsed = 0;

void debug(string message)
{
    cerr << message << endl;
}

struct Node {
    vector<int> seeds;
    int lastActionId;
    int depth;
    int score;
    vector<Node> children;
};

struct Action {
    int lastActionId;
    int score;
    vector<int> seeds;
};


int miniMax(Node node, int depth, bool maximizingPlayer)
{
    int value;
    if (depth == 0 || node.children.size() == 0)
        return node.score;

    if (maximizingPlayer)
    {
        value = -10000;
        for (int i=0; i < node.children.size(); i++)
            value = max(value, miniMax(node.children[i], depth - 1, false));
        return value;
    }
    else  // (* minimizing player *)
    {
        value = 10000;
        for (int i=0; i < node.children.size(); i++)
            value = min(value, miniMax(node.children[i], depth - 1, false));
        return value;
    }
}

int alphaBeta(Node node, int i) {
    // si nœud est une feuille alors
    //     retourner la valeur de nœud
    //  if not node.children:
    // if node.children is not None:
    int j;
    if (node.children.size() == 0)
        return node.score;
    else 
    {
        j = -10000;
        for (int i=0; i < node.children.size(); i++)
        {
            j = max(j, alphaBeta(node.children[i], j));
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
    auto elapsed = millis() - start;

    if (elapsed >= TIMEOUT_MS)
    {
        debug("Elapsed ( " + (to_string(elapsed)) + " > " + to_string(TIMEOUT_MS) + " ms) with finished depth " + to_string(curMaxDepth - 1));
        throw runtime_error("No more time");
    }
}

vector<int> reverse(vector<int> seeds) {
    vector<int> tab2, tab1;
    //Copier les éléments dans le tableau tab2 à partir de la fin du tableau tab1    
    for (int i = seeds.size() - 1, j = 0; i >= 0; i--, j++)
        tab2[j] = tab1[i];
    //Copie le tableau inversé dans l'original.
    for (int i = 0; i < 12; i++)
        tab1[i] = tab2[i];
    return tab1;
}

int sum(vector<int> seeds, int start, int end)
{
    int somme = 0;
    for (int i = start; i <= end; i++)
        somme += seeds[i];
    return somme;

}


int evaluate(vector<int> seeds) {
    int score = 0;
    for (int i = 0; i < 6; i++) {
        if (1 <= seeds[i] <= 2)
            score += 3;
        if (!seeds[i])
            score += 4;
        if (seeds[i] >= 12)
            score += 2;
    }
    return score;
} 

Action getActionScore(int id, vector<int> seeds)
{
    Action actionScoreResult;
    vector<int> newSeeds = reverse(seeds);
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
    while 6 <= j < 12 and newSeeds[j] in [2, 3]:
        captures += newSeeds[j];
        newSeeds[j] = 0;
        j += 1;
        consecutiveCaptures += 1;
        // holes_hit.append(j)
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
    if (!sum(newSeeds, 6, 11) {
        actionScoreResult.score = -1000;
        actionScoreResult.seeds = new_seeds;
        return actionScoreResult;
    }
    else if (!sum(seeds, 6, 11) && sum(newSeeds, 6, 11)) || (sum(seeds, 0, 5) && !sum(newSeeds, 0, 5))
        score += 100;
    score += evaluate(newSeeds) - evaluate(reverse(newSeeds));
    actionScoreResult.score = score;
    actionScoreResult.seeds = newSeeds;
    return actionScoreResult;
}


void buildGraph(Node node) {
    try {
        if (node.depth > MAX_DEPTH or loop_elapsed * 1000 > TIMEOUT_MS)
            return;
        vector<Action> actions;
        vector<int> evalResult;

        for (int i = 0; i < 6; i++)
            if (node.seeds[i]) {
                Action action = getActionScore(i, node.seeds);
                if (action.score >= -100)
                    actions.push_back(action);
            }
        if (!actions.size())
            return;
        for (int i = 0; i < actions.size(); i++) {
            Node childNode;
            childNode.seeds = reverse(node.seeds);
            childNode.lastActionId = i;
            childNode.depth = node.depth + 1;
            node.children[i] = childNode;
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
    // game loop
    while (1) {

        start = millis();
        vector<int> seeds;
        for (int i = 0; i < 12; i++) {
            int s;
            cin >> s; cin.ignore();
            seeds.push_back(s);
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


    max_value = 0
    min_value = -90
    best_node = None
    Node tree;
    tree.seeds = seeds;
    tree.lastActionId = 0;
    tree.score = 0;
    tree.depth = 0;
    tree.children = nullptr;
    build_graph(tree);
    // eprint(tree)


    vector<Node, int> actionsCandidates;
    int value;
    for (int i = 0; i < tree.children.size(); i++) {
        Node node = tree.children[i];
        //value = minimax(node, depth=MAX_DEPTH, maximizingPlayer=True)
        value = alphabeta(node, min_value);
        if (value >= max_value) {
            max_value = value;
            actions_candidates.push_back(node, max_value);
        }
    }
    if (!actionsCandidates)  // no IA solution given in time, select best move by default
        if (!empty(tree.children)) {
            best_node = max_score(tree.children);
            /* eprint("Min Max Failed - Action : {} - Score = {} - cpu_time: {:.2f} ms".format(best_node.last_action_id, best_node.score, elapsed_time)) */
            action = best_node.last_action_id;
        }
        else:
            action = random.choice([i for i, s in enumerate(seeds[6:]) if s])
    else:
        best_value = max(actions_candidates, key=lambda x:x[1])[1]
        actions = [node.last_action_id for node, value in actions_candidates if value == best_value]
        action = random.choice(actions)
    print(action)
    MAX_DEPTH = 4

    cout << action << endl;
    }
}