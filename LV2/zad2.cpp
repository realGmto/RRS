#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

#define MAX_ROUNDS 4
#define INITIATOR 0

void fillVector(std::vector<double>& vec, std::default_random_engine& re) {
    std::uniform_real_distribution<double> unif(0,1);
    vec.resize(100);
    for (int i = 0; i < 100; i++) {
        vec[i] = unif(re);
    }
}

void addRank(std::vector<double>& vec, int rank){
    for (double& val : vec) {
        val += rank;
    }
}

void multiplyRank(std::vector<double>& vec, int rank){
    for (double& val : vec) {
        val *= rank;
    }
}

void changeValues(std::vector<double>& vec, double target){
    for (double& val : vec) {
        val = target;
    }
}

void addRandValue(std::vector<double>& vec, std::default_random_engine& re){
    std::uniform_real_distribution<double> unif(-1,1);
    for (double& val : vec) {
        val += unif(re);
    }
}

double calculateAverage(const std::vector<double>& vec) {
    int sum = 0;
    for (double val : vec) {
        sum += val;
    }
    return static_cast<double>(sum) / vec.size();
}

void printVector(const std::vector<double>& vec){
    std::cout << "message values: ";
    for (double val : vec) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::default_random_engine re;
    std::vector<double> message;
    int round = 0;
    double time = 0;


    MPI_Init(&argc, &argv);

    int rank, world_size;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (world_size < 4) {
        std::cout << "Less than 3 processes used\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
    }

    fillVector(message, re);

    if( rank == INITIATOR)
        MPI_Send(message.data(), message.size(), MPI_INT, 0, 20, MPI_COMM_WORLD);

    while (round < MAX_ROUNDS){
        MPI_Status status;

        MPI_Recv(message.data(), message.size(), MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        // JOB here
        if ( rank == INITIATOR){
            time = MPI_Wtime() - time;
            std::cout << "Time passed for last round: " << time << std::endl;
        }
        std::cout << "Process #" << rank << ", primio poruku od procesa #" << status.MPI_SOURCE << ", ";
        printVector(message);

        if (round == 0)
            addRank(message, rank);
        else if (round == 1)
            multiplyRank(message, rank);
        else if (round == 2)
            changeValues(message, calculateAverage(message));
        else if (round == 3)
            addRandValue(message, re);

        MPI_Send(message.data(), message.size(), MPI_INT, (rank + 1) % world_size, 20, MPI_COMM_WORLD);
        round++;
    }
    if (rank == INITIATOR){
        MPI_Status status;
        MPI_Recv(message.data(), message.size(), MPI_DOUBLE, 3, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        std::cout << "Successfully passed ring topology" << std::endl;
        time = MPI_Wtime() - time;
        std::cout << "Time passed for last round: " << time << std::endl;
        printVector(message);
    }


    MPI_Finalize();
    return 0;
}
