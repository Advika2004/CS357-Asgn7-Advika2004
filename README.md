### CSC 357 Assignment 7 - Pipes

Purpose:
- This assignment involves implementing a parallel merge sort using processes in C, utilizing pipes for interprocess communication. The goal is to understand process creation, management, and interprocess communication using pipes.

Ground Rules:
- Code must compile without memory leaks using gcc and valgrind.
- The program must be compatible with the Cal Poly UNIX servers, using only POSIX-compliant system calls and library functions.

Parallel Merge Sort:
- Implement a function in fsort.c to sort an array using a parallel merge sort approach:
- The array is recursively divided into halves, creating a new process to sort each half.
- For even-length arrays, the first half is sorted in the parent process and the second half in a child process.
- For odd-length arrays, the first half is sorted in the parent process, and the child process handles the slightly larger second half.
- If fork() fails during the creation of a child process, the array should be left in its partially sorted state, and the function must return 1. Otherwise, it should return 0.

Interprocess Communication with Pipes:
- Child processes have their own copy of the unsorted array in memory. Once sorted, they send the results back to the parent process via a pipe.
- It is assumed that each element fits within the pipe, but there’s no assumption about the entire array fitting at once, so the function must handle partial data transfers as needed.

Using void Pointer Offsets:
- The function operates on arrays of any type, requiring void pointers and the size of each element for proper indexing.
- Pointers are cast to char* to handle generic memory manipulation based on the width of each element.

Key System Calls and Functions:
- The assignment suggests using the following:
- pipe, read, and write: For interprocess communication.
- memcpy: To copy data between buffers when merging sorted subarrays.

Testing and Compilation:
- The function will be tested with unit tests in fsorttests.c. It must pass these tests, which check the correctness of the sorting function and its handling of child processes.
- The fsort.c code must be compiled into an object file and then linked into a static archive (fsort.a) for efficient distribution.
