//
//  DISK_driver.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-04-13.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef DISK_driver_h
#define DISK_driver_h

#include <stdio.h>

// public functions that acts like the data structures API 
void initIO(void);
int testValues(void);
void freeMemory(void);
int partition(char *name, int blocksize, int totalblocks);
int mountPart(char *name);
int openfile(char *name);
int readBlock(int file, char *name, char *variable);
char *returnBlock(void);
int writeBlock(int file, char *data[], char *name);


#endif /* DISK_driver_h */
