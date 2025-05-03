# Course Project: Parallel Programming

## Description

This is a project for the requirements of the university course "Parallel and Distributed Computer Architecture ACE418" in 2017-2018.  
The description files of the assignments are not uploaded.  
The aim of the project is to get familiarized with parallel programming.

## Technologies Used

- Languages: C
- Tools: Linux, [POSIX threads standard](https://computing.llnl.gov/tutorials/pthreads/), [OpenMP API](https://www.openmp.org/), [Streaming SIMD Extensions (SSE)](https://software.intel.com/sites/landingpage/IntrinsicsGuide), [Message Passing Interface (MPI)](http://mpitutorial.com/tutorials/)

## First Assignment

Files:

```text
ACE418/
├── assignment_1/
│   ├── init.c
│   ├── Makefile
│   ├── openmpa.c
│   ├── openmpb.c
│   ├── openmpc.c
│   ├── pthreada.c
│   ├── pthreadb.c
│   ├── pthreadc.c
│   └── sequential.c
├── assignment_2/
└── README.md
```

The main goal of this assignment is to calculate the time it takes for various parallel implementations compared to a sequential implementation. The task involves computing the [hamming distance](https://en.wikipedia.org/wiki/Hamming_distance) between two strings. These strings are in sets `A` and `B`. Set `A` contains `m` strings, and set `B` contains `n` strings, with each string having a length of `l`. Where `m, n = 1, 100, 1000, 10000` and `l = 10, 100, 1000`.

In each implementation, all the hamming distances have to be calculated, for all possible combinations, resulting in `m*n` combinations.
To evaluate the effectiveness of the parallel implementations, several files have been created: one for the serial implementation, three for Pthreads, and three for OpenMP.

### String Creation

Initially, all the strings are generated and saved into files, which are later used to calculate the hamming distances. These files contain all possible combinations for the variable sizes of the sets `A`, `B`, and the variable length of the strings. Each file is appropriately named to reflect its contents.  
For example, a file that contains the arrays `A` with size `m=100`, `B` with size `n=1000` and string length `l=10`, would be named `z_m_100_n_1000_l_10`.  
This is implemented in file `init.c`

### Sequential

The data is read from the created files, and the hamming distance is calculated serially. All the distances are then summed and displayed.

### Pthread

For the pthread implementation, the function `void *Hamming_Distance_Function(void *threadid)` was developed. This function is the task that will be assigned to the thread, and it will be different for each task.  
The threads are then created by having the appropriate arguments according to the corresponding task. Task synchronization is achieved through the use of barriers before the threads are joined.  
Protection between threads is ensured by using mutexes.

#### Task A

In the `pthreada.c` file, `task a` is calculated. This task has the smallest granularity. In `task a`, parallelization occurs during the comparison of parts of the strings. By comparing parts of the string, the "local" hamming distances are calculated in each thread and added to the total. The total hamming distance of the file is calculated serially.

#### Task B

`Task b` has medium granularity, where parallelization occurs in the comparison of a pairs of strings. The total hamming distance of the file is calculated serially.

#### Task C

`Task c` has the highest granularity. Here, parallelization occurs by comparing one string from one set with all strings from another set. The total hamming distance of the file is calculated serially.

### OpenMP

The implementation for the OpenMP is simpler as the `#pragma omp parallel for private(index)` instruction was used for the loop parallelization.  
Protection between the threads is ensured by using the instruction `#pragma omp atomic`.

#### Task A

`Task a` has the smallest granularity. As in pthreads, `Task a` parallelization takes place during the comparison of parts of the strings, where each thread calculates a "local" hamming distance to be added to the total. The total hamming distance of the file is calculated serially. The instruction `#pragma omp parallel for private(index)` is applied to the loop that runs over the length of the strings.

#### Task B

`Task b` has medium granularity, meaning that the parallelization takes place in the comparison of a pair of strings. The total hamming distance of the file is calculated serially. The instruction `#pragma omp parallel for private(index)` is applied to the loop that runs over the set of strings. In the case that set B has one string, we parallelize with respect to set A, by applying the necessary checks.

#### Task C

`Task c` has the highest granularity, focusing on comparing one string from one set against all strings from the other set. The total hamming distance of the file is calculated serially.

## Second Assignment

Files:

```text
ACE418/
├── assignment_1/
├── assignment_2/
│   ├── mpi.c
│   ├── reference.c
│   ├── script.run
│   └── sse.c
└── README.md
```

### SSE

In the second assignment, the arrays were vectorized in order to speed up the calculation of a given formula.  
Initially, `sse` instructions were used, and changes were made only within the `for` loop. The `unroll` and `jam` techniques were used, by changing the step from 1 to 4. Additionally, the instructions that had the format mm_\<instruction>_ps were used, which works with 4 floats as each of them corresponds to 32 bits (4 bytes) and are stored in one 128-bit (16 bytes) variable.  

The instructions used were:

- `load` (takes 4 consecutive values ​​and stores them in a 128-bit variable),
- `add` (addition),
- `sub` (subtraction),
- `div` (division),
- `mul` (multiplication),
- `setl` (places a constant per 32-bit),
- `store` (stores a 128-bit variable in an array in 4 consecutive positions).  

The following speedup was observed by executing the `sse` instructions:

| N | Speedup |
|:---:|:---:|
| 100 | 1.00 |
| 1000 | 1.86 |
| 10000 | 1.42 |
| 100000 | 1.41 |

It was noticed that by using the `sse` instructions, the execution times of the calculations are reduced, except for the case N = 100. `malloc()` was also replaced by `_mm_malloc()` in `SSE` as well as in `MPI+SSE`.  

### MPI

Regarding parallelism with `MPI`, speedup of execution was achieved, but only in the cases that it was able to run. The cases where `MPI` could not be applied were those that `(N/4)/numtasks` was not an integer, where `4` are the numbers that sse "draws", and numtasks is the parameter `P`. They are shown in the table below, as well as the speedups for the cases that it was able to run.  

## Contributors

- [Andreas Polychronakis](https://github.com/your-username)
- [Zisis Charokopos](https://github.com/zisxar)
