# OS-Fork-and-Schedule

## Objective
Create an init that will fork two other processes that will carry out a basic scheduling algorithm. Communication through the two processes are done using IPC (Interprocessing Communication). In this assignment, named pipes were used to communicate data between the two processes. One processes acted as a CPU and ticked off a certain amount of time from the process and the other process acted as the scheduler. The scheduler would send information to the CPU and retreive the new estimated running time.

## Current Issues:
Init and forking  
Documentation
