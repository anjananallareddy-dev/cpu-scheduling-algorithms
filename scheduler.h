#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

#define TIME_QUANTUM 3
#define FCFS 1
#define PRIORITY 2
#define RR 3
#define MAX_GANTT 200

typedef struct {
    int pid;
    int start;
    int end;
} GanttEntry;

typedef struct Scheduler {
    Process *head;
    Process *running;
    Process *completed;   
    int currentAlgo;
    int pid_counter;
    int current_time;

    GanttEntry gantt[MAX_GANTT];
    int ganttIndex;

    float avg_wait;
    float avg_turnaround;
} Scheduler;

void contextSwitch(Scheduler *s);
void showStats(const Scheduler *s);
void changeAlgo(Scheduler *s);
void calculateMetrics(Scheduler *s);
void displayGanttChart(const Scheduler *s);
void autoSimulate(Scheduler *s);
void compareAlgorithms(Scheduler *s);
void printMenu();

#endif
