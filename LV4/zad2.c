#include <stdio.h>
#include <omp.h>
#include <string.h>

#define N 1024

void print_matrix(int matrix[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_vector(int vector[N]) {
    for (int i = 0; i < N; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n");
}

int main() {
    int isBetter = 1, index;
    int v[N];
    int m[N][N];
    int result_parallel[N] = {0};
    int result_serial[N] = {0};
    double old_time = 0;

    for (int i = 0; i < N; i++) {
        v[i] = i + 1;
        for (int j = 0; j < N; j++) {
            m[i][j] = (i + 1) * (j + 1);
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
                    result_parallel[i] += m[i][j] * v[j];
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
    //print_vector(result_parallel);

    // Serial
    double start_time_serial = omp_get_wtime();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            result_serial[i] += m[i][j] * v[j];
        }
    }

    double end_time_serial = omp_get_wtime();
    printf("Vrijeme serijskog izvršavanja: %f sekundi\n", end_time_serial - start_time_serial);
    //print_vector(result_serial);
    return 0;
}
