//
//  interpreter.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"
#include "kernel.h"
#include "ram.h"
#include "memorymanager.h"
#include "DISK_driver.h"
#include <dirent.h>
#include "IOSCHEDULER.h"
#include "DISK_driver.h"

// This function will interpret the commands & argument when passed and will modify shell memory when appropriate
int interpreter(char *words[]) {
    
    // Instantiating error code
    int errorCode = 0;
    
    // User is asking to execute one of these commands
    if (strcmp(words[0], "Mount") == 0) {
        errorCode = Mount(words);
    }
    
    // this command is to test the FAT's and partition's persistence
    else if(strcmp(words[0], "testIO") == 0) {
        errorCode = testValues();
    }
    
    else if (strcmp(words[0], "Write") == 0) {
        errorCode = Write(words);
    } else if(strcmp(words[0], "Read") == 0) {
        errorCode = Read(words);
    } else if (strcmp(words[0], "run") == 0) {
        errorCode = script(words);
    } else if (strcmp(words[0], "exec") == 0) {
        errorCode = exec(words);
    } else if (strcmp(words[0], "printPCB") == 0) {
        errorCode = printProcessControlBlock();
    } else if (strcmp(words[0], "quit") == 0) {
        errorCode = quit();
    } else if (strcmp(words[0], "help") == 0) {
        errorCode = help();
    } else if (strcmp(words[0], "set") == 0) {
        errorCode = set(words);
    } else if (strcmp(words[0], "print") == 0) {
        errorCode = print(words);
    } else {
        printf("\nUnkown Command!\n\n");
        errorCode = 0;
    }
    // returns a certain error code according to each individual command
    return errorCode;
}

// Displays all the commands this OS supports
int help() {
    printf("\n");
    printf("These are the available commands below.\n");
    printf("Command format: COMMAND ARGUMENTS \n");
    printf("1) quit: Exits/terminates the shell \n");
    printf("2) help: Display all the commands \n");
    printf("3) set: Assigns a value to shell memory \n");
    printf("4) print: Prints the STRING assigned to VAR  \n");
    printf("5) run: Executes the file SCRIPT.TXT or any other txt file \n");
    printf("6) exec: Executes concurret programs \n");
    printf("7) printPCB: Testing dequeue function in pcb.c \n");
    printf("\n");
    return 0; // error code for no error default code
}

// Quits shell with text bye and returns error code -1 for the infinite while loop to terminate
int quit() {
    printf("\nBye!\n\n");
    return (-1);
}

// checks to see if VAR already exists, if it exits then STRING overwrites the previous value assigned to VAR in shell
// memory, if VAR does not exist then creates a new variable and assign it the input STRING
int set(char *words[]) {
    setVarString(words[1], words[2]);
    printf("\nSuccess!\n\n");
    return 0;
}

// prints string assinged to VAR, checks to see if it exists, if it does then prints it, otherwise prints
// "Variable does not exist"
int print(char *words[]) {
    printValue(words[1]);
    return 0;
}

// user is asking to execute the run command which reads line by line in a text file of all the
// commands, once it gets to a newline then script closes the text file and displays the command
// prompt once more
// in the case while its reading the command and it encounters a quit command then the file closes and commandline prompt terminates
int script(char *words[]) {
    
    // to get rid of the newline char at the end of the script name
    //char *tmp = strtok(words[1], "\n");
    int errCode = 0;
    char scriptInput[1000];
    
    // in the case that the script text file does not exist, just exits the program
    FILE *file = fopen(words[1], "r");
    if (file == NULL) {
        printf("\nScript not found!\n\n");
        return errCode;
    }
    
    // reading each line of the opened text file and sending it to interpreter as commands for shell to do something
    //fgets(scriptInput, 999, file);
    while (fgets(scriptInput, sizeof(scriptInput), file) != NULL) {
        errCode = parse(scriptInput);
        if (errCode == -1) {
            fclose(file);
            return errCode;
        }
    }
    fclose(file);
    return errCode;
}

