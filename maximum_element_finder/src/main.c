#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define MASTER 0

int main(int argc, char *argv[]) {
    int rank, numberOfProcesses;
    int arraySize;
    int *array = NULL;
    int *subArray = NULL;
    int elementsPerProcess, remainder;
    int startIndex, subArraySize;
    int localMax, localIndex;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // current process ID
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses); // number of processes

    if (rank == MASTER) {
        printf("Hello from master process.\n");
        printf("Number of slave processes is %d\n", numberOfProcesses - 1);

        printf("Please enter size of array...\n");
        scanf("%d", &arraySize);

        array = (int *) malloc(arraySize * sizeof(int));

        printf("Please enter array elements ...\n");
        for (int i = 0; i < arraySize; i++) {
            scanf("%d", &array[i]);
        }

        elementsPerProcess = arraySize / numberOfProcesses;
        remainder = arraySize % numberOfProcesses;

        // Send array size, elementsPerProcess, and remainder to slave processes
        for (int dest = 0; dest < numberOfProcesses; dest++) {
            if (dest != MASTER) {
                MPI_Send(&arraySize, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&elementsPerProcess, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&remainder, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            }
        }

        // Send subarray to each slave process
        for (int dest = 0; dest < numberOfProcesses; dest++) {
            if (dest != MASTER) {
                int slaveSubArraySize;
                int slaveStartIndex;

                if (dest < remainder) {
                    slaveSubArraySize = elementsPerProcess + 1;
                    slaveStartIndex = dest * slaveSubArraySize;
                } else {
                    slaveSubArraySize = elementsPerProcess;
                    slaveStartIndex = dest * slaveSubArraySize + remainder;
                }

                MPI_Send(&array[slaveStartIndex], slaveSubArraySize, MPI_INT, dest, 0, MPI_COMM_WORLD);
            }
        }

        // Master process's portion
        if (MASTER < remainder) {
            subArraySize = elementsPerProcess + 1;
            startIndex = MASTER * subArraySize;
        } else {
            subArraySize = elementsPerProcess;
            startIndex = MASTER * subArraySize + remainder;
        }

        subArray = (int *) malloc(subArraySize * sizeof(int));

        for (int i = 0; i < subArraySize; i++) {
            subArray[i] = array[startIndex + i];
        }
    } else {
        MPI_Recv(&arraySize, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&elementsPerProcess, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&remainder, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (rank < remainder) {
            subArraySize = elementsPerProcess + 1;
        } else {
            subArraySize = elementsPerProcess;
        }

        subArray = (int *) malloc(subArraySize * sizeof(int));

        MPI_Recv(subArray, subArraySize, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        localMax = subArray[0];
        localIndex = 0;
        for (int i = 1; i < subArraySize; i++) {
            if (subArray[i] > localMax) {
                localMax = subArray[i];
                localIndex = i;
            }
        }
    }

    // Find local max value and index for ALL processes
    // Calculate global index of max element
    if (rank != MASTER) {
        int globalOffset;
        if (rank < remainder) {
            globalOffset = rank * (elementsPerProcess + 1);
        } else {
            globalOffset = rank * elementsPerProcess + remainder;
        }

        int globalIndex = globalOffset + localIndex;

        printf("Hello from slave#%d Max number in my partition is %d and index is %d.\n", rank, localMax, localIndex);

        MPI_Send(&localMax, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        MPI_Send(&globalIndex, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
    } else {
        int finalMax = subArray[0];
        int finalIndex = startIndex;

        for (int i = 1; i < subArraySize; i++) {
            if (subArray[i] > finalMax) {
                finalMax = subArray[i];
                finalIndex = startIndex + i;
            }
        }

        for (int src = 1; src < numberOfProcesses; src++) {
            int receivedMax, receivedIndex;
            MPI_Recv(&receivedMax, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&receivedIndex, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (receivedMax > finalMax) {
                finalMax = receivedMax;
                finalIndex = receivedIndex;
            }
        }

        printf("Master process announce the final max which is %d and its index is %d.\n", finalMax, finalIndex);
        printf("\nThanks for using our program\n");
    }

    if (rank == MASTER) {
        free(array);
    }
    free(subArray);

    MPI_Finalize();
    return 0;
}
