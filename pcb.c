//
//  pcb.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#include "pcb.h"
#include <stdlib.h>
#include <stdbool.h>
#include "memorymanager.h"

// global transfer PCB
struct PCB *transferPCB;

// Instantiating PCB head and tail pointers
struct PCB *headPCB;
struct PCB *tailPCB;

// Initializes the ready queue
void initReadyQueue() {
    headPCB = NULL;
    tailPCB = NULL;
}

// Mallocs a PCB for a program
struct PCB* makePCB(FILE *p, char *name, int maxPages, int frameNum, int pageNum, int offset) {
    struct PCB *newPCB = (struct PCB*) malloc(sizeof(struct PCB));
    for (int i = 0; i < 10; i++) {
        newPCB->pageTable[i] = '\0';
    }
    newPCB->programCounter = p;
    newPCB->pages_max = maxPages;
    newPCB->PC_page = pageNum;
    newPCB->pageTable[(newPCB->PC_page) - 1] = frameNum;
    newPCB->PC_offset = offset;
    newPCB->name = name;
    return newPCB;
}

// Making a transfer PCB reference 
void makeTransferPCB() {
    transferPCB = (struct PCB*) malloc(sizeof(struct PCB));
    printf("Transfer PCB has been malloced!\n");
}

// Checks to see if ready is empty or not for kernel to know when to stop 
bool isReadyQueueEmpty() {
    if (headPCB == NULL && tailPCB == NULL) {
        return true;
    } else {
        return false;
    }
}

// Adds PCB to the end of queue (as linkedlist)
void enqueue(struct PCB* pcb) {
    
    // If queue is initially empty then head & tail are both pointed at the first pcb passed in
    if (headPCB == NULL) {
        headPCB = pcb;
        tailPCB = pcb;
        printf("\nPCB: %s added to ready queue.\n",headPCB->name);
        tailPCB->next = NULL;
    } else {
        tailPCB->next = pcb;
        tailPCB->next->next = NULL;
        // The tail pointer is always pointing to the last PCB at the end of the queue
        tailPCB = tailPCB->next;
        printf("\nPCB: %s added to ready queue.\n", tailPCB->name);
    }
    
}

// Dequeueing from the head of the linkedlist and increments the head pointer to pointing to next pcb in = NULL line
// The first pcb that head was originally pointed to gets returned
struct PCB* dequeue() {
    // On the next iteration of the dequeue after queue has been emptied out, it will just return
    // the temp is just a place holder for the prgram to stop execution at a specific point
    if (headPCB == NULL && tailPCB == NULL) {
        printf("\nError! Ready is already empty!\n");
        return NULL;
    }
    struct PCB* returnedPCB = headPCB;
    printf("PCB: %s dequeued from ready.\n", returnedPCB->name);
    headPCB = headPCB->next;
    if (headPCB == NULL) {
        headPCB = NULL;
        tailPCB = NULL;
        printf("Ready queue is now empty!\n\n");
    }
    return returnedPCB;
}


