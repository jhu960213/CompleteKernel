//
//  IOSCHEDUELER.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-04-13.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#include "IOSCHEDULER.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// IO queue data structure
struct IONode {
    char *data[20];
    struct PCB *ptr;
    int cmd;
    struct IONode *next;
};

// IO queue head and tail pointers
struct IONode *IOHead;
struct IONode *IOTail;

// Init head and tail pointes to IO queue
void initIOQ() {
    IOHead = NULL;
    IOTail = NULL;
}

// When read or write function has been called in interpreter then call this function to add it to IO queue
char *IOscheduler(char *data[], struct PCB *ptr, int cmd) {
    addLastIOQ(data, ptr, cmd);
    return NULL;
}

// Add last to the IO queue
void addLastIOQ(char *inputData[], struct PCB *ptr, int cmd) {
    
    // Mallocing for a new node and setting the node atrributes to the input args
    struct IONode *newNode = (struct IONode*) malloc(sizeof(struct IONode));
    newNode->cmd = cmd;
    for (int i = 2; i < 20; i++) {
        newNode->data[i-2] = inputData[i];
    }
    newNode->ptr =ptr;
    printf("\nNew IO Node has been malloced\n");
    
    // Adding to head or last based on if the queue is empty or not
    if (IOHead == NULL) {
        IOHead = newNode;
        IOTail = newNode;
        printf("\nAdded to head of IO Queue!\n\n");
        newNode->next = NULL;
    } else {
        IOTail->next = newNode;
        IOTail = IOTail->next;
        newNode->next = NULL;
        printf("\nAdded to the back of IO Queue!\n\n");
    }
}

// Remove the first node of the IO queue
struct IONode* removeFirstIO() {
    if(IOHead == NULL && IOTail == NULL) {
        printf("\nError! IO Queue is already empty!\n");
        return NULL;
    }
    struct IONode *returnedNode = IOHead;
    printf("\nHead of IONode returned!\n");
    IOHead = IOHead->next;
    if (IOHead == NULL) {
        IOHead = NULL;
        IOTail = NULL;
        printf("IO queue is now empty!");
    }
    return returnedNode;
}


