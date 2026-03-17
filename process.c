#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "scheduler.h"

void createProcess(Scheduler *s) {
    Process *p = (Process *)malloc(sizeof(Process)); 
    if (!p) {
        printf("Memory allocation failed.\n");
        return;
    }

    p->pid = s->pid_counter++;
    printf("Enter burst time: ");
    if (scanf("%d", &p->burst) != 1 || p->burst <= 0) {
        printf("Invalid burst time.\n");
        free(p);
        s->pid_counter--;
        while (getchar() != '\n');
        return;
    }

    printf("Enter priority (lower = higher priority): ");
    if (scanf("%d", &p->priority) != 1 || p->priority < 0) {
        printf("Invalid priority.\n");
        free(p);
        s->pid_counter--;
        while (getchar() != '\n');
        return;
    }

    p->remaining = p->burst;
    p->waiting = 0;
    p->turnaround = 0;
    p->completion = 0;
    p->next = NULL;

    // Add process to end of list
    if (!s->head)
        s->head = p;
    else {
        Process *t = s->head;
        while (t->next) t = t->next;
        t->next = p;
    }

    printf("Process %d created successfully.\n", p->pid);
    fflush(stdout);
}

void deleteProcess(Scheduler *s) {
    int id;
    printf("Enter PID to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }

    Process *t = s->head, *prev = NULL;
    while (t && t->pid != id) {
        prev = t;
        t = t->next;
    }

    if (!t) {
        printf("Process not found.\n");
        return;
    }

    if (!prev) s->head = s->head->next;
    else prev->next = t->next;
    free(t);

    printf("Process %d deleted.\n", id);
    fflush(stdout);
}
Process* pickNext(Scheduler *s) {
    if (!s->head) return NULL;

    Process *t = s->head, *prev = NULL;
    Process *best = s->head, *bestPrev = NULL;

    // Priority Scheduling
    if (s->currentAlgo == PRIORITY) {
        int min = s->head->priority;
        while (t) {
            if (t->priority < min) {
                min = t->priority;
                best = t;
                bestPrev = prev;
            }
            prev = t;
            t = t->next;
        }

        // Remove the best from the ready queue
        if (bestPrev) bestPrev->next = best->next;
        else s->head = s->head->next;
        return best;
    }

    // FCFS (default)
    best = s->head;
    s->head = s->head->next;
    return best;
}

void requeue(Scheduler *s, Process *p) {
    p->next = NULL;
    if (!s->head) s->head = p;
    else {
        Process *t = s->head;
        while (t->next) t = t->next;
        t->next = p;
    }
}

void resetProcesses(Scheduler *s) {
    Process *t = s->head;
    while (t) {
        t->remaining = t->burst;
        t->waiting = 0;
        t->turnaround = 0;
        t->completion = 0;
        t = t->next;
    }

    s->current_time = 0;
    s->ganttIndex = 0;
    s->completed = NULL;
}
