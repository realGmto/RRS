#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

void printVector(const std::vector<int>& abc){
    std::cout << "Received vector: ";
    for (double val : abc) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::default_random_engine re;

    MPI_Init(&argc, &argv);
    std::string line;
    std::vector<int> abc;
    int world_size, rank;
    double multiplier = 0; 
    int vector_size = 0;
    double average = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == 0){
        std::cout << "Unesite cjelobrojni vektor i jedan decimalni broj odvojene razmakom: ";
        std::getline(std::cin, line);

        std::stringstream ss(line);
        while (ss >> number) {
            abc.push_back(number);
            multiplier = number;
        }
        // Remove last number because it is multiplier
        if (abc.size() > 0) {
            abc.pop_back();
        }
        //Fill a vector and 1 decimal number
        vector_size = abc.size();

        MPI_Scatter(abc.data(), vector_size/world_size, MPI_INT, abc.data(), vector_size/world_size, MPI_INT, 0, MPI_COMM_WORLD)
        MPI_Bcast(&multiplier, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&vector_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    for (int &number : abc){
        number *= multiplier;
    }

    MPI_Gather(abc.data(), vector_size/world_size, MPI_INT, abc.data(), vector_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Reduce(abc.data(), &average, vector_size/world_size, MPI_DOUBLE, /*TODO*/, 0, MPI_COMM_WORLD);

    if (rank == 0){
        printVector(abc);
        std::cout << "Average value of vector elements is: " << average << std::endl;
    }
    MPI_Finalize();
    return 0;
}