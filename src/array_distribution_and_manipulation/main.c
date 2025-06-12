#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define mcw MPI_COMM_WORLD

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

    int rank, size;
    int *arr = NULL, *result = NULL;
    MPI_Comm_rank(mcw, &rank);
    MPI_Comm_size(mcw, &size);

    const int PROCESSORS = size;

    int element;

    // Init array in root process
    if (rank == 0) {
        result = malloc(PROCESSORS * sizeof(int));
        arr = malloc(PROCESSORS * sizeof(int));
        for (int i = 0; i < PROCESSORS; ++i)
            arr[i] = i;
    }

    // Send one element to each process
    MPI_Scatter(arr, 1, MPI_INT, &element, 1, MPI_INT, 0, mcw);

    // Perform operation
    element += rank;

    // Gather results from all processes
    MPI_Gather(&element, 1, MPI_INT, result, 1, MPI_INT, 0, mcw);

    // Print result and free allocated memory
    if (rank == 0) {
        print(result, PROCESSORS);
        free(result);
        free(arr);
    }

    MPI_Finalize();
    return 0;
}