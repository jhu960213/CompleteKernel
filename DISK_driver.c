//
//  DISK_driver.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-04-13.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#include "DISK_driver.h"
#include "IOSCHEDULER.h"
#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

// Private data structure for storing info about the structure and format of the partition
struct PARTITION {
    int totalBlocks;
    int blockSize;
};

// File allocation table data structure of partition, stores data about each file in the partition
struct FAT {
    char *fileName;
    int fileLen;
    int blockPrts[10];
    int curLoc;
};

int fatCounter = 0;
struct FAT *fat[20];
struct PARTITION *part;



// Block buffer data structure & all system wide opened file pointers
int fileIndex = 0;
char *blockBuff;
FILE *fp[5];


// testing function to help reset the FAT counter to 0 every time mount has been called
void resetFATCounter() {
    fatCounter = 0;
}

// mallocs memory for partition and FAT
void makePartition() {
    // mallocing memoring for an array of 20 structs of type FAT
    *fat = malloc(20 * sizeof(*fat));
    for (int i = 0; i < 20; i++) {
        fat[i] = malloc(sizeof(*fat[i]));
    }
    // mallocing memory for the partition struct
    part = (struct PARTITION*) malloc(sizeof(struct PARTITION));
}


// Initializes all IO data structures and variables
void initIO() {
    
    // Reset partition directory on every run
    printf("\nResetting PARTITION directory!\n");
    system("rm -rf PARTITION");
    
    // resets fatCounter
    resetFATCounter();
    // Block buffer and open files pointer data structure initialization
    blockBuff = NULL;
    for(int i = 0; i < 5; i++) {
        fp[i] = NULL;
    }
    initIOQ();
    // FAT data structure & partition structure initializations
    for(int j = 0; j < 20; j++){
        fat[j] = NULL;
    }
    part = NULL;
    // mallocs memory for global partition and FAT data structures
    makePartition();
    printf("I/O data structures have been initiated!\n");
}

// testing persistence of the data structures 
int testValues() {
    printf("\nblock size: %d & num blocks: %d\n", part->blockSize, part->totalBlocks);
    for (int i = 0; i < 20; i++) {
        printf("\nfile name: %s & file length: %d\n\n", fat[i]->fileName, fat[i]->fileLen);
    }
    return 0;
}



// call this function to free up existing partion and FAT info before new partitions and FAT are loaded
void freeMemory() {
    free(part);
    for(int j = 0; j < 20; j++) {
        free(fat[j]);
    }
    //free(fat);
}

// Create & format partition, return success (1) or failure (0)
int partition(char *name, int blocksize, int totalblocks) {
    
    //system("rm -rf PARTITION");
    system("mkdir PARTITION");
    DIR *partDirec = opendir("PARTITION");
    
    if (partDirec == NULL) {
        printf("\nError! Could not open PARTITION directory!\n");
        return 0;
    }
    
    char fileName[30] = "\0";
    strcpy(fileName, name);
    char dirPath[50] = "PARTITION/\0";
    char *filePath = strcat(dirPath, fileName);
    printf("\nFile path is: %s\n\n", filePath);
    FILE *partData = fopen(filePath, "w");
    if (partData == NULL) {
        printf("\nCould not partition due to file!\n");
        return 0;
    }
    
    // writing partition info
    printf("\nWriting the chosen block size: %d into partition file: %s onto the first line\n", blocksize, name);
    // blocksize should be between 0-9 for this to work
    char blockS = blocksize + '0';
    fputc(blockS, partData);
    fputc('\n', partData);
    printf("Block size write was successful!\n");
    printf("\nWriting num blocks: %d into partition file: %s onto the second line\n", totalblocks, name);
    char numBlocks = totalblocks + '0';
    fputc(numBlocks, partData);
    printf("Block number write was successful!\n");
    fputc('\n', partData);
    
    // writing FAT info
    printf("Writing file FAT info...");
    fputs(name, partData);
    fputc('\n', partData);
    // writing the file length part of FAT
    char fileLength = totalblocks + '0';
    fputc(fileLength, partData);
    fputc('\n', partData);
    // writing the blockptrs initial data
    char initValue = '0';
    for (int b = 0; b < 10; b++) {
        fputc(initValue, partData);
    }
    fputc('\n', partData);
    // writing current location of block
    fputc(initValue, partData);
    fputc('\n', partData);
    // appending 0s to the data area (num bytes = num of 0s)
    int dataSize = (blocksize * totalblocks);
    for (int c = 0; c < dataSize; c++) {
        fputc('0', partData);
    }
    
    printf("\nSuccessfully wrote FAT data to: %s\n", name);
    memset(fileName, '\0', sizeof(fileName));
    memset(filePath, '\0', 80);
    fclose(partData);
    closedir(partDirec);
    printf("\n\nOS successfully partitioned an area of HDD!\n\n");
    return 1;
}

