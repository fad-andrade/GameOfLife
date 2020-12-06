#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<omp.h>

#define DIM 2048
#define MAX_GEN 2000
#define MAX_THREADS 4
#define SRAND_VALUE 1985

int getNeighbors(int** grid, int i, int j){
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

int main(){
    // Clock initialize
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Declaring matrices
    int **grid = (int**) malloc(DIM*sizeof(int*));
    for(int i = 0; i < DIM; i++){
        grid[i] = (int*) malloc(DIM * sizeof(int));
    }
    int **newgrid = (int**) malloc(DIM*sizeof(int*));
    for(int i = 0; i < DIM; i++){
        newgrid[i] = (int*) malloc(DIM * sizeof(int));
    }

    // Randomly filling the grid
    srand(SRAND_VALUE);
    for(int i = 0; i < DIM; i++){
        for(int j = 0; j < DIM; j++){
            grid[i][j] = rand() % 2;
        }
    }

    // Clock initialize
    double time2[2];
    time2[0] = omp_get_wtime();
    
    // Running 2K genarations
    int newgrid_alives;
    int alive_neighbors;
    int g, h, i, j;
    for(g = 1; g <= MAX_GEN; g++){
        newgrid_alives = 0;
        
		// Parallelizing the code
        omp_set_num_threads(MAX_THREADS);
        #pragma omp parallel shared(grid, newgrid)
        {
        	// Distribute loop iterations
        	#pragma omp for private(h, i, j, alive_neighbors) \
        			reduction(+:newgrid_alives)
        	// For every cell
		    for(h = 0; h < DIM*DIM; h++){
		        i = h / DIM;
		        j = h % DIM;
		        alive_neighbors = getNeighbors(grid, i, j);

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

	        // Distribute loop iterations
		    #pragma omp for private(h, i, j)
	        // Copy newgrid to grid
		    for(h = 0; h < DIM*DIM; h++){
		        i = h / DIM;
		        j = h % DIM;
		        grid[i][j] = newgrid[i][j];
		    }
        }
    }

    // Clock finalize
    time2[1] = omp_get_wtime();

    printf("%d alive cells\n", newgrid_alives);
    
    printf("%fms to run the main loop\n", (time2[1] - time2[0]) * 1000);

    // Clock finalize
    gettimeofday(&end, NULL);
    // Printing results
    printf("%ldms to run the entire code\n", (1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));

    free(grid);
    free(newgrid);
    return 0;
}