// exec command/function runs concurrent scripts and checks to see if script with
// same name has been initiated, if it has then throw error
int exec(char *words[]) {
    
    // To count how many arguments exec executes with
    // It could have "exec arg1 arg2 arg3" or "exec arg1 arg2" or "exec arg1"
    int numArgs = 0;
    int errCode = 0;
    for (int i = 1; i<=3; i++) {
        if(words[i] != NULL) {
            numArgs++;
        }
    }
    // To switch between diff versions of exec depending on how many args user wants
    printf("\nNumber of arguments: %d\n", numArgs);
    switch (numArgs) {
        case 1:
            printf("exec with 1 prog!\n\n");
            FILE *prog1 = fopen(words[1], "r");
            if (prog1 == NULL) {
                printf("\nScript not found!\n\n");
                fclose(prog1);
                return errCode;
            } else {
                //myInit(prog1, words[1]);
                launcher(prog1, words[1]);
                scheduler();
                clean();
                fclose(prog1);
                return errCode;
            }
        case 2:
            printf("exec with 2 prog!\n\n");
            // original file pointers
            FILE *prog2 = fopen(words[1], "r");
            FILE *prog3 = fopen(words[2], "r");
            // copy file pointers
            FILE *prog2Copy = fopen(words[1], "r");
            FILE *prog3Copy = fopen(words[2], "r");
        
            if (prog2 == NULL && prog3 == NULL) {
                printf("\nScript not found!\n\n");
                fclose(prog2);
                fclose(prog3);
                fclose(prog2Copy);
                fclose(prog3Copy);
                return errCode;
            } else if(prog2 != NULL && prog3 == NULL) {
                printf("\nSecond script not found!\n\n");
                launcher(prog2, words[1]);
                scheduler();
                clean();
                fclose(prog2);
                fclose(prog3);
                fclose(prog2Copy);
                fclose(prog3Copy);
                return errCode;
            } else if (prog2 == NULL && prog3 != NULL) {
                printf("\nFirst script not found!\n\n");
                launcher(prog3, words[2]);
                scheduler();
                clean();
                fclose(prog2);
                fclose(prog3);
                fclose(prog2Copy);
                fclose(prog3Copy);
                return errCode;
            } else {
                if (strcmp(words[1], words[2]) == 0) {
                    launcher(prog2, words[1]);
                    char *alias = "progCopy.txt";
                    launcher(prog2Copy, alias);
                    scheduler();
                    clean();
                    fclose(prog2);
                    fclose(prog3);
                    fclose(prog2Copy);
                    fclose(prog3Copy);
                    return errCode;
                } else {
                    launcher(prog2, words[1]);
                    launcher(prog3, words[2]);
                    scheduler();
                    clean();
                    fclose(prog2);
                    fclose(prog3);
                    fclose(prog2Copy);
                    fclose(prog3Copy);
                    return errCode;
                }
            }
            
        case 3:
            printf("exec with 3 prog!\n\n");
            // original file pointers
            FILE *progA = fopen(words[1], "r");
            FILE *progB = fopen(words[2], "r");
            FILE *progC = fopen(words[3], "r");
            // copy file pointers
            FILE *progACopy = fopen(words[1], "r");
            FILE *progACopy1 = fopen(words[1], "r");
            FILE *progBCopy = fopen(words[2], "r");
            FILE *progCCopy = fopen(words[3], "r");
                
            if (progA == NULL && progB == NULL && progC == NULL) {
                printf("Scripts not found!\n\n");
                fclose(progA);
                fclose(progB);
                fclose(progBCopy);
                fclose(progC);
                fclose(progCCopy);
                fclose(progACopy);
                fclose(progACopy1);
                return errCode;
            } else if( progA != NULL && progB == NULL && progC == NULL ) {
                printf("\nSecond & thrid scripts not found!\n");
                launcher(progA, words[1]);
                scheduler();
                clean();
                fclose(progA);
                fclose(progB);
                fclose(progBCopy);
                fclose(progC);
                fclose(progCCopy);
                fclose(progACopy);
                fclose(progACopy1);
                return errCode;
            } else if (progA != NULL && progB != NULL && progC == NULL) {
                if (strcmp(words[1], words[2]) == 0) {
                    printf("\nThrid script not found!\n");
                    launcher(progA, words[1]);
                    char *alias = "progCopy.txt";
                    launcher(progACopy, alias);
                    scheduler();
                    clean();
                    fclose(progA);
                    fclose(progB);
                    fclose(progBCopy);
                    fclose(progC);
                    fclose(progCCopy);
                    fclose(progACopy);
                    fclose(progACopy1);
                    return errCode;
                }
                printf("\nThrid script not found!\n");
                launcher(progA, words[1]);
                launcher(progB, words[2]);
                scheduler();
                clean();
                fclose(progA);
                fclose(progB);
                fclose(progBCopy);
                fclose(progC);
                fclose(progCCopy);
                fclose(progACopy);
                fclose(progACopy1);
                return errCode;
            } else if (progA != NULL && progB != NULL && progC != NULL) {
                if (strcmp(words[1], words[2]) == 0) {
                    if (strcmp(words[2], words[3]) == 0) {
                        if (strcmp(words[1], words[3]) == 0) {
                            launcher(progA, words[1]);
                            char *progAlias = "progCopy1.txt";
                            launcher(progACopy, progAlias);
                            char *progCopy = "progCopy2.txt";
                            launcher(progACopy1, progCopy);
                            scheduler();
                            clean();
                            fclose(progA);
                            fclose(progB);
                            fclose(progBCopy);
                            fclose(progC);
                            fclose(progCCopy);
                            fclose(progACopy);
                            fclose(progACopy1);
                            return errCode;
                        }
                    }
                }
                else if (strcmp(words[1], words[2]) == 0) {
                    launcher(progA, words[1]);
                    char *progAB = "progCopy.txt";
                    launcher(progACopy, progAB);
                    launcher(progC, words[3]);
                    scheduler();
                    clean();
                    fclose(progA);
                    fclose(progB);
                    fclose(progBCopy);
                    fclose(progC);
                    fclose(progCCopy);
                    fclose(progACopy);
                    fclose(progACopy1);
                    return errCode;
                }
                else if (strcmp(words[2], words[3]) == 0) {
                    launcher(progA, words[1]);
                    launcher(progB, words[2]);
                    char *progAlias = "progCopy.txt";
                    launcher(progBCopy, progAlias);
                    scheduler();
                    clean();
                    fclose(progA);
                    fclose(progB);
                    fclose(progBCopy);
                    fclose(progC);
                    fclose(progCCopy);
                    fclose(progACopy);
                    fclose(progACopy1);
                    return errCode;
                }
                else if (strcmp(words[1], words[3]) == 0) {
                    launcher(progA, words[1]);
                    launcher(progB, words[2]);
                    char *progAlias = "progCopy.txt";
                    launcher(progACopy, progAlias);
                    scheduler();
                    clean();
                    fclose(progA);
                    fclose(progB);
                    fclose(progBCopy);
                    fclose(progC);
                    fclose(progCCopy);
                    fclose(progACopy);
                    fclose(progACopy1);
                    return errCode;
                }
                launcher(progA, words[1]);
                launcher(progB, words[2]);
                launcher(progC, words[3]);
                scheduler();
                clean();
                fclose(progA);
                fclose(progB);
                fclose(progBCopy);
                fclose(progC);
                fclose(progCCopy);
                fclose(progACopy);
                fclose(progACopy1);
                return errCode;
            } else if (progA != NULL && progB == NULL && progC != NULL) {
                if (strcmp(words[1], words[3]) == 0) {
                    printf("\nSecond script not found!\n");
                    launcher(progA, words[1]);
                    char *alias = "progCopy.txt";
                    launcher(progACopy, alias);
                    scheduler();
                    clean();
                    fclose(progA);
                    fclose(progB);
                    fclose(progBCopy);
                    fclose(progC);
                    fclose(progCCopy);
                    fclose(progACopy);
                    fclose(progACopy1);
                    return errCode;
                }
                printf("\nSecond script not found!\n");
                launcher(progA, words[1]);
                launcher(progC, words[3]);
                scheduler();
                clean();
                fclose(progA);
                fclose(progB);
                fclose(progBCopy);
                fclose(progC);
                fclose(progCCopy);
                fclose(progACopy);
                fclose(progACopy1);
                return errCode;
            } else if (progA == NULL && progB != NULL && progC != NULL) {
                if (strcmp(words[2], words[3]) == 0) {
                    printf("\nFirst script not found!\n");
                    launcher(progB, words[2]);
                    char *alias = "progCopy.txt";
                    launcher(progBCopy, alias);
                    scheduler();
                    clean();
                    fclose(progA);
                    fclose(progB);
                    fclose(progBCopy);
                    fclose(progC);
                    fclose(progCCopy);
                    fclose(progACopy);
                    fclose(progACopy1);
                    return errCode;
                }
                printf("\nFirst script not found!\n");
                launcher(progB, words[2]);
                launcher(progC, words[3]);
                scheduler();
                clean();
                fclose(progA);
                fclose(progB);
                fclose(progBCopy);
                fclose(progC);
                fclose(progCCopy);
                fclose(progACopy);
                fclose(progACopy1);
                return errCode;
            }
    }
    printf("\nError! Did not find any programs!\n");
    return errCode;
}

