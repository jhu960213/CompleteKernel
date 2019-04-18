//
//  memorymanager.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-03-17.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#include "memorymanager.h"
#include "pcb.h"
#include "dirent.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "ram.h"
#include "kernel.h"


int launcher(FILE *ptr, char *fileName) {
    
    // Open the backing store director so I could to a file within it
    DIR *bStore = opendir("BackingStore");
    // Used a tmp char to hold the fgetc of char from ptr
    char tmp;
    if (bStore == NULL) {
        printf("\nCould not open directory to copy files over.\n\n");
        return 0;
    }
    printf("\nBacking store directory opened!\n");
    char name[30] = "\0";
    strcpy(name, fileName);
    char dirPath[100] = "BackingStore/\0";
    char *concatPath = strcat(dirPath, name);
    FILE *newFile = fopen(concatPath,"w");
    tmp = fgetc(ptr);
    while(tmp != EOF) {
        fputc(tmp, newFile);
        tmp = fgetc(ptr);
    }
    tmp = '\0';
    memset(name, '\0', sizeof(name));
    printf("Successfully copied %s over!\n", fileName);
    fclose(ptr);
    fclose(newFile);
    // calls myInit by passing in the new fopened file in backing store
    printf("%s\n\n", concatPath);
    // need to use two diff file pointers since one is modeified by countPages and the other will be put into ram
    FILE *copy = fopen(concatPath, "r");
    FILE *countPages = fopen(concatPath, "r");
    if(copy == NULL && countPages == NULL) {
        printf("\nNull file pointer!\n");
        return 0;
    }
    int maxPages = countTotalPages(countPages);
    myInit(copy, fileName, maxPages);
    closedir(bStore);
    return 1;
}

int countTotalPages(FILE *input) {
    int lineCounter = 0;
    int pageNum = 0;
    char tmp;
    printf("\nStarting the count process!");
    tmp = fgetc(input);
    while(tmp != EOF) {
        if(tmp == '\n') {
            lineCounter++;
        }
        tmp = fgetc(input);
        if (tmp == EOF) {
            lineCounter++;
            printf("\nNumber of Lines: %d\n", lineCounter);
        }
    }
    tmp = '\0';
    if ((lineCounter % 4) == 0) {
        pageNum = (lineCounter/4);
        printf("Max pages needed by program: %d\n", pageNum);
    } else {
        pageNum = ((lineCounter/4) + 1);
        printf("Max pages needed by program: %d\n", pageNum);
        //printf("\n%lf\n\n", pageNum);
    }
    fclose(input);
    return pageNum;
}

FILE *findPage(int pageNum, FILE *input) {
    int pageOffset = (pageNum - 1);
    char buffer[1000];
    // Duplicating my file pointer
    FILE *fp2 = fdopen(dup (fileno(input)), "r");
    for(int i = 0; i < (4*pageOffset); i++) {
        fgets(buffer, sizeof(buffer), fp2);
    }
    memset(buffer, '\0', sizeof(buffer));
    return fp2;
}

int findFrame() {
    int ramIndex = '\0';
    for(int i = 0; i < 10; i++) {
        if (ram[i] == NULL) {
            ramIndex = i;
            break;
        }
    }
    if (ramIndex == '\0') {
        return -1;
    } else {
        return ramIndex;
    }
}

int findVictim(struct PCB *p) {
    
    int randomFrameNum = (rand() % 10);
    for(int i = 0; i < 10; i++) {
        if(p->pageTable[i] == randomFrameNum) {
            while(true) {
                bool freeFrame = true;
                for (int j = 0; j < 10; j++) {
                    if (p->pageTable[j] == randomFrameNum) {
                        freeFrame = false;
                        break;
                    }
                }
                if (freeFrame == false) {
                    randomFrameNum = ((randomFrameNum + 1) % 10);
                } else {
                    return randomFrameNum;
                }
            }
        }
    }
    return randomFrameNum;
}

void updateFrame(int frameNum, int victimFrame, FILE *page) {
    if(frameNum == -1) {
        ram[victimFrame] = page;
    } else {
        ram[frameNum] = page;
    }
}

void updatePageTable(struct PCB *p, int pageNum, int frameNum, int victimFrame, bool isVictim) {
    if(isVictim) {
        p->pageTable[pageNum] = victimFrame;
    } else {
        p->pageTable[pageNum] = frameNum;
    }
}


