#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "process.h"

void contextSwitch(Scheduler *s) {
    if (!s->head && !s->running) {
        printf("No processes available.\n");
        return;
    }

    // Round Robin
    if (s->running && s->currentAlgo == RR) {
        int start_time = s->current_time;
        int run_time = (s->running->remaining > TIME_QUANTUM) ? TIME_QUANTUM : s->running->remaining;
        s->running->remaining -= run_time;
        s->current_time += run_time;

        s->gantt[s->ganttIndex++] = (GanttEntry){s->running->pid, start_time, s->current_time};

        if (s->running->remaining > 0) {
            requeue(s, s->running);
            s->running = NULL;
        } else {
            s->running->completion = s->current_time;
            s->running->next = s->completed;
            s->completed = s->running;
            s->running = NULL;
        }
    }

    // Pick next process
    s->running = pickNext(s);
    if (!s->running) return;

    printf("Running P%d (Burst=%d, Remaining=%d, Priority=%d)\n",
           s->running->pid, s->running->burst, s->running->remaining, s->running->priority);

    if (s->currentAlgo != RR) {
        int start_time = s->current_time;
        s->current_time += s->running->burst;
        s->running->completion = s->current_time;
        s->running->remaining = 0;

        s->gantt[s->ganttIndex++] = (GanttEntry){s->running->pid, start_time, s->current_time};
        printf("P%d finished.\n", s->running->pid);

        s->running->next = s->completed;
        s->completed = s->running;
        s->running = NULL;
    }
}

void calculateMetrics(Scheduler *s) {
    int total_wait = 0, total_turnaround = 0, count = 0;
    Process *t = s->completed;

    while (t) {
        t->turnaround = t->completion;
        t->waiting = t->turnaround - t->burst;
        total_wait += t->waiting;
        total_turnaround += t->turnaround;
        count++;
        t = t->next;
    }

    s->avg_wait = count ? (float)total_wait / count : 0;
    s->avg_turnaround = count ? (float)total_turnaround / count : 0;
}

void displayGanttChart(const Scheduler *s) {
    if (s->ganttIndex == 0) {
        printf("\n--- Gantt Chart ---\n(No processes have been executed yet!)\n");
        return;
    }

    printf("\n--- Gantt Chart ---\n");
    for (int i = 0; i < s->ganttIndex; i++) {
        printf("| P%d (%d-%d) ", s->gantt[i].pid, s->gantt[i].start, s->gantt[i].end);
    }
    printf("|\nTime: ");
    for (int i = 0; i < s->ganttIndex; i++) {
        printf("%d\t", s->gantt[i].start);
    }
    printf("%d\n", s->gantt[s->ganttIndex - 1].end);
}

void autoSimulate(Scheduler *s) {
    Process *backup = NULL, *tail = NULL, *t = s->head;
    while (t) {
        Process *newP = malloc(sizeof(Process));
        *newP = *t;
        newP->next = NULL;
        if (!backup) backup = newP;
        else tail->next = newP;
        tail = newP;
        t = t->next;
    }

    resetProcesses(s);
    s->current_time = 0;
    s->ganttIndex = 0;
    s->completed = NULL;

    while (s->head) contextSwitch(s);

    calculateMetrics(s);

    // Free backup safely
    while (backup) {
        Process *tmp = backup;
        backup = backup->next;
        free(tmp);
    }

    s->head = NULL;
    s->running = NULL;
    s->completed = NULL;
    fflush(stdout);
}

void compareAlgorithms(Scheduler *s) {
    float best_wait = 1e9;
    int best_algo = FCFS;

    printf("\n=== Algorithm Comparison ===\n");

    Process *original = NULL, *tail = NULL, *t = s->head;
    while (t) {
        Process *newP = malloc(sizeof(Process));
        *newP = *t;
        newP->next = NULL;
        if (!original) original = newP;
        else tail->next = newP;
        tail = newP;
        t = t->next;
    }

    for (int algo = FCFS; algo <= RR; algo++) {
        Process *copy = NULL, *tail2 = NULL, *temp = original;
        while (temp) {
            Process *newP = malloc(sizeof(Process));
            *newP = *temp;
            newP->next = NULL;
            if (!copy) copy = newP;
            else tail2->next = newP;
            tail2 = newP;
            temp = temp->next;
        }

        s->head = copy;
        s->currentAlgo = algo;
        s->completed = NULL;
        s->ganttIndex = 0;
        s->current_time = 0;

        printf("\n----------------------------------------\n");
        printf("  Running Algorithm: %s\n",
               algo == FCFS ? "FCFS (First Come First Serve)" :
               algo == PRIORITY ? "Priority Scheduling" :
               "Round Robin (Time Quantum = 3)");
        printf("----------------------------------------\n");

        autoSimulate(s);

        displayGanttChart(s);
        printf("Average Waiting Time: %.2f\n", s->avg_wait);
        printf("Average Turnaround Time: %.2f\n", s->avg_turnaround);

        if (s->avg_wait < best_wait) {
            best_wait = s->avg_wait;
            best_algo = algo;
        }

        while (s->head) {
            Process *tmp = s->head;
            s->head = s->head->next;
            free(tmp);
        }
    }

    printf("\n========================================\n");
    printf(" Best Algorithm: %s (Lowest Avg Waiting Time)\n",
           best_algo == FCFS ? "FCFS" :
           best_algo == PRIORITY ? "Priority" : "Round Robin");
    printf("========================================\n");

    while (original) {
        Process *tmp = original;
        original = original->next;
        free(tmp);
    }

    s->head = NULL;
    s->completed = NULL;
    s->running = NULL;
    s->current_time = 0;
    s->ganttIndex = 0;

    fflush(stdout);
    while (getchar() != '\n' && !feof(stdin));
}

void showStats(const Scheduler *s) {
    printf("\n--- Process Stats ---\n");
    if (!s->head) {
        printf("No processes available.\n");
        return;
    }
    printf("Algorithm: %s\n", 
           s->currentAlgo == FCFS ? "FCFS" : 
           s->currentAlgo == PRIORITY ? "Priority" : "Round Robin");
    printf("PID\tBurst\tPriority\n");
    Process *t = s->head;
    while (t) {
        printf("%d\t%d\t%d\n", t->pid, t->burst, t->priority);
        t = t->next;
    }
}

void changeAlgo(Scheduler *s) {
    int choice;
    printf("1.FCFS  2.Priority  3.Round Robin\nChoose: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
        printf("Invalid choice.\n");
        while (getchar() != '\n');
        return;
    }
    s->currentAlgo = choice;
    printf("Algorithm changed.\n");
}

void printMenu() {
    printf("\n----------------------------------------\n");
    printf("          PROCESS SCHEDULER MENU        \n");
    printf("----------------------------------------\n");
    printf("1. Create Process\n");
    printf("2. Delete Process\n");
    printf("3. Show Stats\n");
    printf("4. Manual Context Switch\n");
    printf("5. Auto Simulate (Run all)\n");
    printf("6. Change Algorithm\n");
    printf("7. Compare Algorithms (Best)\n");
    printf("8. View Gantt Chart\n");
    printf("9. Exit\n");
    printf("----------------------------------------\n");
    printf("Enter your choice: ");
}