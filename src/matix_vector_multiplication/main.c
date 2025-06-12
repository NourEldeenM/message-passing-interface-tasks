#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define mcw MPI_COMM_WORLD

void print(int *v, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        printf("%d ", v[i]);
        fflush(stdout);
    }
    printf("\n");
}

int main()
{
    MPI_Init(NULL, NULL);

    int rank, size;
    int rows = 4, columns = 2, rows_per_proc;
    int *matrix = NULL, *vector = NULL, *local_matrix = NULL, *local_result = NULL;

    MPI_Comm_rank(mcw, &rank);
    MPI_Comm_size(mcw, &size);

    if (rows % size != 0)
    {
        if (rank == 0)
        {
            fprintf(stderr, "Number of rows (%d) must be divisible by number of processes (%d)\n", rows, size);
        }
        MPI_Finalize();
        return 1;
    }

    // rows to divide per processes
    rows_per_proc = rows / size;

    // allocate vector
    vector = malloc(columns * sizeof(int));

    if (rank == 0)
    {
        // Allocate & initialize matrix, vector
        matrix = malloc(rows * columns * sizeof(int));

        for (int i = 0; i < rows * columns; ++i)
        {
            matrix[i] = i + 1;
        }
        for (int i = 0; i < columns; ++i)
        {
            vector[i] = i + 1;
        }
    }

    // broadcast vector from root process to all processes
    MPI_Bcast(vector, columns, MPI_INT, 0, mcw);

    // allocate local matrix to receive
    local_matrix = malloc(rows_per_proc * columns * sizeof(int));

    // scatter rows from main matrix to all processes
    MPI_Scatter(matrix, rows_per_proc * columns, MPI_INT, local_matrix, rows_per_proc * columns, MPI_INT, 0, mcw);

    // allocate local result matrix and store in it computed values
    local_result = malloc(rows_per_proc * sizeof(int));
    for (int i = 0; i < rows_per_proc; i++)
    {
        local_result[i] = 0;
        for (int j = 0; j < columns; j++)
        {
            local_result[i] += local_matrix[i * columns + j] * vector[j];
        }
    }

    // allocate global result matrix
    int *global_result = NULL;
    if (rank == 0)
        global_result = malloc(rows * sizeof(int));

    // gather local results from all processes and store in global result in process zero
    MPI_Gather(local_result, rows_per_proc, MPI_INT, global_result, rows_per_proc, MPI_INT, 0, mcw);

    // print result and free memory
    if (rank == 0) {
        print(global_result, rows);
        free(global_result);
        free(matrix);
    }
    
    free(vector);
    free(local_matrix);
    free(local_result);

    MPI_Finalize();
    return 0;
}