# Assignment 3

## Objective:

In this assignment, there is a focus on scheduling. There are several scheduling algorithms inplemented:

- FCFS
- SJN
- Priority
- RR FCFS
- RR SRTN
- RR Priority

## Executing:

Compile scheduler.c and cpu_emulator.c

```
gcc scheduler.c -o scheduler
gcc cpu_emulator.c -o cpu_emulator
```

To run a specific scheduling algorithm. it must be ran with proper arguments

```
To run FCFS:
./scheduler FCFS
./cpu_emulator FCFS

To run SJN:
./scheduler SJN
./cpu_emulator SJN

To run Priority:
./scheduler PRIORITY
./cpu_emulator PRIORITY

To run RR FCFS with specific time quantum N:
./scheduler RR FCFS N
./cpu_emulator RR FCFS

To run RR SRTN with specific time quantum N:
./scheduler RR SRTN N
./cpu_emulator RR SRTN

To run RR Priority with specific time quantum N:
./scheduler RR PRIORITY N
./cpu_emulator RR PRIORITY
```

