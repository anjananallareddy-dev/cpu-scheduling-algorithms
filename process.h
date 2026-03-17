#ifndef PROCESS_H
#define PROCESS_H

// Structure definition for Process
typedef struct Process {
    int pid;
    int burst;
    int remaining;
    int priority;
    int waiting;
    int turnaround;
    int completion;
    struct Process *next;
} Process;

// Forward declaration to avoid circular dependency
struct Scheduler;

// Function prototypes for process management
void createProcess(struct Scheduler *s);
void deleteProcess(struct Scheduler *s);
Process* pickNext(struct Scheduler *s);
void requeue(struct Scheduler *s, Process *p);
void resetProcesses(struct Scheduler *s);

#endif