// Linux mount cmd that mounts a particular partition, if one exists it just mounts, if not then create a partition then mount it
int Mount(char *words[]) {
    char fileName[30] = "\0";
    strcpy(fileName, words[1]);
    char dirPath[50] = "PARTITION/\0";
    char *filePath = strcat(dirPath, fileName);
    FILE *openPartition = fopen(filePath, "r");
    if (openPartition == NULL) {
        int blockSize = atoi(words[2]);
        int blockNum = atoi(words[3]);
        printf("\nPartition does not exist!\n");
        partition(words[1], blockSize, blockNum);
        printf("\nPartition has been created, now mounting partition!\n");
        mountPart(words[1]);
    } else {
        printf("\nMounting existing Partition!\n");
        mountPart(words[1]);
    }
    fclose(openPartition);
    return 0;
}

// Writes to a file in partition
int Write(char *words[]) {
    IOscheduler(words, transferPCB , 1);
    int fileCode = openfile(words[1]);
    writeBlock(fileCode, words, words[1]);
    return 0;
}

// Reads from a file in partition
int Read(char *words[]) {
    int fileCode = openfile(words[1]);
    readBlock(fileCode, words[1], words[2]);
    return 0;
}

// Added function during the ready queue testing phase 
// Testing dequeue from pcb.c
int printProcessControlBlock() {
    int errorCode = 0;
    printPCB();
    return errorCode;
}




