import java.util.*;
import java.util.ArrayList;

class MyTreeNode<T> {
    private T data = null;
    private List<MyTreeNode> children = new ArrayList<>();
    private MyTreeNode parent = null;

    public MyTreeNode(T data) {
        this.data = data;
    }

    public void addChild(MyTreeNode child) {
        child.setParent(this);
        this.children.add(child);
    }

    public void addChild(T data) {
        MyTreeNode<T> newChild = new MyTreeNode<>(data);
        this.addChild(newChild);
    }

    public void addChildren(List<MyTreeNode> children) {
        for (MyTreeNode t : children) {
            t.setParent(this);
        }
        this.children.addAll(children);
    }

    public List<MyTreeNode> getChildren() {
        return children;
    }

    public T getData() {
        return data;
    }

    public void setData(T data) {
        this.data = data;
    }

    private void setParent(MyTreeNode parent) {
        this.parent = parent;
    }

    public MyTreeNode getParent() {
        return parent;
    }
}

class GameState {
    Board board;
    char action;
    int depth;
    long value;
    final static int SIZE = 4;
    private final String dirs = "URDL";

    public GameState(Board b, char action, int depth) {
        this.board = b;
        this.action = action;
        this.depth = depth;
    }

    public int getScore(char c) {
        int turnScore;
        int[][] backup = new int[SIZE][SIZE];
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) backup[x][y] = board.grid[x][y];
        }
        turnScore = board.getScore(c);
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                board.grid[x][y] = backup[x][y];
            }
        }
        return turnScore;
    }

}

class Board {
    private final int SIZE = 4;
    long seed;
    int[][] grid;
    int score;
    private int moves;
    private final String dirs = "URDL";

    public Board(long seed, int score, int[][] grid) {
        this.seed = seed;
        this.score = score;
        this.grid = grid;
    }

