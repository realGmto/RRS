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
    std::vector<std::vector<double>> rows(20);
    std::vector<std::vector<double>> abc;
    int world_size, rank;
    int num_of_rows = 0;
    double multiplier = 0; 
    std::vector<int> vector_size;
    int numberOfRows = 0;
    std::vector<double> sum(100);
    double average = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == INITIATOR){
        std::cout << "Unesite broj redova:";
        std::cin >> num_of_rows;
        int iteration = 0;
        do{
            std::cout << "Unesite cjelobrojne vektore: ";
            std::getline(std::cin, line);

            std::stringstream ss(line);
            double number;

            while (ss >> number) {
                abc[iteration].push_back(number);
                multiplier = number;
            }
            vector_size.push_back(abc[iteration].size());
            iteration++;
            numberOfRows++;
        }while(iteration < num_of_rows);
        std::cout << "Unesite decimalni broj: ";
        std::cin >> multiplier;
    }
    MPI_Bcast(&multiplier, 1, MPI_DOUBLE, INITIATOR, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfRows, 1, MPI_INT, INITIATOR, MPI_COMM_WORLD);

    rows.resize(numberOfRows/world_size);
    sum.resize(numberOfRows/world_size);
    MPI_Scatterv(abc.data(), vector_size.data(), displacement, MPI_DOUBLE, rows.data(), vector_size.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    for (auto &row : rows){
        for (auto &number : row){
            number *= multiplier;
        }
        printVector(row);
    }

    MPI_Reduce(rows.data(), sum.data(), numberOfRows/world_size, MPI_DOUBLE, MPI_SUM, INITIATOR, MPI_COMM_WORLD);
    MPI_Gather(rows.data(), numberOfRows/world_size, MPI_DOUBLE, abc.data(), numberOfRows/world_size, MPI_DOUBLE, INITIATOR, MPI_COMM_WORLD);

    /*if (rank == 0){
        printVector(abc);
        double summation = 0;
        for (double val : sum) {
            summation += val;
        }
        average = summation/vector_size;
        std::cout << "Average value of vector elements is: " << average << std::endl;
    }*/
    MPI_Finalize();
    return 0;
}