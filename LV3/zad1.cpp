#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

void fillVector(std::vector<double>& vec, int numOfElements, std::default_random_engine& re) {
    std::uniform_real_distribution<double> unif(0,100);
    vec.resize(numOfElements);
    for (int i = 0; i < numOfElements; i++) {
        vec[i] = unif(re);
    }
}

int main(int argc, char* argv[]) {
    std::default_random_engine re;

    MPI_Init(&argc, &argv);
    int world_size, rank;
    std::vector<double> abc;

    fillVector(abc, 10, re);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){
        MPI_Bcast(&abc.data()[8], 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    std::cout << "P" << rank << " prima: " << abc[8] << " " << abc[9] << std::endl;
    MPI_Finalize();
    return 0;
}