#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DIM 2048
#define MAX_GEN 2000
#define SRAND_VALUE 1985

int getNeighbors(int** grid, int i, int j){
    int up = i - 1;

    int down = i + 1;

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

int main(int argc, char * argv[]) {
    // MPI begin
    MPI_Init(&argc, &argv);
    int pid, np;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // Declaring grid matrix
    int **grid = (int**) malloc(DIM * sizeof(int*));
    for(int i = 0; i < DIM; i++){
        grid[i] = (int*) malloc(DIM * sizeof(int));
    }

    // Randomly filling the grid
    srand(SRAND_VALUE);
    for(int i = 0; i < DIM; i++){
        for(int j = 0; j < DIM; j++){
            grid[i][j] = rand() % 2;
        }
    }

    //Find p_grid size based on pid
    int size = floor(DIM/np) + 2;
    if(pid == np - 1){
        size += DIM - floor(DIM/np)*np;
    }
    
    // Malloc p_grid
    int **p_grid = (int**) malloc(size * sizeof(int*));
    for(int i = 0; i < size; i++){
        p_grid[i] = (int*) malloc(DIM * sizeof(int));
    }
    
    // Find initial line at grid based on pid
    int k = floor(DIM/np)*pid;

    // Fill p_grid with grid values
    for(int i = 1; i < size - 1; i++){
        for(int j = 0; j < DIM; j++){
            p_grid[i][j] = grid[k][j];
        }
        k++;
    }

    // Declaring p_newgrid
    int **p_newgrid = (int**) calloc(size - 2, sizeof(int*));
    for(int j = 0; j < size - 2; j++){
        p_newgrid[j] = (int*) calloc(DIM, sizeof(int));
    }

    // Running 2K genarations
    int newgrid_alives, i;
    for(i = 1; i <= MAX_GEN; i++){
        if(np == 1){
            MPI_Sendrecv(&p_grid[1][0],        DIM, MPI_INTEGER,  0, 1,
                         &p_grid[size - 1][0], DIM, MPI_INTEGER, 0, 1,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            MPI_Sendrecv(&p_grid[size - 2][0], DIM, MPI_INTEGER, 0, 2,
                         &p_grid[0][0],        DIM, MPI_INTEGER,  0, 2,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else{
            if(pid == 0){
                MPI_Sendrecv(&p_grid[1][0],       DIM, MPI_INTEGER,  np - 1, pid,
                            &p_grid[size - 1][0], DIM, MPI_INTEGER, pid + 1, pid + 1,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Sendrecv(&p_grid[size - 2][0], DIM, MPI_INTEGER, pid + 1, 8 + pid,
                            &p_grid[0][0],         DIM, MPI_INTEGER,  np - 1, 8 + np - 1,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if(pid == np - 1){
                MPI_Sendrecv(&p_grid[1][0],       DIM, MPI_INTEGER, pid - 1, pid,
                            &p_grid[size - 1][0], DIM, MPI_INTEGER,       0, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Sendrecv(&p_grid[size - 2][0], DIM, MPI_INTEGER,       0, 8 + pid,
                            &p_grid[0][0],         DIM, MPI_INTEGER, pid - 1, 8 + pid - 1,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else{
                MPI_Sendrecv(&p_grid[1][0],       DIM, MPI_INTEGER, pid - 1, pid,
                            &p_grid[size - 1][0], DIM, MPI_INTEGER, pid + 1, pid + 1,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Sendrecv(&p_grid[size - 2][0], DIM, MPI_INTEGER, pid + 1, 8 + pid,
                            &p_grid[0][0],         DIM, MPI_INTEGER, pid - 1, 8 + pid - 1,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

        newgrid_alives = 0;
        // Calculate newgrid for every cell
        for(int j = 1; j < size - 1; j++){
            for(int k = 0; k < DIM; k++){
                int alive_neighbors = getNeighbors(p_grid, j, k);

                // Implementing the game rules to the next generation
		        // Underpopulation
		        if(p_grid[j][k] == 1 && alive_neighbors < 2){
		            p_newgrid[j - 1][k] = 0;
		        // Still alive
		        }else if(p_grid[j][k] == 1 && (alive_neighbors == 2 || alive_neighbors == 3)){
		            p_newgrid[j - 1][k] = 1;
		            newgrid_alives++;
		        // Overpopulation
		        }else if(p_grid[j][k] == 1 && alive_neighbors >= 4){
		            p_newgrid[j - 1][k] = 0;
		        // Reproduction
		        }else if(p_grid[j][k] == 0 && alive_neighbors == 3){
		            p_newgrid[j - 1][k] = 1;
		            newgrid_alives++;
		        }
            }
        }

        // Copy p_newgrid to p_grid
        for(int j = 1; j < size - 1; j++){
            for(int k = 0; k < DIM; k++){
                p_grid[j][k] = p_newgrid[j - 1][k];
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }
    
    int total_alives;
    MPI_Reduce(&newgrid_alives, &total_alives, 1, MPI_INTEGER, MPI_SUM, 0, MPI_COMM_WORLD );
    if(pid == 0){       
        printf("Alive cells %d - Gen %d\n", total_alives, i - 1);
    }
    
    MPI_Finalize();

    for(int i = 0; i < DIM; i++) free(grid[i]);
    free(grid);
    for(int i = 0; i < size; i++) free(p_grid[i]);
    free(p_grid);
    for(int i = 0; i < size - 2; i++) free(p_newgrid[i]);
    free(p_newgrid);

    return 0;
}
