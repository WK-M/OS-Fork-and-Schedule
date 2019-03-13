# OS-Fork-and-Schedule

## Objective
Create an init that will fork two other processes that will carry out a basic scheduling algorithm. Communication through the two processes are done using IPC (Interprocessing Communication). In this assignment, named pipes were used to communicate data between the two processes. One processes acted as a CPU and ticked off a certain amount of time from the process and the other process acted as the scheduler. The scheduler would send information to the CPU and retreive the new estimated running time.

## How to run:
```c
gcc cpu_emulator.c -o cpu_emulator
gcc scheduler_process.c -o scheduler_process
gcc init.c -o init

./init
```

## Current Issues:
Documentation
