#include <stdio.h>
#include <omp.h>

#define SIZE 8000

int main() {
    int x[SIZE], y = 0;
    int y_local = 0;
    
    for (int i = 0; i < SIZE; i++) {
        x[i] = i + 1;
    }

    printf("Initial value of y: %d\n", y);
    
    printf("Number of threads before parallel region: %d\n", omp_get_max_threads());

    #pragma omp parallel for reduction(+:y_local)
    for (int i = 3; i < SIZE; i++) {
        y_local += x[i - 3];
    }

    y += y_local;

    printf("Final value of y after parallel computation: %d\n", y);
    
    return 0;
}
