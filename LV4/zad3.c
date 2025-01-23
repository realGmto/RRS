#include <stdio.h>
#include <string.h>
#include <omp.h>

#define N 625

void print_matrix(int matrix[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int isBetter = 1, index;
    int A[N][N], B[N][N], result_parallel[N][N] = {0}, result_serial[N][N] = {0};
    double old_time = 0;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }
    }

    // Parallel
    for(index = 1; isBetter; index++){
        memset(result_parallel, 0, sizeof(result_parallel));
        double start_time = omp_get_wtime();

        omp_set_num_threads(index);
        #pragma omp parallel
        {
            #pragma omp for
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    for (int k = 0; k < N; k++) {
                        result_parallel[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
        }
        double end_time = omp_get_wtime();
        printf("Vrijeme paralelnog izvršavanja za %d niti: %f sekundi\n", index, end_time - start_time);
        if ((end_time - start_time)*1.1 >= old_time && old_time != 0)
            isBetter = 0;
        old_time = end_time - start_time;
    }

    printf("Optimalni broj petlji je: %d\n", index-2);
    //print_matrix(result_parallel);

    // Serial
    double start_time_serial = omp_get_wtime();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                result_serial[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    double end_time_serial = omp_get_wtime();
    printf("Vrijeme serijskog izvršavanja: %f sekundi\n", end_time_serial - start_time_serial);
    //print_matrix(result_serial);
    return 0;
}
