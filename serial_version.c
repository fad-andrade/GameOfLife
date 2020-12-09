#include<sys/time.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#define DIM 2048
#define MAX_GEN 2000
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
    struct timeval start2, end2;
    gettimeofday(&start2, NULL);
    
    // Running 2K Generations
    int newgrid_alives;
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
    gettimeofday(&end2, NULL);

    // Printing results
    printf("%d alive cells\n", newgrid_alives);
    
    printf("%ldms to run the main loop\n", (1000 * (end2.tv_sec - start2.tv_sec) + (end2.tv_usec - start2.tv_usec) / 1000));

    // Free dynamic matrices
    for(int i = 0; i < DIM; i++){
        free(grid[i]);
        free(newgrid[i]);
    }
    free(grid);
    free(newgrid);

    // Clock finalize
    gettimeofday(&end, NULL);
    // Printing results
    printf("%ldms to run the entire code\n", (1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));

    return 0;
}
