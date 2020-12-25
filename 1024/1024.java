import java.util.*;
import java.io.*;
import java.math.*;
import java.util.ArrayList;

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

    //private final static String dirs = "URDL";
    final static String dirs = "URDL";
    final static int SIZE = 4;
    final static int ACTIONS_PER_TURN = 9;
    final static int MAX_TIME = 50;

    public static char chooseBestAction(Board board) {
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
    }

    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);
        int MAX_TIME = 1000;
        long start, end, elapsedTime = 0;

        // game loop
        while (true) {
            long seed = in.nextInt(); // needed to predict the next spawns
            int score = in.nextInt();
            int[][] grid = new int[4][4];
            int actionsCount = 0;
            int dir;
            String result = "";

            start = System.currentTimeMillis();

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    int cell = in.nextInt();
                    grid[j][i] = cell;
                    //System.err.println(cell);
                }
            }
            Board b = new Board(seed, score, grid);
            char action = chooseBestAction(b);
/*            dir = dirs.indexOf(action);
            score = b.applyMove(dir);
            //System.err.println("New board: \n" + Arrays.deepToString(grid));
            b.spawnTile();
            System.err.printf("Action: %d - Dir: %s - score: %d - Elapsed Time: %d ms\n", actionsCount, dir, score, elapsedTime);
            result = Character.toString(action);
            actionsCount++;
            */
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
            }
            System.err.printf("Actions count: %d - Elapsed Time: %d ms\n", actionsCount, elapsedTime);
            System.out.println(result);
            b = null;
            MAX_TIME = 50;
        }
    }
}