    void spawnTile() {
        ArrayList<Integer> freeCells = new ArrayList<>();
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                if (grid[x][y] == 0) freeCells.add(x + y * SIZE);
            }
        }

        int spawnIndex = freeCells.get((int) seed % freeCells.size());
        int value = (seed & 0x10) == 0 ? 2 : 4;
        //System.err.println("New Tile: " + spawnIndex + "/" + value);

        grid[spawnIndex % SIZE][spawnIndex / SIZE] = value;

        seed = seed * seed % 50515093L;
        //return value;
    }

    public int getScore() {
        return score;
    }

    public int getMoves() {
        return moves;
    }

    public boolean canMove() {
        for (int i = 0; i < 4; i++) {
            if (canMove(i)) return true;
        }
        return false;
    }

    private boolean canMove(int dir) {
        int[][] backup = new int[SIZE][SIZE];
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) backup[x][y] = grid[x][y];
        }
        applyMove(dir);
        boolean changed = false;
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                changed |= grid[x][y] != backup[x][y];
                grid[x][y] = backup[x][y];
            }
        }
        return changed;
    }

    int applyMove(int dir) {
        int turnScore = 0;
        boolean[][] merged = new boolean[SIZE][SIZE];
        int targetStart = new int[]{0, SIZE - 1, SIZE * (SIZE - 1), 0}[dir];
        int targetStep = new int[]{1, SIZE, 1, SIZE}[dir];
        int sourceStep = new int[]{SIZE, -1, -SIZE, 1}[dir];
        for (int i = 0; i < SIZE; i++) {
            int finalTarget = targetStart + i * targetStep;
            for (int j = 1; j < SIZE; j++) {
                int source = finalTarget + j * sourceStep;
                int sourceX = source % SIZE;
                int sourceY = source / SIZE;
                if (grid[sourceX][sourceY] == 0) continue;
                for (int k = j - 1; k >= 0; k--) {
                    int intermediate = finalTarget + k * sourceStep;
                    int intermediateX = intermediate % SIZE;
                    int intermediateY = intermediate / SIZE;
                    if (grid[intermediateX][intermediateY] == 0) {
                        grid[intermediateX][intermediateY] = grid[sourceX][sourceY];
                        grid[sourceX][sourceY] = 0;
                        source = intermediate;
                        sourceX = source % SIZE;
                        sourceY = source / SIZE;
                    } else {
                        if (!merged[intermediateX][intermediateY] && grid[intermediateX][intermediateY] == grid[sourceX][sourceY]) {
                            //System.err.println("Direction = " + dirs.charAt(dir)+ " -> (" + intermediateX + "," + intermediateY + ") - Score = " + grid[intermediateX][intermediateY]*2 );
                            grid[sourceX][sourceY] = 0;
                            grid[intermediateX][intermediateY] *= 2;
                            merged[intermediateX][intermediateY] = true;
                            turnScore += grid[intermediateX][intermediateY];
                        }
                        break;
                    }
                }
            }
        }
        return turnScore;
    }

    // version without simulation: get best action at each round without predicting new states
    char chooseBestAction() {
        int[][] backup = new int[SIZE][SIZE];
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) backup[x][y] = this.grid[x][y];
        }
        int maxScore = 0;
        int score = 0;
        char bestDir = '\0';
        for (char c : dirs.toCharArray()) {
            score = this.getScore(c);
            if (score >= maxScore) {
                maxScore = score;
                bestDir = c;
            }
            for (int x = 0; x < SIZE; x++) {
                for (int y = 0; y < SIZE; y++) {
                    this.grid[x][y] = backup[x][y];
                }
            }
        }
        return bestDir;
    }

    public List<Map.Entry<Character, Integer>> chooseTwoBestAction() {
        int[][] backup = new int[SIZE][SIZE];
        Map<Character, Integer> scores = new HashMap<>();

        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) backup[x][y] = this.grid[x][y];
        }
        int score = 0;
        for (char c : dirs.toCharArray()) {
            score = this.getScore(c);
            scores.put(c, score);
            for (int x = 0; x < SIZE; x++) {
                for (int y = 0; y < SIZE; y++) {
                    this.grid[x][y] = backup[x][y];
                }
            }
        }
        List<Map.Entry<Character, Integer>> result = new ArrayList<Map.Entry<Character, Integer>>();
        List<Map.Entry<Character, Integer>> entries = new ArrayList<Map.Entry<Character, Integer>>(scores.entrySet());
        Collections.sort(entries, new Comparator<Map.Entry<Character, Integer>>() {
            public int compare(Map.Entry<Character, Integer> e1, Map.Entry<Character, Integer> e2) {
                return e1.getValue().compareTo(e2.getValue());
            }
        });
        result.add(entries.get(2));
        result.add(entries.get(3));
        return result;
    }


    public int getScore(char action) {
        int dir = dirs.indexOf(action);
        if (!canMove(dir)) {
            return -1; // ignore remaining plans
        }
        int score = applyMove(dir);
        return score;
    }
}


class Player {

    final static String dirs = "URDL";
    final static int SIZE = 4;
    static int MAX_DEPTH_START = 20;    // 14 in codingame
    static int MAX_DEPTH = 16;  // 10 for codingame
    static int maxDepth = MAX_DEPTH_START;
    static long maxScore = 0;
    static int roundCount = 0, actionCount = 0;
    static int nodesCount = 0;
    static MyTreeNode<GameState> destNode, currNode, destNodeParent;
    static boolean codingameFlag = false; // set flag to false to run it locally...


    // Create a tree of possible best actions (limited to 2 out of 4 for performance reason)
    // TODO: improve the heuristic function (?)
    // TODO: improve algorithm to use beam search
    public static void buildTree(MyTreeNode node, GameState gs) {
        int turnScore;
        if (gs.depth > maxDepth)
            return;
        if (gs.board.score > maxScore && gs.depth == maxDepth) {
            destNode = node;
            destNodeParent = destNode.getParent();
            maxScore = gs.board.score;
        }
        for (Map.Entry<Character, Integer> action : gs.board.chooseTwoBestAction()) {
            if (action.getValue() > -1) {
                int[][] grid = new int[SIZE][SIZE];
                for (int x = 0; x < SIZE; x++) {
                    for (int y = 0; y < SIZE; y++) {
                        grid[x][y] = gs.board.grid[x][y];
                    }
                }
                Board newBoard = new Board(gs.board.seed, gs.board.score, grid);
                turnScore = newBoard.getScore(action.getKey());
                newBoard.score += turnScore;
                if (turnScore > -1) {
                    GameState newGs = new GameState(newBoard, action.getKey(), gs.depth);
                    newGs.board.spawnTile();
                    newGs.depth += 1;
                    MyTreeNode<GameState> childNode = new MyTreeNode<GameState>(newGs);
                    nodesCount++;
                    node.addChild(childNode);
                    buildTree(childNode, newGs);
                }
            }
        }
    }

