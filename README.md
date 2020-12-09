# Game Of Life

Multithreaded implementation of the Game of Life using JavaThreads, OpenMP and PThreads with C.<br/>
A project of Concurrent and Distributed Programming class from ICT-UNIFESP.

## The Game
The Game of Life is a cellular automaton that simulates successive generations of a society with living organisms. It consists of an infinity two-dimensional board with square cells that are alive when it assumes 1 and dead when it assumes 0.

## Game rules
- Living cells with less than 2 (two) living neighbors die from abandonment;
- Each living cell with 2 (two) or 3 (three) neighbors must remain alive for the next generation;
- Each living cell with 4 (four) or more neighbors die from overpopulation;
- Each died cell with exactly 3 (three) neighbors must become alive.

## Run the commands below to compile and execute the codes

C serial version
> gcc -o serial serial_version.c

OpenMP version
> gcc -o omp -fopnemp omp_version.c

PThreads version
> gcc -o omp -pthread pthread_version.c

Java serial version
> javac SerialVersion.java
> java SerialVersion

JavaThreads
> javac JavaThreads.java
> java JavaThreads

## Parameters
- Max generations: 2000
- Board dimension: 2048X2048
- Number of threads: 1, 2, 4 and 8
- Initial state: pseudo random with seed = 1985

## Results

## Developed by
Angelina Gomes and Felipe Amorim
