#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 40000000 // Size of the array to sort
#define THRESHOLD 24  // Threshold for switching to insertion sort

void insertionSort(double *A, int low, int high) { // Insertion sort
  int i, j;
  for (i = low + 1; i <= high; i++) { 
    int key = A[i]; // The element to be inserted
    j = i - 1;
    while (j >= low && A[j] > key) { // Shift the elements to the right until the correct position is found
      A[j + 1] = A[j];
      j = j - 1;
    }
    A[j + 1] = key; // Insert the element
  }
}

void bottomUpMergeSort(double *A, double *temp, int n) { // Bottom-up merge sort
  int width;
  for (width = 1; width < n; width = 2 * width) {
    int i;
    #pragma omp parallel for // Parallelizing this loop
    for (i = 0; i < n; i = i + 2 * width) {
      int low = i;
      int mid = i + width - 1;
      int high = i + 2 * width - 1;
      high = (high < n) ? high : n - 1; // Make sure high is within the bounds of the array
      int left = low;
      int right = mid + 1;
      int k;
      if (high - low <= THRESHOLD) { // Switch to insertion sort if the subarray is small
        insertionSort(A, low, high);
      } else {
        for (k = low; k <= high; k++) { // Merge the two subarrays
          if (left <= mid && (right > high || A[left] <= A[right])) {
            temp[k] = A[left]; // Copy the element from the left subarray
            left++;
          } else {
            temp[k] = A[right]; // Copy the element from the right subarray
            right++;
          }
        }
      }
    }
    int j;
    // Copy the values from B back to A
    for (j = 0; j < n; j++) { 
      A[j] = temp[j];     
    }
  }
}

int main() { // Main function
	
  int i,j,k;
  double *A, *temp;
  
  int num_threads[] = {1, 2, 4, 6, 8, 10, 12, 14, 16};
  int num_threads_count = sizeof(num_threads) / sizeof(num_threads[0]);
  for(k = 0; k <5; k++){
  for (j = 0; j < num_threads_count; j++) {

  // Allocate memory for the arrays
  A = (double*) malloc(N * sizeof(double));
  temp = (double*) malloc(N * sizeof(double));
  if (A == NULL || temp == NULL) {
    printf("Memory allocation failed.\n");
    exit(1);
  }

  // Initialize the random number generator with the current time
  srand(time(NULL));
	
	for (i = 0; i < N; i++) {
    A[i] = (double)rand()/ RAND_MAX * 40000000;
    temp[i] = A[i];
  } 
  
  // Set the number of threads to use
  omp_set_num_threads(num_threads[j]);

  // Sort the array using bottom-up merge sort
  double start = omp_get_wtime();
  bottomUpMergeSort(A, temp, N);
  double end = omp_get_wtime();
  double elapsed = end - start;
  printf("%d  %f\n",num_threads[j],elapsed);

  
	/*for (i = 0; i < 10; i++) {
    printf("A[%d] = %4f\n", i, A[i]);
  }*/

  // Free the memory
  free(A);
  free(temp);
}
}
  return 0;
}