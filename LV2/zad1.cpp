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
        vec[i] = std::rand();
    }
}

double calculateAverage(const std::vector<int>& vec) {
    int sum = 0;
    for (int val : vec) {
        sum += val;
    }
    return static_cast<double>(sum) / vec.size();
}

int main(int argc, char* argv[]) {
    std::srand(static_cast<unsigned int>(time(0)));

    MPI_Init(&argc, &argv);

    std::vector<int> a = {1}, b = {1}, c = {1}, d = {1};
    std::vector<int> firstReceived;
    int world_size, rank;
    MPI_Request requests[4];
    MPI_Status statuses[4];
    MPI_Status status;

    int total_runs = 100;
    long double total_avg = 0.0;
    int firstReceivedCount[4] = {0};

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (world_size < 3) {
        std::cout << "Less than 3 processes used\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
    }

    for (int run = 0; run < total_runs; ++run) {
        if (rank == 2) {
            fillVector(a);
            fillVector(b);
            fillVector(c);
            fillVector(d);

            MPI_Isend(&a.front(), a.size(), MPI_INT, 0, 20, MPI_COMM_WORLD, &requests[0]);
            MPI_Isend(&b.front(), b.size(), MPI_INT, 0, 40, MPI_COMM_WORLD, &requests[1]);
            MPI_Isend(&c.front(), c.size(), MPI_INT, 0, 60, MPI_COMM_WORLD, &requests[2]);
            MPI_Isend(&d.front(), d.size(), MPI_INT, 0, 80, MPI_COMM_WORLD, &requests[3]);

            MPI_Waitall(4, requests, statuses);

        } else if (rank == 0) {
            firstReceived.clear();
            firstReceived.resize(4);

            int size_a;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &size_a);
            MPI_Recv(&a.front(), size_a, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            firstReceived.push_back(status.MPI_TAG);

            int size_b;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &size_b);
            MPI_Recv(&b.front(), size_b, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            firstReceived.push_back(status.MPI_TAG);

            int size_c;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &size_c);
            MPI_Recv(&c.front(), size_c, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            firstReceived.push_back(status.MPI_TAG);

            int size_d;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &size_d);
            MPI_Recv(&d.front(), size_d, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            firstReceived.push_back(status.MPI_TAG);

            total_avg += (calculateAverage(a) + calculateAverage(b) + calculateAverage(c) + calculateAverage(d)) / 4.0;

            std::cout << firstReceived[0] << std::endl;

            if (firstReceived[0] == 20) {
                firstReceivedCount[0]++;
            } else if (firstReceived[0] == 40) {
                firstReceivedCount[1]++;
            } else if (firstReceived[0] == 60) {
                firstReceivedCount[2]++;
            } else if (firstReceived[0] == 80) {
                firstReceivedCount[3]++;
            }
        }
    }

    if (rank == 0) {
        std::cout << "Overall average value of all vectors: " << total_avg / total_runs << std::endl;

        int max_received_tag = std::max_element(firstReceivedCount, firstReceivedCount + 4) - firstReceivedCount;
        std::cout << "Tag received first the most times: " << (max_received_tag == 0 ? 20 : (max_received_tag == 1 ? 40 : (max_received_tag == 2 ? 60 : 80))) << std::endl;
    }

    MPI_Finalize();
    return 0;
}
