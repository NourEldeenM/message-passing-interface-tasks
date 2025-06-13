#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define mcw MPI_COMM_WORLD

// return 0 if not prime, 1 if prime
int isPrime(int n)
{
    if (n < 2)
        return 0;
    for (int i = 2; i * i <= n; ++i)
        if (n % i == 0)
            return 0;
    return 1;
}

// print dynamic array given its size
void print(int *v, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d ", v[i]);
        fflush(stdout);
    }
    printf("\n");
}

int main() {
    MPI_Init(NULL, NULL);

    int rank, size, start, end, total_nums, portion_size;
    int *global_nums = NULL, *local_nums = NULL, 
        *global_result = NULL, *local_result = NULL;

    MPI_Comm_rank(mcw, &rank);
    MPI_Comm_size(mcw, &size);

    if (rank == 0) {
        // take input
        scanf("%d %d", &start, &end);
        total_nums = end - start + 1;
        portion_size = total_nums / size;
        
        // make sure each process has enough size for taking its portion
        if (total_nums % size != 0)
            portion_size++;

        // initialize global nums array
        global_nums = malloc(total_nums * sizeof(int));
        for (int i = start; i <= end; ++i) {
            global_nums[i - start] = i;
        }
    }

    // send portion size to processes
    MPI_Bcast(&portion_size, 1, MPI_INT, 0, mcw);

    // recieve local chuncks to be processed independently
    local_nums = malloc(portion_size * sizeof(int));
    MPI_Scatter(global_nums, portion_size, MPI_INT, local_nums, portion_size, MPI_INT, 0, mcw);

    // calculate results
    local_result = malloc(portion_size * sizeof(int));
    int index = 0;
    for (int i = 0; i < portion_size; ++i) {
        if (isPrime(local_nums[i])) {
            local_result[index] = local_nums[i];
            index++;
        }
    }

    // recvcounts - array of how many elements each rank sends
    // displs - array of the starting index (displacement) in the final receive buffer for each rank
    int *recvcounts = NULL, *displs = NULL;
    int local_count = index;

    if (rank == 0) {
        recvcounts = malloc(size * sizeof(int));
    }
    // gather local counts from each process, and store in received counts
    MPI_Gather(&local_count, 1, MPI_INT, recvcounts, 1, MPI_INT, 0, mcw);

    if (rank == 0) {
        // calculate total number of primes, and displacement.
        int total_primes = 0;
        displs = malloc(size * sizeof(int));
        displs[0] = 0;
        total_primes += recvcounts[0];
        for (int i = 1; i < size; ++i) {
            displs[i] = displs[i - 1] + recvcounts[i - 1];
            total_primes += recvcounts[i];
        }
        // initialize global result array
        global_result = malloc(total_primes * sizeof(int));
    }

    // gather variable length results from processes, and store in global_result in root process
    MPI_Gatherv(local_result, local_count, MPI_INT, global_result, recvcounts, displs, MPI_INT, 0, mcw);

    // print result, and free memory
    if (rank == 0) {
        int total_primes = 0;
        for (int i = 0; i < size; ++i)
            total_primes += recvcounts[i];
        print(global_result, total_primes);
        free(global_result);
        free(recvcounts);
        free(displs);
        free(global_nums);
    }

    free(local_nums);
    free(local_result);

    MPI_Finalize();
    return 0;
}