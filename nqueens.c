#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define BOARD_SIZE 100 // Maximum board size 

int queens[BOARD_SIZE], board_size, top = -1;
int solution_count = 0;

bool is_valid(int row, int column) {      // Check if (row, column) is a valid position for a queen
    int i;
    for (i = 0; i < row; i++) {
        if (queens[i] == column || abs(queens[i] - column) == abs(i - row)) { // Check if there is a queen in the same column or diagonal
            return false;
        }
    }
    return true;
}

void print_solution(int board_size) {   // Print the solution
    int i, j;
    #pragma omp critical
    {
        for (i = 0; i < board_size; i++) {
            for (j = 0; j < board_size; j++) {
                if (queens[i] == j) {
                    printf("Q ");
                } else {
                    printf("_ ");
                }
            }
            printf("\n");
        }
        printf("\n");
    }
}

bool solve_queens_util(int row, int board_size) { // Solve the N-Queens problem
    if (row == board_size) {
        #pragma omp atomic
        solution_count++;
        return false;
    }
    int column;
    bool found_solution = false;
    #pragma omp parallel for private(column) shared(found_solution)    // Parallelizing this loop 
    for (column = 0; column < board_size; column++) {
        if (is_valid(row, column)) {
            queens[++top] = column;
            if (solve_queens_util(row + 1, board_size)) {  //solve for remaining queens
                found_solution = true;
            }
            top--;
        }
    }
    return found_solution;
}

void solve_queens(int board_size, int num_threads) {  
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);  // Start time
    omp_set_num_threads(num_threads);
    #pragma omp parallel
    {
        #pragma omp single
        solve_queens_util(0, board_size);
    }
    gettimeofday(&t2, NULL); // End time
    printf("\nTotal solutions found: %d\n", solution_count);
    printf("Elapsed time: %ld ms\n", (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000);
}

int main(int argc, char*argv[]) { // Main function
    int num_threads = atoi(argv[1]); // Number of threads
    board_size = atoi(argv[2]); // Board size
    solve_queens(board_size, num_threads);
    return 0;
}
