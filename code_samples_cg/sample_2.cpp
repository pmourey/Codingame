    static long getWinner(State state) {
        long resultGrid0 = (state.data1 & Constants.MASK_GRID0) >> 54;
        long resultGrid1 = (state.data1 & Constants.MASK_GRID1) >> 56;
        long resultGrid2 = (state.data1 & Constants.MASK_GRID2) >> 58;
        long resultGrid3 = (state.data2 & Constants.MASK_GRID0) >> 54;
        long resultGrid4 = (state.data2 & Constants.MASK_GRID1) >> 56;
        long resultGrid5 = (state.data2 & Constants.MASK_GRID2) >> 58;
        long resultGrid6 = (state.data3 & Constants.MASK_GRID0) >> 54;
        long resultGrid7 = (state.data3 & Constants.MASK_GRID1) >> 56;
        long resultGrid8 = (state.data3 & Constants.MASK_GRID2) >> 58;

        if (resultGrid0 == resultGrid1 && resultGrid1 == resultGrid2 && (resultGrid0 == 1 || resultGrid0 == 2)) return resultGrid0;
        if (resultGrid3 == resultGrid4 && resultGrid4 == resultGrid5 && (resultGrid3 == 1 || resultGrid3 == 2)) return resultGrid3;
        if (resultGrid6 == resultGrid7 && resultGrid7 == resultGrid8 && (resultGrid6 == 1 || resultGrid6 == 2)) return resultGrid6;

        if (resultGrid0 == resultGrid3 && resultGrid3 == resultGrid6 && (resultGrid0 == 1 || resultGrid0 == 2)) return resultGrid0;
        if (resultGrid1 == resultGrid4 && resultGrid4 == resultGrid7 && (resultGrid1 == 1 || resultGrid1 == 2)) return resultGrid1;
        if (resultGrid2 == resultGrid5 && resultGrid5 == resultGrid8 && (resultGrid2 == 1 || resultGrid2 == 2)) return resultGrid2;

        if (resultGrid0 == resultGrid4 && resultGrid4 == resultGrid8 && (resultGrid0 == 1 || resultGrid0 == 2)) return resultGrid0;
        if (resultGrid2 == resultGrid4 && resultGrid4 == resultGrid6 && (resultGrid2 == 1 || resultGrid2 == 2)) return resultGrid2;

        int score1 = 0;
        int score2 = 0;
        boolean isFilled = true;
        if (resultGrid0 == 1) score1++;
        else if (resultGrid0 == 2) score2++;
        else if (resultGrid0 == 0) isFilled = false;
        if (resultGrid1 == 1) score1++;
        else if (resultGrid1 == 2) score2++;
        else if (resultGrid1 == 0) isFilled = false;
        if (resultGrid2 == 1) score1++;
        else if (resultGrid2 == 2) score2++;
        else if (resultGrid2 == 0) isFilled = false;
        if (resultGrid3 == 1) score1++;
        else if (resultGrid3 == 2) score2++;
        else if (resultGrid3 == 0) isFilled = false;
        if (resultGrid4 == 1) score1++;
        else if (resultGrid4 == 2) score2++;
        else if (resultGrid4 == 0) isFilled = false;
        if (resultGrid5 == 1) score1++;
        else if (resultGrid5 == 2) score2++;
        else if (resultGrid5 == 0) isFilled = false;
        if (resultGrid6 == 1) score1++;
        else if (resultGrid6 == 2) score2++;
        else if (resultGrid6 == 0) isFilled = false;
        if (resultGrid7 == 1) score1++;
        else if (resultGrid7 == 2) score2++;
        else if (resultGrid7 == 0) isFilled = false;
        if (resultGrid8 == 1) score1++;
        else if (resultGrid8 == 2) score2++;
        else if (resultGrid8 == 0) isFilled = false;
        if (!isFilled) return 0;
        return score1 > score2 ? 1 : score2 > score1 ? 2 : 3;
    }

for (int i = gridStartX; i < gridEndX; i++) {
    for (int j = gridStartY; j < gridEndY; j++) {
        if ((data & Constants.MASKS[i][j]) == 0) legalPlays.add(Constants.GRID[i][j]);
    }
}