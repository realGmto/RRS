#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <sstream>

#define INITIATOR 0

void printVector(const std::vector<double>& abc){
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
    std::vector<double> allHave(200);
    std::vector<double> abc;
    int world_size, rank;
    double multiplier = 0; 
    int vector_size = 0;
    std::vector<double> sum(100);
    double average = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == INITIATOR){
        do{
            abc.clear();
            std::cout << "Unesite cjelobrojni vektor koji je dijeljiv sa " << world_size <<" i jedan decimalni broj odvojene razmakom: ";
            std::getline(std::cin, line);

            std::stringstream ss(line);
            double number;

            while (ss >> number) {
                abc.push_back(number);
                multiplier = number;
            }
            // Remove last number because it is multiplier
            if (abc.size() > 0) {
                abc.pop_back();
            }
            vector_size = abc.size();
            std::cout << "vector size: " << vector_size <<std::endl;
        }while(vector_size % world_size != 0);
    }
    MPI_Bcast(&multiplier, 1, MPI_DOUBLE, INITIATOR, MPI_COMM_WORLD);
    MPI_Bcast(&vector_size, 1, MPI_INT, INITIATOR, MPI_COMM_WORLD);

    allHave.resize(vector_size/world_size);
    sum.resize(vector_size/world_size);
    MPI_Scatter(abc.data(), vector_size/world_size, MPI_DOUBLE, allHave.data(), vector_size/world_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    for (double &number : allHave){
        number *= multiplier;
    }

    MPI_Reduce(allHave.data(), sum.data(), vector_size/world_size, MPI_DOUBLE, MPI_SUM, INITIATOR, MPI_COMM_WORLD);
    MPI_Gather(allHave.data(), vector_size/world_size, MPI_DOUBLE, abc.data(), vector_size/world_size, MPI_DOUBLE, INITIATOR, MPI_COMM_WORLD);

    if (rank == 0){
        printVector(abc);
        double summation = 0;
        for (double val : sum) {
            summation += val;
        }
        average = summation/vector_size;
        std::cout << "Average value of vector elements is: " << average << std::endl;
    }
    MPI_Finalize();
    return 0;
}