#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

void fillVector(std::vector<int>& vec) {
    int numOfElements = std::rand() % 16 + 5;
    vec.resize(numOfElements);
    for (int i = 0; i < numOfElements; i++) {
        vec[i] = std::rand() % 100 + 1;
    }
}

double calculateAverage(const std::vector<int>& vec) {
    int sum = 0;
    for (int val : vec) {
        sum += val;
    }
    return static_cast<double>(sum) / vec.size();
}

void printVector(const std::vector<int>& vec){
    for (int val : vec) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::srand(static_cast<unsigned int>(time(0)));


    MPI_Init(&argc, &argv);
    long double total_avg = 0.0;
    int firstReceivedCount[4] = {0};

    std::vector<int> vector = {1};
    int world_size, rank;
    MPI_Request requests[4];
    MPI_Status statuses[4];

    int total_runs = 100;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (world_size < 3) {
        std::cout << "Less than 3 processes used\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
    }

    for (int run = 0; run < total_runs; ++run) {
        if (rank == 2) {
            for(int k = 0; k < 4; k++){
                fillVector(vector);
                MPI_Isend(vector.data(), vector.size(), MPI_INT, 0, 20*(k+1), MPI_COMM_WORLD, &requests[k]);
            }

            MPI_Waitall(4, requests, statuses);

        } else if (rank == 0) {
            double sum = 0;
            int firstReceived;

            for(int k = 0; k < 4; k++){
                int size;
                MPI_Status status;

                MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_INT, &size);
                vector.resize(size);
                MPI_Recv(vector.data(), 20, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                std::cout<< "Primljena poruka #" << (k + 1) << ", sa tagom " << status.MPI_TAG <<", duljina " << size << ": ";
                printVector(vector);
                if( k == 0){
                    firstReceived = status.MPI_TAG;
                }
                sum += size;
            }

            total_avg += sum / 4.0;

            if (firstReceived == 20) {
                firstReceivedCount[0]++;
            } else if (firstReceived == 40) {
                firstReceivedCount[1]++;
            } else if (firstReceived == 60) {
                firstReceivedCount[2]++;
            } else if (firstReceived == 80) {
                firstReceivedCount[3]++;
            }
        }
    }

    if (rank == 0) {
        std::cout << "Overall average size of all vectors: " << total_avg / total_runs << std::endl;

        int max_received_tag = std::max_element(firstReceivedCount, firstReceivedCount + 4) - firstReceivedCount;
        std::cout << "Tag received first the most times: " << (max_received_tag == 0 ? 20 : (max_received_tag == 1 ? 40 : (max_received_tag == 2 ? 60 : 80))) << std::endl;
    }

    MPI_Finalize();
    return 0;
}
