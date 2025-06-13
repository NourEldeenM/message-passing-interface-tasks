#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.c"

int driver()
{
    int rank, proc_count, input_len, option, file;
    char string[SIZE];

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(mcw, &rank);
    MPI_Comm_size(mcw, &proc_count);

    // master function
    if (rank == 0)
    {
        // option input
        printf("Encode (1) - Decode(2): ");
        fflush(stdout);
        scanf("%d", &option);
        clear_input_buffer();
        if (option != 1 && option != 2)
        {
            printf("Invalid choice\n*\n*\n");
            fflush(stdout);
            MPI_Abort(mcw, 400);
        }

        // method input
        int inputMethod;
        printf("File (1) - Console (2): ");
        fflush(stdout);
        scanf("%d", &inputMethod);
        clear_input_buffer();
        if (inputMethod != 1 && inputMethod != 2)
        {
            printf("Invalid choice\n*\n*\n");
            fflush(stdout);
            MPI_Abort(mcw, 400);
        }


        // file input
        if (inputMethod == 1)
        {
            char fileName[SIZE];
            printf("File name: ");
            fflush(stdout);
            fgets(fileName, SIZE, stdin);

            FILE *file = fopen("input.txt", "r");
            if (file == NULL)
            {
                perror("Error opening file");
                return 1;
            }

            size_t len = fread(string, sizeof(char), SIZE - 1, file);
            string[len] = '\0'; // Null-terminate the string
            fclose(file);
        }
        // console input
        else
        {
            printf("Enter String: ");
            fflush(stdout);
            fgets(string, SIZE, stdin);
            string[strcspn(string, "\n")] = '\0'; // remove newline
        }

        input_len = strlen(string);

        int base_size = input_len / proc_count;
        int remainder = input_len % proc_count;

        send_data_to_processes(string, proc_count, base_size, remainder, option);
        // process own chunk
        int my_chunk = base_size + (0 < remainder ? 1 : 0);
        process_chunk(string, my_chunk, option); // process in-place

        recv_data_from_processes(string, proc_count, base_size, remainder);

        // print result
        printf("Result: %s\n", string);
    }
    // worker functions
    else 
    {
        MPI_Recv(&option, 1, MPI_INT, 0, 0, mcw, MPI_STATUS_IGNORE);
        int chunk_size;
        MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, mcw, MPI_STATUS_IGNORE);
        char *buffer = (char *)malloc(chunk_size + 1);
        MPI_Recv(buffer, chunk_size, MPI_CHAR, 0, 0, mcw, MPI_STATUS_IGNORE);
        buffer[chunk_size] = '\0';

        process_chunk(buffer, chunk_size, option);

        MPI_Send(buffer, chunk_size, MPI_CHAR, 0, 0, mcw);
        free(buffer);
    }

    MPI_Finalize();
}