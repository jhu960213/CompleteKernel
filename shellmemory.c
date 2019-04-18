//
//  shellmemory.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "shellmemory.h"

struct LinkedListNode *head;
struct LinkedListNode *currentNode;

void initShellMemory() {
    // to keep track of where my head and current nodes are
    head = NULL;
    currentNode = NULL;
}


// creates a new node and adds it to the end of the linkedlist storing inputed var and value
bool addLast(char *var, char *value) {
    
    // creating a new node storing the passed variable and value into this node
    struct LinkedListNode *newNode = (struct LinkedListNode*) malloc(sizeof(struct LinkedListNode));
    newNode->var = var;
    newNode->value = value;
    
    // to keep track of which node we're at from head
    struct LinkedListNode *current ;
    
    // checks to see if the linkedlist is empty, if it is empty then head pointer set to new node
    // if linkedlist isn't empty then iterate over the nodes until the next field of the current node is NULL
    // then add the newly created node there
    if (head == NULL) {
        head = newNode;
        head->next = NULL;
        printf("\nYour stored variable: %s Value: %s", head->var, head->value);
    } else {
        current = head;
        // printf("\nVariable: %s Value: %s\n", current->var, current->value);
        while (current->next != NULL) {
            if (strcmp(current->var, newNode->var) == 0) {
                current->value = newNode->value;
                printf("\nYour stored variable: %s value: %s", current->var, current->value);
                return true;
            } else {
                current = current->next;
            }
        }
        if (strcmp(current->var, newNode->var) == 0) {
            current->value = newNode->value;
            printf("\nYour stored variable: %s value: %s", current->var, current->value);
            return true;
        }
        current->next = newNode;
        printf("\nYour stored variable: %s value: %s", current->next->var, current->next->value);
        current->next->next = NULL;
    }
    return true;
}

// chopping the \n newline char at the end of the inputed variable
char* chop(char *inputChar) {
    inputChar[strcspn (inputChar, "\n")] = '\0';
    return inputChar;
}

// finds the variable in the linkedlist and prints the corresponding value
bool printValueOfDesiredVar(char *var) {
    
    // setting currentNode to the head pointer of the begining of the linkedlist
    // passing the input var with newline char "\n" into the chop function to
    // to get rid of the new line character so it can be compared with the stored var that
    // has no "\n" at the end
    currentNode = head;
    //char *choppedVar = chop(var);
    
    while (currentNode != NULL) {
        //printf("\n%s\n", currentNode->value);
        if (strcmp(currentNode->var, var) == 0) {
            printf("\nDesired variable is: %s and value is: %s\n\n", currentNode->var, currentNode->value);
            return true;
        }
        currentNode = currentNode->next;
    }
    printf("\nVariable does not exit!\n\n");
    return false;
}

// set function from interpreter.c calls this funciton which calls addLast to store the variable and value in the linkedlist
void setVarString(char *inputVar, char *inputValue) {
    addLast(inputVar, inputValue);
}

// print function from interpreter.c calls this function which calls printValueOfDesiredVar to print the desired variable and value
void printValue(char *desiredVar) {
    printValueOfDesiredVar(desiredVar);
}






































