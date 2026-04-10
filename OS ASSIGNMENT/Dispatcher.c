// Dispatcher.c

#include <stdio.h>   // For input/output functions like printf
#include <stdlib.h>  // For dynamic memory allocation (malloc, free)
#include "Process.h" // Custom header defining PCB struct and process states

// Maximum size of the ready queue
const int READYQUEUESIZE = 10;

/*
 * FCFS (First Come First Serve) Scheduling Algorithm
 * Returns the index of the process that arrived earliest.
 * If arrival times are equal, it breaks ties using the smallest PID.
 */
int FCFS(PCB* queue, int length){
    int index = -1; // Stores index of selected process

    for(int i=0; i < length; i++){
        if(index < 0){
            // First process encountered
            index = i;
        }
        else if(queue[i].arrivalTime < queue[index].arrivalTime){
            // Found a process that arrived earlier
            index = i;
        }
        else if(queue[i].arrivalTime == queue[index].arrivalTime &&
                queue[i].pid < queue[index].pid){
            // Same arrival time → choose process with smaller PID
            index = i;
        }
    }
    return index;
}

/*
 * SJF (Shortest Job First) Scheduling Algorithm
 * Returns the index of the process with the smallest burst time.
 * If multiple processes have the same burst time,
 * FCFS is used to break ties.
 */
int SJF(PCB* queue, int length){
    int index = -1; // Index of shortest job
    int count = 0;  // Number of processes with same shortest burst time

    for(int i=0; i < length; i++){
        if(index < 0){
            // First process
            index = i;
            count = 1;
        }
        else if(queue[i].burstTime < queue[index].burstTime){
            // Found shorter job
            index = i;
            count = 1;
        }
        else if(queue[i].burstTime == queue[index].burstTime){
            // Same shortest burst time → increment count
            count++;
        }
    }

    // If only one shortest job exists, return it
    if(count == 1){
        return index;
    }

    /*
     * If multiple processes have same shortest burst time,
     * create a temporary sub-queue to apply FCFS
     */
    int placeIndex = 0;

    // Allocate memory for sub-queue
    PCB* subQueue = (PCB*)malloc(count * sizeof(PCB));

    // Fill sub-queue with processes having equal shortest burst time
    for(int i=0; i < length; i++){
        if(queue[i].burstTime == queue[index].burstTime){
            subQueue[placeIndex] = queue[i];
            placeIndex++;
        }
    }

    // Use FCFS to choose among them
    int subIndex = FCFS(subQueue, count);

    int result = -1;

    // Find the selected process back in original queue
    for(int i=0; i < length; i++){
        if(queue[i].pid == subQueue[subIndex].pid){
            result = i;
            break;
        }
    }

    // Free allocated memory
    free(subQueue);

    return result;
}

/*
 * Removes and returns the next process to execute from the ready queue
 * Uses SJF scheduling
 */
PCB* GetNextProcess(PCB* queue, int* length){
    if(*length == 0){
        // No processes left
        return NULL;
    }

    // Get index of next process using SJF
    int index = SJF(queue, *length);

    if(index < 0){
        return NULL;
    }

    // Allocate memory for selected process (heap allocation)
    PCB* result = (PCB*)malloc(sizeof(PCB));

    // Copy selected process into result
    *result = queue[index];

    // Shift remaining processes left to fill gap
    for(int i = index; i < (*length) - 1; i++){
        queue[i] = queue[i+1];
    }

    // Reduce queue size
    (*length)--;

    return result;
}

/*
 * Dispatcher structure
 * Keeps track of scheduling state
 */
struct Dispatcher{
    int ticksPassed;           // Total time elapsed
    int currentQueueLength;    // Number of processes in ready queue
    PCB* readyQueue;           // Array of ready processes
    PCB* currentProcess;       // Process currently running
};

/*
 * Dispatch a process to CPU
 */
void Dispatch(struct Dispatcher* dispatcher, PCB* process){
    printf("Dispatching process %d\n", process->pid);

    // If a process is already running, terminate and free it
    if(dispatcher->currentProcess != NULL){
        dispatcher->currentProcess->state = TERMINATED;
        free(dispatcher->currentProcess);
    }

    // Assign new process
    dispatcher->currentProcess = process;

    // Set process state to RUNNING
    dispatcher->currentProcess->state = RUNNING;

    // Initialize remaining execution time
    dispatcher->currentProcess->remainingTime =
        dispatcher->currentProcess->burstTime;
}

/*
 * Switch CPU context to next process
 */
void SwitchContext(struct Dispatcher* dispatcher){
    // Get next process from ready queue
    PCB* nextProcess = GetNextProcess(
        dispatcher->readyQueue,
        &(dispatcher->currentQueueLength)
    );

    if(nextProcess == NULL){
        // No processes left
        printf("No more processes to dispatch\n");
        dispatcher->currentProcess = NULL;
        return;
    }else{
        printf("Switching to process %d\n", nextProcess->pid);

        // Dispatch new process
        Dispatch(dispatcher, nextProcess);
    }
}

/*
 * Simulates one CPU clock tick
 */
void TickUpdate(struct Dispatcher* dispatcher){
    // Increment global time
    dispatcher->ticksPassed++;

    if(dispatcher->currentProcess != NULL){
        // Decrease remaining execution time
        dispatcher->currentProcess->remainingTime--;

        /*
        Debug print (optional)
        printf("Tick %d: Process %d remaining time: %d\n",
               dispatcher->ticksPassed,
               dispatcher->currentProcess->pid,
               dispatcher->currentProcess->remainingTime);
        */

        // If process has finished execution
        if(dispatcher->currentProcess->remainingTime <= 0){
            printf("Process %d has finished execution\n",
                   dispatcher->currentProcess->pid);

            // Free process memory
            free(dispatcher->currentProcess);

            dispatcher->currentProcess = NULL;

            // Switch to next process
            SwitchContext(dispatcher);
        }
    }
}

/*
 * Main function: runs scheduling simulation
 */
int main(){
    // Allocate memory for ready queue
    PCB* readyQueue =
        (PCB*)malloc(READYQUEUESIZE * sizeof(PCB));

    // Initialize processes: {pid, burstTime, arrivalTime, remainingTime, state}
    readyQueue[0] = (PCB){0, 200, 0, 200, READY};
    readyQueue[1] = (PCB){1, 100, 0, 100, READY};
    readyQueue[2] = (PCB){2, 100, 0, 100, READY};
    readyQueue[3] = (PCB){3, 100, 0, 100, READY};

    // Initialize dispatcher
    struct Dispatcher myDispatcher = {
        0,          // ticksPassed
        4,          // number of processes
        readyQueue, // ready queue
        NULL        // no current process yet
    };

    // Start first process
    SwitchContext(&myDispatcher);

    // Run simulation until no processes remain
    while(myDispatcher.currentProcess != NULL){
        TickUpdate(&myDispatcher);
    }

    // Free memory
    free(readyQueue);

    printf("Simulation completed. Total ticks: %d\n",
           myDispatcher.ticksPassed);

    return 0;
}
