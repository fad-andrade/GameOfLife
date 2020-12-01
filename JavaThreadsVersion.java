import java.util.Random;

class Global{
    public static final int DIM = 2048;
    public static final int MAX_GEN = 2000;
    public static final int MAX_THREADS = 4;

    // Global matrices
    public static int[][] grid = new int[DIM][DIM];
    public static int[][] newgrid = new int[DIM][DIM];
    // Global alive cells for each thread
    public static int[] newgrid_alives = new int[MAX_THREADS];
}

class Threads extends Thread{
    private String type;
    private int thd;

    public Threads(String type, int thd){
        this.type = type;
        this.thd = thd;
    }

    private int getNeighbors(int[][] grid, int i, int j){
        int up = i - 1;
        if(up < 0) up = Global.DIM - 1;
    
        int down = i + 1;
        if(down >= Global.DIM) down = 0;
    
        int left = j - 1;
        if(left < 0) left = Global.DIM - 1;
        
        int right = j + 1;
        if(right >= Global.DIM) right = 0;
    
        int alive_neighbors = 
        grid[up][left] + grid[up][j] + grid[up][right] +
        grid[i][left] + grid[i][right] +
        grid[down][left] + grid[down][j] + grid[down][right];
    
        return alive_neighbors;    
    }

    private void setNewGrid(int th){
        int thd;
        thd = th;
    
        // For every cell
        for(int h = thd; h < Global.DIM*Global.DIM; h += Global.MAX_THREADS){
            int i = h / Global.DIM;
            int j = h % Global.DIM;
            int alive_neighbors = getNeighbors(Global.grid, i, j);
            
            // Implementing the game rules to the next generation
            // Underpopulation
            if(Global.grid[i][j] == 1 && alive_neighbors < 2){
                Global.newgrid[i][j] = 0;
            // Still alive
            }else if(Global.grid[i][j] == 1 && (alive_neighbors == 2 || alive_neighbors == 3)){
                Global.newgrid[i][j] = 1;
                Global.newgrid_alives[thd]++;
            // Overpopulation
            }else if(Global.grid[i][j] == 1 && alive_neighbors >= 4){
                Global.newgrid[i][j] = 0;
            // Reproduction
            }else if(Global.grid[i][j] == 0 && alive_neighbors == 3){
                Global.newgrid[i][j] = 1;
                Global.newgrid_alives[thd]++;
            }
        }
    }

    private void copyNewgridToGrid(int th){
        int thd;
        thd = th;
    
        // For every cell
        for(int h = thd; h < Global.DIM*Global.DIM; h += Global.MAX_THREADS){
            int i = h / Global.DIM;
            int j = h % Global.DIM;
            Global.grid[i][j] = Global.newgrid[i][j];
        }
    }

    public void run(){
        if(type.equals("set"))
            setNewGrid(thd);
        else if(type.equals("copy"))
            copyNewgridToGrid(thd);
    }
}

public class JavaThreadsVersion{
    public static void main(String[] args){
        // public static int[] newgrid_alives = new int[MAX_THREADS];
        Threads threads[] = new Threads[Global.MAX_THREADS];

        // Randomly filling the grid
        Random generator = new Random(1985);
        for(int i = 0; i < Global.DIM; i++){
            for(int j = 0; j < Global.DIM; j++){
                Global.grid[i][j] = generator.nextInt(2147483647) % 2;
            }
        }

        // Clock initialize
        long[] time = new long[2];
        time[0] = System.currentTimeMillis();
        
        int newgrid_alives_local = 0;
        //Running 2K generations
        for(int g = 1; g <= Global.MAX_GEN; g++){

            // Setting newgrid
            newgrid_alives_local = 0;
            // Create threads
            for(int th = 0; th < Global.MAX_THREADS; th++){
                threads[th] = new Threads("set", th);
                threads[th].start();
            }
            // Join threads
            for(int th = 0; th < Global.MAX_THREADS; th++){
                try{
                    threads[th].join();
                }catch(InterruptedException e){
                    e.printStackTrace();
                }
                newgrid_alives_local += Global.newgrid_alives[th];
                Global.newgrid_alives[th] = 0;
            }

            // Copying newgrid to grid
            // Create threads
            for(int th = 0; th < Global.MAX_THREADS; th++){
                threads[th] = new Threads("copy", th);
                threads[th].start();
            }
            for(int th = 0; th < Global.MAX_THREADS; th++){
                try{
                    threads[th].join();
                }catch(InterruptedException e){
                    e.printStackTrace();
                }
            }       
        }

        // Clock finalize
        time[1] = System.currentTimeMillis();
        
        // Printing results
        System.out.println(newgrid_alives_local + " alive cells");

        System.out.println((time[1] - time[0] + "ms to run"));
    }
}
