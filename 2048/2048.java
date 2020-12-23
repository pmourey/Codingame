import java.util.*;
import java.io.*;
import java.math.*;
import java.util.ArrayList;

class Board {
    public final int SIZE = 4;
    long seed;
    public int[][] grid = new int[SIZE][SIZE];
    int score;
    private int moves;

    public Board(long seed, int score, int[][] grid) {
        this.seed = seed;
        this.score = 0;
        this.grid = grid;
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

    private int applyMove(int dir) {
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

    private final String dirs = "URDL";

    public char chooseBestAction() {
        int maxScore = 0, score;
        char bestDir = '\0';
        for (char c : dirs.toCharArray()) {
            score = getScore(c);
            System.err.println("Direction: " + c + "- Score: " + score);
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

    public ArrayList<String> getInput() {
        ArrayList<String> result = new ArrayList<>();
        result.add(String.valueOf(seed));
        result.add(String.valueOf(score));
        for (int y = 0; y < SIZE; y++) {
            String line = "";
            for (int x = 0; x < SIZE; x++) line += grid[x][y] + " ";
            result.add(line.trim());
        }
        return result;
    }
}


class Player {

    private final static String dirs = "URDL";

    public static char chooseBestAction(Board board) {
        Board b = new Board(board.seed, board.score, board.grid);
        int maxScore = 0, score;
        char bestDir = '\0';
        for (char c : dirs.toCharArray()) {
            score = b.getScore(c);
            System.err.println("Direction: " + c + " - Score: " + score);
            if (score >= maxScore) {
                maxScore = score;
                bestDir = c;
            }
        }
        b = null;
        return bestDir;
    }

    public static void main(String args[]) {
        Scanner in = new Scanner(System.in);

        // game loop
        while (true) {
            long seed = in.nextInt(); // needed to predict the next spawns
            int score = in.nextInt();
            int[][] grid = new int[4][4];

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    int cell = in.nextInt();
                    grid[j][i] = cell;
                    //System.err.println(cell);
                }
            }
            Board b = new Board(seed, score, grid);
            System.err.println(Arrays.deepToString(b.grid));
            System.out.println(chooseBestAction(b));
            b = null;
        }
    }
}