// Load FAT & createbuffer_block return success/failure
int mountPart(char *name) {
    
    // opening the partitioned directory
    DIR *partDirec = opendir("PARTITION");
    if (partDirec == NULL) {
        printf("\nCould not open the PARTITION directory!\n");
        return 0;
    }
    
    // opening the desired partition for reading
    char fileName[30] = "\0";
    strcpy(fileName, name);
    char dirPath[50] = "PARTITION/\0";
    char *filePath = strcat(dirPath, fileName);
    printf("\nFile path is: %s\n", filePath);
    FILE *desiredPart = fopen(filePath, "r");
    if (desiredPart == NULL) {
        printf("\nCould not open the desired partition file since its null!\n");
        return 0;
    }
    
    //
    // reading partition and FAT info into the global data structures
    //
    
    // reading from desired partition into part and fat[]
    char temp[20];
    int tmpCounter = 0;
    int lineCounter = 1;
    int blockPtrCounter = 0;
    
    char tmp = fgetc(desiredPart);
    printf("\nExtracting block size!\n");
    part->blockSize = (tmp - '0');
    while (tmp != EOF) {
        if (tmp == '\n') {
            lineCounter++;
        } else {
            if (lineCounter == 2) {
                printf("\nExtracting total blocks!\n");
                part->totalBlocks = (tmp - '0');
            } else if(lineCounter == 3) {
                temp[tmpCounter] = tmp;
                tmpCounter++;
            } else if (lineCounter == 4) {
                // loading file name
                printf("\nExtracting file name!\n");
                temp[tmpCounter] = '\0';
                char *name = strdup(temp);
                fat[fatCounter]->fileName = name;
                
                // loading file length
                printf("\nExtracting file length!\n");
                fat[fatCounter]->fileLen = (tmp - '0');
            } else if (lineCounter == 5) {
                printf("\nExtracting block ptrs!\n");
                fat[fatCounter]->blockPrts[blockPtrCounter] = (tmp - '0');
                blockPtrCounter++;
            } else if (lineCounter == 6) {
                printf("\nExtracting current block number!\n");
                fat[fatCounter]->curLoc = (tmp - '0');
            }
        }
        tmp = fgetc(desiredPart);
    }
    printf("\nLoaded block size: %d and total blocks: %d\n", part->blockSize, part->totalBlocks);
    printf("\nLoaded file name: %s & file length: %d & current block location: %d\n", fat[fatCounter]->fileName,
           fat[fatCounter]->fileLen, fat[fatCounter]->curLoc);
    for (int a = 0; a < 10; a++) {
        printf("\nLoaded block pointer number: %d at %dth position\n", fat[fatCounter]->blockPrts[a], a);
    }
    // mallocing memory for blockBuffer 
    blockBuff = malloc(sizeof(part->blockSize));
    printf("\nBlock Buff memory has been allocated!\n\n");
    fatCounter++;
    memset(temp, '\0', sizeof(temp));
    //freeMemory();
    fclose(desiredPart);
    return 1;
}

// Find filename or creates file if it does not exist, returns file’s FAT index
int openfile(char *name) {
    
    int freeSpot = -1;
    DIR *dir = opendir("PARTITION");
    // searching the FAT data structure for file name
    for (int i = 0; i < 20; i++) {
        if (fat[i]->fileName == name) {
            // found the file name so the partition alreay exits
            char fileName[30] = "\0";
            strcpy(fileName, name);
            char dirPath[50] = "PARTITION/\0";
            char *filePath = strcat(dirPath, fileName);
            FILE *file = fopen(filePath, "r+");
            for (int a = 0; a < 5; a++) {
                if (fp[a] == NULL) {
                    printf("\nfp[] isn't full, found a free spot\n");
                    freeSpot = a;
                    break;
                }
            }
            if (freeSpot == -1) {
                printf("\nError! fp[] is full!\n");
                return -1;
            }
            fp[fileIndex] = file;
            fileIndex = ((fileIndex + 1) % 5);
            printf("\nReturning the existing entry's index of FAT\n");
            fclose(file);
            return i;
        } else if(fat[i]->fileName == NULL) {
            // creating a new entry in the table
            fat[i]->fileName = name;
            fat[i]->fileLen = -1;
            fat[i]->curLoc = -1;
            for (int j = 0; j < 10; j++) {
                fat[i]->blockPrts[j] = -1;
            }
            fp[fileIndex] = NULL;
            printf("\nReturning the new entry's index of FAT\n");
            return i;
        }
    }
    
    // if we've went through the 20 FAT structs and we did not find a matching file name or a NULL spot then fat[] is currently full
    // so return -1
    closedir(dir);
    printf("\nfat[] is currently full!\n");
    return -1;
}

