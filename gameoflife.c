#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define WIDTH 50
#define HEIGHT 30
#define NUM_THREADS 4

int grid[HEIGHT][WIDTH];

void initialize_grid() {
    // Initialize the grid with random live and dead cells
    srand(time(NULL));
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = rand() % 2;
        }
    }
}

void print_grid() {
    // Print out the grid to the console
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (grid[i][j] == 0) {
                printf(".");
            } else {
                printf("X");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void update_grid(int start_row, int end_row) {
    // Update the sub-grid based on the Game of Life rules
    int new_grid[HEIGHT][WIDTH];
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int live_neighbors = 0;
            for (int y = i-1; y <= i+1; y++) {
                for (int x = j-1; x <= j+1; x++) {
                    if (y == i && x == j) {
                        continue; // ignore the current cell
                    }
                    if (y < 0 || y >= HEIGHT || x < 0 || x >= WIDTH) {
                        continue; // ignore cells outside the grid
                    }
                    live_neighbors += grid[y][x];
                }
            }
            if (grid[i][j] == 1) {
                if (live_neighbors < 2 || live_neighbors > 3) {
                    new_grid[i][j] = 0; // cell dies
                } else {
                    new_grid[i][j] = 1; // cell lives on
                }
            } else {
                if (live_neighbors == 3) {
                    new_grid[i][j] = 1; // cell is born
                } else {
                    new_grid[i][j] = 0; // cell remains dead
                }
            }
        }
    }
    // Copy the new sub-grid back to the original grid
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }
}

int main() {
    // Initialize the grid and run the game loop
    initialize_grid();
    omp_set_num_threads(NUM_THREADS);
    while (1) {
        print_grid();
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            int start_row = tid * (HEIGHT / NUM_THREADS);
            int end_row = (tid + 1) * (HEIGHT / NUM_THREADS);
            update_grid(start_row, end_row);
        }
        // Wait for some time to see the animation
        struct timespec delay;
        delay.tv_sec = 0;
        delay.tv_nsec = 100000000; // 100 milliseconds in nanoseconds
        nanosleep(&delay, NULL);
    }
    return 0;
}