    public static void debug(long seed, int score, int[][] grid) {
        ArrayList<String> result = new ArrayList<>();
        result.add(String.valueOf(seed));
        result.add(String.valueOf(score));
        for (int x = 0; x < SIZE; x++) {
            String line = "";
            for (int y = 0; y < SIZE; y++) line += grid[x][y] + " ";
            result.add(line.trim());
        }
        System.err.println(result);
    }

    // debugging function used to verify score calculations performed in simulation
    public static int calcScore(int[][] grid) {
        int score = 0;
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                score += grid[x][y];
            }
        }
        return score;
    }

    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);
        long start = 0, end, elapsedTime = 0;
        boolean isStart = true;
        int[][] newGrid = new int[4][4];
        int[][] grid = new int[4][4];
        int score, newScore = 0;
        long seed, newSeed = 0;
        ArrayList<Character> actionsResult = new ArrayList<>();

        // game loop
        while (true) {
            start = System.currentTimeMillis();
            roundCount++;
            actionsResult.clear();
            if (isStart || codingameFlag) {
                seed = in.nextInt(); // needed to predict the next spawns
                score = in.nextInt();

                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        int cell = in.nextInt();
                        grid[j][i] = cell;
                    }
                }
                isStart = false;
            } else {
                seed = newSeed; // needed to predict the next spawns
                score = newScore;
                grid = newGrid;
            }
            debug(seed, score, grid);
            Board b = new Board(seed, score, grid);
            GameState gs_start = new GameState(b, '\0', 0);
            MyTreeNode<GameState> root = new MyTreeNode<GameState>(gs_start);
            nodesCount = 0;
            buildTree(root, gs_start);
            if (destNode == null) {
                System.out.println(b.chooseBestAction());
                if (!codingameFlag)
                    break;
            }
            else {
                currNode = destNode;
                while (currNode.getParent() != null) {
                    actionsResult.add(currNode.getData().action);
                    currNode = currNode.getParent();
                }
                ArrayList<Character> actionsResultReverse = new ArrayList<>();
                for (int i = actionsResult.size() - 1; i >= 0; i--)
                    actionsResultReverse.add(actionsResult.get(i));
                actionsResult = actionsResultReverse;
                // Print the actions to console
                // System.err.println(actionsResult.toString());
                // System.err.println(actionsResult.toString().replaceAll("[,\\s\\[\\]]", ""));
                if (codingameFlag)
                    System.out.println(actionsResult.toString().replaceAll("[,\\s\\[\\]]", ""));
                else
                    System.err.println(actionsResult);
                maxDepth = MAX_DEPTH;
                actionCount += actionsResult.size();
                newSeed = destNode.getData().board.seed;
                newScore = destNode.getData().board.score;
                newGrid = destNode.getData().board.grid;
                debug(newSeed, newScore, newGrid);
                end = System.currentTimeMillis();
                elapsedTime = end - start;
                System.err.printf("Action print finished! (nodes = %d) - Total score: %d - Round n° %d - Actions count: %d - Elapsed Time: %d ms\n", nodesCount, newScore, roundCount, actionCount, elapsedTime);
                // Futile attempt to tell java garbage collector to delete my references :-DDD
                maxScore = 0;
                b = null;
                gs_start = null;
                root = null;
                destNode = null;
                currNode = null;
            }
        }
    }
}
