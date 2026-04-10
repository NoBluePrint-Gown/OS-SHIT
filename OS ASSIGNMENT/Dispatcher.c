// Dispatcher.c
#include <stdio.h>
#include <stdlib.h>
#include "Process.h"

const int READYQUEUESIZE = 10;

int FCFS(PCB* queue, int length){
    int index = -1;
    for(int i=0; i < length; i++){
        if(index < 0){
            // First instance of smallest arrival time
            index = i;
        }else if(queue[i].arrivalTime < queue[index].arrivalTime){
            // Process arrived before the stored
            index = i;
        }else if(queue[i].arrivalTime == queue[index].arrivalTime && queue[i].pid < queue[index].pid){
            // processes arrived simultaneously
            index = i;
        }
    }
    return index;
}

int SJF(PCB* queue, int length){
    int index = -1;
    int count = 0;

    for(int i=0; i < length; i++){
        if(index < 0){
            index = i;
            count = 1;
        }
        else if(queue[i].burstTime < queue[index].burstTime){
            index = i;
            count = 1;
        }else if(queue[i].burstTime == queue[index].burstTime){
            count++;
        }
    }

    if(count == 1){
        return index;
    }

    int placeIndex = 0;
    PCB* subQueue = (PCB*)malloc(count * sizeof(PCB));

    for(int i=0; i < length; i++){
        if(queue[i].burstTime == queue[index].burstTime){
            subQueue[placeIndex] = queue[i];
            placeIndex++;
        }
    }

    int subIndex = FCFS(subQueue, count);
    int result = -1;

    for(int i=0; i < length; i++){
        if(queue[i].pid == subQueue[subIndex].pid){
            result = i;
            break;
        }
    }

    free(subQueue);
    return result;
}

PCB* GetNextProcess(PCB* queue, int* length){
    if(*length == 0){
        return NULL;
    }

    int index = SJF(queue, *length);

    if(index < 0){
        return NULL;
    }

    // Allocate memory for the result on the heap
    PCB* result = (PCB*)malloc(sizeof(PCB));
    *result = queue[index];

    // Shift remaining processes
    for(int i = index; i < (*length) - 1; i++){
        queue[i] = queue[i+1];
    }
    (*length)--;

    return result;
}

struct Dispatcher{
    int ticksPassed;
    int currentQueueLength;
    PCB* readyQueue;
    PCB* currentProcess;
};

void Dispatch(struct Dispatcher* dispatcher, PCB* process){
    printf("Dispatching process %d\n", process->pid);

    if(dispatcher->currentProcess != NULL){
        dispatcher->currentProcess->state = TERMINATED;
        free(dispatcher->currentProcess);
    }

    dispatcher->currentProcess = process;
    dispatcher->currentProcess->state = RUNNING;
    dispatcher->currentProcess->remainingTime = dispatcher->currentProcess->burstTime;
}

void SwitchContext(struct Dispatcher* dispatcher){
    PCB* nextProcess = GetNextProcess(dispatcher->readyQueue, &(dispatcher->currentQueueLength));
    if(nextProcess == NULL){
        printf("No more processes to dispatch\n");
        dispatcher->currentProcess = NULL;
        return;
    }else{
        printf("Switching to process %d\n", nextProcess->pid);
        Dispatch(dispatcher, nextProcess);
    }
}

void TickUpdate(struct Dispatcher* dispatcher){
    dispatcher->ticksPassed++;

    if(dispatcher->currentProcess != NULL){
        dispatcher->currentProcess->remainingTime--;
        /*
        printf("Tick %d: Process %d remaining time: %d\n",
               dispatcher->ticksPassed,
               dispatcher->currentProcess->pid,
               dispatcher->currentProcess->remainingTime);
        */
        if(dispatcher->currentProcess->remainingTime <= 0){
            printf("Process %d has finished execution\n", dispatcher->currentProcess->pid);
            free(dispatcher->currentProcess);
            dispatcher->currentProcess = NULL;
            SwitchContext(dispatcher);
        }
    }
}

int main(){
    PCB* readyQueue = (PCB*)malloc(READYQUEUESIZE * sizeof(PCB));

    readyQueue[0] = (PCB){0, 200, 0, 200, READY};
    readyQueue[1] = (PCB){1, 100, 0, 100, READY};
    readyQueue[2] = (PCB){2, 100, 0, 100, READY};
    readyQueue[3] = (PCB){3, 100, 0, 100, READY};

    struct Dispatcher myDispatcher = {0, 4, readyQueue, NULL};

    // Initial context switch to start the first process
    SwitchContext(&myDispatcher);

    while(myDispatcher.currentProcess != NULL){
        TickUpdate(&myDispatcher);
    }

    // Clean up
    free(readyQueue);
    printf("Simulation completed. Total ticks: %d\n", myDispatcher.ticksPassed);

    return 0;
}
