#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.c"

int driver() {
    int rank, proc_count, input_len;
    char string[SIZE];

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(mcw, &rank);
    MPI_Comm_size(mcw, &proc_count);

    if (rank == 0)
    {
        printf("Enter String: ");
        fflush(stdout); // ensure the prompt is displayed
        fgets(string, SIZE, stdin);
        string[strcspn(string, "\n")] = '\0'; // remove newline
        input_len = strlen(string);

        int base_size = input_len / proc_count;
        int remainder = input_len % proc_count;

        send_data_to_processes(string, proc_count, base_size, remainder);

        // process own chunk
        int my_chunk = base_size + (0 < remainder ? 1 : 0);
        process_chunk(string, my_chunk, 0); // process in-place

        recv_data_from_processes(string, proc_count, base_size, remainder);

        // print result
        printf("Result: %s\n", string);
    }
    else
    {
        int chunk_size;
        MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, mcw, MPI_STATUS_IGNORE);
        char *buffer = (char *)malloc(chunk_size + 1);
        MPI_Recv(buffer, chunk_size, MPI_CHAR, 0, 0, mcw, MPI_STATUS_IGNORE);
        buffer[chunk_size] = '\0';

        process_chunk(buffer, chunk_size, 0);

        MPI_Send(buffer, chunk_size, MPI_CHAR, 0, 0, mcw);
        free(buffer);
    }

    MPI_Finalize();
}