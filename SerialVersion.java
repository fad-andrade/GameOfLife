import java.util.Random;

public class SerialVersion{
    public static final int DIM = 2048;
    public static final int MAX_GEN = 2000;

    public static int getNeighbors(int[][] grid, int i, int j){
        int up = i - 1;
        if(up < 0) up = DIM - 1;
    
        int down = i + 1;
        if(down >= DIM) down = 0;
    
        int left = j - 1;
        if(left < 0) left = DIM - 1;
        
        int right = j + 1;
        if(right >= DIM) right = 0;
    
        int alive_neighbors = 
        grid[up][left] + grid[up][j] + grid[up][right] +
        grid[i][left] + grid[i][right] +
        grid[down][left] + grid[down][j] + grid[down][right];
    
        return alive_neighbors;    
    }

    public static void main(String[] args) {
        // Clock initialize
        long[] time = new long[2];
        time[0] = System.currentTimeMillis();

        // Declaring matrices
        int[][] grid = new int[DIM][DIM];
        int[][] newgrid = new int[DIM][DIM];

        // Randomly filling the grid
        Random generator = new Random(1985);
        for(int i = 0; i < DIM; i++){
            for(int j = 0; j < DIM; j++){
                grid[i][j] = generator.nextInt(2147483647) % 2;
            }
        }

        // Clock initialize
        long[] time2 = new long[2];
        time2[0] = System.currentTimeMillis();

        // Running 2K Generations
        int newgrid_alives = 0;
        for(int g = 1; g <= MAX_GEN; g++){
            // For every cell
            newgrid_alives = 0;
            for(int h = 0; h < DIM*DIM; h++){
                int i = h / DIM;
                int j = h % DIM;
                int alive_neighbors = getNeighbors(grid, i, j);

                // Implementing the game rules to the next generation
                // Underpopulation
                if(grid[i][j] == 1 && alive_neighbors < 2){
                    newgrid[i][j] = 0;
                // Still alive
                }else if(grid[i][j] == 1 && (alive_neighbors == 2 || alive_neighbors == 3)){
                    newgrid[i][j] = 1;
                    newgrid_alives++;
                // Overpopulation
                }else if(grid[i][j] == 1 && alive_neighbors >= 4){
                    newgrid[i][j] = 0;
                // Reproduction
                }else if(grid[i][j] == 0 && alive_neighbors == 3){
                    newgrid[i][j] = 1;
                    newgrid_alives++;
                }
            }

            // Copy newgrid to grid
            for(int h = 0; h < DIM*DIM; h++){
                int i = h / DIM;
                int j = h % DIM;
                grid[i][j] = newgrid[i][j];
            }    
        }

        // Clock finalize
        time2[1] = System.currentTimeMillis();

        // Printing results
        System.out.println(newgrid_alives + " alive cells");

        System.out.println((time2[1] - time2[0] + "ms to run the main loop"));

        // Clock finalize
        time[1] = System.currentTimeMillis();
        // Printing results
        System.out.println((time[1] - time[0] + "ms to run the entire code"));
    }
}