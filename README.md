# 🔧 message-passing-interface-tasks

This project demonstrates the use of MPI (Message Passing Interface) to solve two classic parallel computing problems:

1. **Maximum Element Finder** – parallel array processing to find the maximum value and its index.
2. **Distributed Caesar Cipher** – string encryption/decryption using parallel character processing.

---

## 📁 Project Structure

```
.
├── maximum_element_finder
│   ├── src/ 
│   │   └── main.c # Max finder using MPI (Master-Slave) 
│   └── reports/ 
│       └── output_example.png # Screenshot showing output of max element task 
├── ceaser_cipher/ 
│   └── src/ 
│       └── main.c # Main entry for Caesar cipher 
│       └── utils/ 
│           ├── driver.c # Logic to handle MPI string processing 
│           └── utils.c # Utility functions (send/receive + Caesar logic)
├── .gitignore        # Files and directories to ignore
└── README.md         # Project documentation

```

---

## 🧠 Problem Descriptions

### 🟢 Problem 1 – Maximum Element in Array

A parallel MPI C program where:
- The master process reads an integer array and distributes partitions to slave processes.
- Each process computes the local maximum of its partition.
- The master collects local maxima and determines the global maximum and its index.

#### Run Instructions

**Compile:**
```bash
mpicc maximum_element_finder/src/main.c -o max_finder.out
```

**Run:**
```bash
mpirun -n 4 max_finder.out
```

---

### 🟡 Problem 2 – Distributed Caesar Cipher (Encode/Decode)

A parallel program to encrypt or decrypt a string using the Caesar cipher technique with a fixed shift (3), distributed among MPI processes.

#### Features:
- Supports both encoding and decoding
- Handles uneven string splits
- Processes both alphabetic and non-alphabetic characters
- Modular design using separate utility files

#### Run Instructions

**Compile:**
```bash
mpicc maximum_element_finder/src/main.c -o caesar_cipher.out
```

**Run:**
```bash
mpirun -n 4 caesar_cipher.out
```
---
## 🧰 Requirements

- C Compiler (`gcc`)
- MPI Library (`OpenMPI` or `MPICH`)
- Unix-based environment (Linux/macOS/WSL)

---

## ⚙️ Technical Notes

- Uses only `MPI_Send` and `MPI_Recv` for communication
- Designed to run on any MPI-supported system (Linux, WSL, or via emulators)
- Properly handles cases where data size isn't evenly divisible by the number of processes
