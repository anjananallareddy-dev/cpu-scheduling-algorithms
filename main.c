#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"
#include "process.h"

void cleanup(Scheduler *s) {
    while (s->head) {
        Process *tmp = s->head;
        s->head = s->head->next;
        free(tmp);
    }
    while (s->completed) {
        Process *tmp = s->completed;
        s->completed = s->completed->next;
        free(tmp);
    }
    s->running = NULL;
}

int main() {
    Scheduler s = {0};
    s.currentAlgo = FCFS;
    s.pid_counter = 1;
    s.head = NULL;
    s.completed = NULL;
    s.running = NULL;
    s.current_time = 0;
    s.ganttIndex = 0;
    s.avg_wait = 0.0f;
    s.avg_turnaround = 0.0f;

    int choice;

    while (1) {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // clear invalid input
            continue;
        }

        switch (choice) {
            case 1:
                createProcess(&s);
                break;
            case 2:
                deleteProcess(&s);
                break;
            case 3:
                showStats(&s);
                break;
            case 4:
                contextSwitch(&s);
                break;
            case 5:
                printf("\nRunning Auto Simulation...\n");
                autoSimulate(&s);
                displayGanttChart(&s);
                break;
            case 6:
                changeAlgo(&s);
                break;
            case 7:
                compareAlgorithms(&s);
                break;
            case 8:
                displayGanttChart(&s);
                break;
            case 9:
                printf("Exiting program. Cleaning up memory...\n");
                cleanup(&s);
                printf("Cleanup done. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
                break;
        }

        // Flush any extra input before showing menu again
        fflush(stdout);
        while (getchar() != '\n' && !feof(stdin));
    }

    return 0;
}