// using the file FAT index number, load buffer with data from blockID
int readBlock(int file, char *fileName, char *variable) {
    
    // all the neccessary variable instantiations to make readBlock work
    char *words[50];
    char *convertedString;
    char readBuff[100];
    int buffIndex = 0;
    char tmp;
    
    // due to openFile()
    if(file == -1) {
        printf("\nError! Could not read block, openFile() returned an invalid index!\n");
        return -1;
    }
    
    // opening the desired file
    char name[30] = "\0";
    strcpy(name, fileName);
    char dirPath[50] = "PARTITION/\0";
    char *filePath = strcat(dirPath, fileName);
    FILE *foundPtr = fopen(filePath, "r");
    
    // making sure the opened file exists
    if (foundPtr == NULL) {
        printf("File does not exist!\n\n");
        return -1;
    }
    
    // reading chars from file to the readBuffer
    tmp = fgetc(foundPtr);
    while(tmp != EOF) {
        // skipping over the newline character when reading chars
        if (tmp != '\n') {
            readBuff[buffIndex] = tmp;
            buffIndex++;
            tmp = fgetc(foundPtr);
        } else {
            tmp = fgetc(foundPtr);
        }
    }
    
    // converting from char array to char *
    readBuff[buffIndex] = '\0';
    convertedString = strdup(readBuff);
    
    // using the set command to set the *variable to the converted string
    words[1] = variable;
    words[2] = convertedString;
    
    // adding to IO queue for a read command
    IOscheduler(words, transferPCB , 0);
    
    // store the returned string value of data in shellmemory for the inputed variable 
    set(words);
    printf("\nRead block successfully completed!");
    
    // clearing char arrays, char *, setting char * to nul, etc
    memset(words,'\0', sizeof(words));
    memset(readBuff, '\0', sizeof(readBuff));
    convertedString = NULL;
    tmp = '\0';
    printf("\nClean of buffer and char * have completed! Ready for the next read block!\n\n");
    
    // clearing name and direcPath char arrays
    memset(name,'\0', sizeof(name));
    memset(dirPath, '\0', sizeof(dirPath));
    
    // closing opened for reading file pointer
    fclose(foundPtr);
    return 0;
}

// return block data as string from buffer_block
char *returnBlock() {
    
    return 0;
}


// sensitive to block size, write data to disk at blockID
int writeBlock(int file, char *data[], char *fileName) {
    
    
    
    // open the directory PARTITION
    DIR *dir = opendir("PARTITION");
    
    // due to openFile()
    if(file == -1) {
        printf("\nError! Could not read block, openFile() returned an invalid index!\n");
        return -1;
    }
    
    // open the desired file for writing if one does not exists then create one and write to it
    char name[30] = "\0";
    strcpy(name, fileName);
    char dirPath[50] = "PARTITION/\0";
    char *filePath = strcat(dirPath, fileName);
    FILE *foundPtr = fopen(filePath, "r");
    if (foundPtr != NULL) {
        fclose(foundPtr);
        FILE *existingFile = fopen(filePath, "a");
        fputc(' ', existingFile);
        fputc('\n', existingFile);
        for (int i = 2; i < 100; i++) {
            if(data[i] != NULL || data[i] != '\0') {
                // printing a string on each line of the textfile
                // after the string has been printed I add a white space and then a newline character
                if (data[i + 1] == NULL || data[i + 1] == '\0') {
                    fputs(data[i], existingFile);
                } else {
                    fputs(data[i], existingFile);
                    fputc(' ', existingFile);
                    fputc('\n', existingFile);
                }
            } else {
                fclose(existingFile);
                closedir(dir);
                printf("Successfully wrote to the desired file!\n\n");
                return 0;
            }
        }
    } else {
        // close the file reading pointer
        fclose(foundPtr);
        FILE *newFile = fopen(filePath, "w");
        for (int i = 2; i < 100; i++) {
            if(data[i] != NULL || data[i] != '\0') {
                // printing a string on each line of the textfile
                // after the string has been printed I add a white space and then a newline character
                if (data[i + 1] == NULL || data[i + 1] == '\0') {
                    fputs(data[i], newFile);
                } else {
                    fputs(data[i], newFile);
                    fputc(' ', newFile);
                    fputc('\n', newFile);
                }
            } else {
                fclose(newFile);
                closedir(dir);
                printf("Successfully wrote to the new file!\n\n");
                return 0;
            }
        }
    }
    fclose(foundPtr);
    closedir(dir);
    return 0;
}








