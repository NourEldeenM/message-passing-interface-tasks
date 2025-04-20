#include <mpi.h>

#define mcw MPI_COMM_WORLD
#define SIZE 1000
#define SHIFT 3

void send_data_to_processes(char string[], int proc_count, int base_size, int remainder)
{
    int offset = base_size + (0 < remainder ? 1 : 0); // Master's chunk size
    for (int i = 1; i < proc_count; i++)
    {
        int chunk_size = base_size + (i < remainder ? 1 : 0);
        MPI_Send(&chunk_size, 1, MPI_INT, i, 0, mcw);
        MPI_Send(string + offset, chunk_size, MPI_CHAR, i, 0, mcw);
        offset += chunk_size;
    }
}

void recv_data_from_processes(char string[], int proc_count, int base_size, int remainder)
{
    int my_chunk = base_size + (0 < remainder ? 1 : 0);
    int offset = my_chunk;
    for (int i = 1; i < proc_count; i++)
    {
        int chunk_size = base_size + (i < remainder ? 1 : 0);
        MPI_Recv(string + offset, chunk_size, MPI_CHAR, i, 0, mcw, MPI_STATUS_IGNORE);
        offset += chunk_size;
    }
}

char shift_char(char c, int encode)
{
    if (c < 'a' || c > 'z')
        return c;
    if (encode == 1)
        return ((c - 'a' + SHIFT) % 26 + 'a');
    else
        return ((c - 'a' - SHIFT + 26) % 26 + 'a');
}

void process_chunk(char *chunk, int length, int encode)
{
    for (int i = 0; i < length; i++)
        chunk[i] = shift_char(chunk[i], encode);
}
