//
//  kernel.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//


#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "kernel.h"
#include "DISK_driver.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "ram.h"
#include "pcb.h"
#include "cpu.h"
#include "shellmemory.h"
#include "memorymanager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>


// OS boot sequence function that calls all other module init functions for OS initialization
void boot() {
    printf("\nInitiating boot sequence.\n");
    initReadyQueue();
    initShellMemory();
    initRAM();
    makeTransferPCB();
    // Granting read/write/search access to everyone who has access to this folder
    // System command to delete BackingStore folder
    // For the TA who will be grading this please change the path to your desired path on your OS
    system("rm -rf BackingStore");
    system("mkdir BackingStore");
    printf("BackingStore succesfully initialized!\n");
}

// Kernel's main function, this is the root function of all process executions
int main(int argc, const char * argv[]) {
    
    // OS bootstrap initialization fucntion
    boot();
    // IO initialization
    initIO();
    // Loads kernel UI shell after all of kernel's initialization passes
    printf("\nKernel 1.0 loaded!\n");
    printf("Welcome to Jack Hu's Shell!\n");
    printf("Shell version 2.0 Updated February 2019\n\n");
    displayShellPromptAndPassCommand();
    return 0;
}


// Adds PCB to the end of Ready Queue
void addToReady(struct PCB* pcb) {
    enqueue(pcb);
}

// Testing dequeue from pcb.c, not part of this assignment
void printPCB() {
    dequeue();
}

// Kernel's initialization function to load file ptr into ram, malloc a PCB, and set file ptr to PCB's
// Program counter, load PCB into Ready Queue
// Added an extra field to makePCB to keep track of which prog I'm running 
void myInit(FILE *p, char *pcbName, int maxPages) {
    int frameNum = addToRAM(p);
    struct PCB* newPCB = makePCB(p, pcbName, maxPages, frameNum, 1, 0);
    addToReady(newPCB);
}

// Kernel's command to run all active processes in ready queue
void scheduler() {
    
    bool done = false;
    struct CPU* myCPU = makeCPU();
    struct PCB* retrievedPCB;
    while(!done) {
        if ( (myCPU->IP == NULL) || (myCPU->quanta == 2) ) {
            retrievedPCB = dequeue();
            // copy the dequeued PCB to the glob transfer PCB for I/O purposes
            transferPCB = retrievedPCB;
            myCPU->IP = retrievedPCB->programCounter;
            myCPU->offset = retrievedPCB->PC_offset;
            myCPU = run(myCPU);
            retrievedPCB->PC_offset = myCPU->offset;
            if (retrievedPCB->PC_offset == 0) {
                // increases to the next page 
                retrievedPCB->PC_page++;
                // if my current page is larger than max pages need then terminate the progream immediately
                if (!(retrievedPCB->PC_page <= retrievedPCB->pages_max)) {
                    printf("\nProgram has exceeded the max amount of virtual memory allocated (pageNum > maxPages) , OS initiating kill-9 on process!\n");
                    retrievedPCB->programCounter = NULL;
                    goto TERMINATION;
                }
                // if our frame exits in our page table
                if (retrievedPCB->pageTable[(retrievedPCB->PC_page) - 1] != '\0') {
                    FILE *foundPtr = ram[retrievedPCB->pageTable[retrievedPCB->PC_page - 1]];
                    retrievedPCB->PC_offset = 0;
                    retrievedPCB->programCounter = foundPtr;
                    goto TERMINATION;
                } else {
                    // if our frame does not exist in our page table, need to find free frame then put next page into that frame and update
                    // pcb file program counter, update page table and contine execution
                    int freeFrame = findFrame();
                    if (freeFrame == -1) {
                        retrievedPCB->programCounter = myCPU->IP;
                        goto TERMINATION;
                    }
                    ram[freeFrame] = myCPU->IP;
                    retrievedPCB->pageTable[retrievedPCB->PC_page - 1] = freeFrame;
                    retrievedPCB->PC_offset = 0;
                    retrievedPCB->programCounter = ram[freeFrame];
                }
            } else {
                retrievedPCB->programCounter = myCPU->IP;
            }
            TERMINATION: if(retrievedPCB->programCounter == NULL) {
                printf("Program: %s terminated\n\n", retrievedPCB->name);
                free(retrievedPCB);
                if (isReadyQueueEmpty() == true) {
                    done = true;
                }
            } else {
                enqueue(retrievedPCB);
            }
            
        }
    }
    free(myCPU);
}



