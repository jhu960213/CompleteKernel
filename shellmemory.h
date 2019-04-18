//
//  shellmemory.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef shellmemory_h
#define shellmemory_h

#include <stdbool.h>


// defining the linkedlist "class" and its properties
struct LinkedListNode {
    char * var;
    char * value;
    struct LinkedListNode *next;
};

void initShellMemory(void);

// C function instantiations
bool addLast(char *var, char *value);
bool printValueOfDesiredVar(char *var);
// These functions are now public
void setVarString(char *inputVar, char *inputValue);
void printValue(char *desiredVar);

#endif /* shellmemory_h */
