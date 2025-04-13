#include <mpi.h>
#include <stdio.h>

#define mcw MPI_COMM_WORLD

int main(int argc, char *argv[]) {
	
	char string[100];
	printf("Enter a string: ");
	fgets(string, sizeof(string), stdin);

	int encrypt;
	printf("Enter 1 for encryption, 2 for decryption: ");
	fgets(encrypt, sizeof(encrypt), stdin);

	if (encrypt != 1 && encrypt != 2) {
		printf("Invalid input option, ending the program...");
		return 1;
	}

	MPI_Init(NULL, NULL);

	int rank;
	MPI_Comm_rank(mcw, &rank);

	int size;
	MPI_Comm_size(mcw, &size);


	MPI_Finalize();
}

