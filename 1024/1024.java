import java.util.*;
import java.io.*;
import java.math.*;
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
        return this.parent;
    }
}

class GameState {
    Board board;
    char action;
    int turnScore;
    int depth;
    long value;
    final static int SIZE = 4;
    private final String dirs = "URDL";

    public GameState(Board b, char action, int turnScore, int depth) {
        this.board = b;
        this.action = action;
        this.turnScore = turnScore;
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
    public final int SIZE = 4;
    long seed;
    public int[][] grid;
    int score;
    private int moves;
    private final String dirs = "URDL";

    public Board(long seed, int score, int[][] grid) {
        this.seed = seed;
        this.score = score;
        this.grid = grid;
    }

    int spawnTile() {
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
        return value;
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

    public char chooseBestAction() {
        int maxScore = 0, score;
        char bestDir = '\0';
        for (char c : dirs.toCharArray()) {
            score = getScore(c);
            if (score > maxScore) {
                maxScore = score;
                bestDir = c;
            }
        }
        return bestDir;
    }

    public int getScore(char action) {
        int dir = dirs.indexOf(action);
        if (!canMove(dir)) {
            return -1; // ignore remaining plans
        }
        score = applyMove(dir);
        return score;
    }
}


class Player {

    final static String dirs = "URDL";
    final static int SIZE = 4;
    static int MAX_DEPTH = 14;
    static int nodesCount = 0;
    static long maxScore = 0;
    static int roundCount = 0, actionCount = 0;
    static MyTreeNode<GameState> bestNode, node;
    static boolean codingameFlag = false;


    // version without simulation: get best action at each round without predicting new states
/*    public static char chooseBestAction(Board board) {
        int[][] backup = new int[SIZE][SIZE];
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) backup[x][y] = board.grid[x][y];
        }
        int maxScore = 0;
        int score = 0;
        char bestDir = '\0';
        for (char c : dirs.toCharArray()) {
            score = board.getScore(c);
            if (score >= maxScore) {
                maxScore = score;
                bestDir = c;
            }
            for (int x = 0; x < SIZE; x++) {
                for (int y = 0; y < SIZE; y++) {
                    board.grid[x][y] = backup[x][y];
                }
            }
        }
        return bestDir;
    }*/


    public static List<Map.Entry<Character, Integer>> chooseTwoBestAction(Board board) {
        int[][] backup = new int[SIZE][SIZE];
        //char[] actions = new char[2];
        //int[] scores = new int[4];
        List<Character> actions = new ArrayList<>();
        Map<Character, Integer> scores = new HashMap<>();

        int dir;
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) backup[x][y] = board.grid[x][y];
        }
        int maxScore = 0;
        int score = 0;
        char bestDir = '\0';
        for (char c : dirs.toCharArray()) {
            score = board.getScore(c);
            dir = dirs.indexOf(c);
            scores.put(c, score);
            for (int x = 0; x < SIZE; x++) {
                for (int y = 0; y < SIZE; y++) {
                    board.grid[x][y] = backup[x][y];
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

    // Create the tree of all possibles actions
    public static void buildTree(MyTreeNode node, GameState gs) {
        int turnScore;
        if (gs.depth > MAX_DEPTH)
            return;
        if (gs.board.score > maxScore && gs.depth == MAX_DEPTH) {
            bestNode = node;
            maxScore = gs.board.score;
        }
        for (Map.Entry<Character, Integer> action : chooseTwoBestAction(gs.board)) {
            if (action.getValue() > -1) {
                int[][] grid = new int[SIZE][SIZE];
                for (int x = 0; x < SIZE; x++) {
                    for (int y = 0; y < SIZE; y++) {
                        grid[x][y] = gs.board.grid[x][y];
                    }
                }
                Board board = new Board(gs.board.seed, gs.board.score, grid);
                turnScore = board.getScore(action.getKey());
                if (turnScore > -1) {
                    GameState newGs = new GameState(board, action.getKey(), turnScore, gs.depth);
                    int value = newGs.board.spawnTile();
                    newGs.board.score = gs.board.score + turnScore + value;
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
        System.err.println(seed);
        System.err.println(score);
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                System.err.print(grid[y][x] + " ");
            }
            System.err.println();
        }
    }
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
        int MAX_TIME = 1000;
        long start = 0, end, elapsedTime = 0;
        boolean isStart = true;
        int[][] newGrid = new int[4][4];
        int[][] grid = new int[4][4];
        int score, newScore = 0;
        long seed, newSeed = 0;
        String result = "";


        // game loop
        while (true) {
            roundCount++;
            start = System.currentTimeMillis();
            if (isStart || codingameFlag) {
                seed = in.nextInt(); // needed to predict the next spawns
                score = in.nextInt();
                int actionsCount = 0;
                int dir;
                result = "";

                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        int cell = in.nextInt();
                        grid[i][j] = cell;
                    }
                }
                debug(seed, score, grid);
                isStart = false;
            }
            else {
                seed = newSeed; // needed to predict the next spawns
                score = newScore;
                grid = newGrid;
                result = "";
            }
            Board b = new Board(seed, score, grid);
            GameState gs_start = new GameState(b, '\0', score, 0);
            MyTreeNode<GameState> root = new MyTreeNode<GameState>(gs_start);
            System.err.printf("Building tree! (depth = %d)\n", MAX_DEPTH);
            nodesCount = 0;
            buildTree(root, gs_start);
            end = System.currentTimeMillis();
            elapsedTime = end - start;
            System.err.printf("Build Tree finished! (nodes = %d) - Elapsed Time: %d ms\n", nodesCount, elapsedTime);

            node = bestNode;
            while (node.getParent() != null) {
                result += Character.toString(node.getData().action);
                node = node.getParent();
            }
 /*           char action = chooseBestAction(b);
            end = System.currentTimeMillis();
            elapsedTime = end - start;
            while (action != '\0' && elapsedTime < MAX_TIME) {
                result += Character.toString(action);
                actionsCount++;
                dir = dirs.indexOf(action);
                score = b.applyMove(dir);
                b.spawnTile();
                end = System.currentTimeMillis();
                elapsedTime = end - start;
                action = chooseBestAction(b);
            }*/
            System.out.println(result);
            end = System.currentTimeMillis();
            elapsedTime = end - start;
            newSeed = bestNode.getData().board.seed;
            newScore = bestNode.getData().board.score;
            newGrid = bestNode.getData().board.grid;
            debug(newSeed, newScore, newGrid);
            actionCount += result.length();
            System.err.printf("Action print finished! action score: %d - (nodes = %d) - Total score: %d - Round n° %d - Actions count: %d - Elapsed Time: %d ms\n", maxScore, nodesCount, calcScore(bestNode.getData().board.grid), roundCount, actionCount, elapsedTime);
            b = null;
            root = null;
            gs_start = null;
            bestNode = null;
            node = null;
            maxScore = 0;
            MAX_DEPTH = 11;
        }
    }
}