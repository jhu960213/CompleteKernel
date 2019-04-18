//
//  pcb.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef pcb_h
#define pcb_h

#include <stdio.h>
#include <stdbool.h>


// Instantiating PCB "object" & LinkedListNode of type PCB, stores a file ptr and name
struct PCB {
    // Current position in file
    FILE *programCounter;
    // Index of this array is the page number and value stored at each index is the frame number or the index of ram
    int pageTable[10];
    // This tracks which page and offset the program is currently at, and total number of pages in this program
    int PC_page;
    int PC_offset;
    int pages_max;
    struct PCB *next;
    char *name;
};

// Global transfer PCB
extern struct PCB *transferPCB;

// Method instantiations that are "public"
void initReadyQueue(void);
struct PCB* makePCB(FILE *p, char *name, int maxPages, int frameNum, int pageNum, int offset);
void makeTransferPCB(void);
void enqueue(struct PCB* pcb);
struct PCB* dequeue(void);
bool isReadyQueueEmpty(void);

#endif /* pcb_h */
