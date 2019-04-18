//
//  memorymanager.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-03-17.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef memorymanager_h
#define memorymanager_h

#include <stdio.h>
#include "pcb.h"
#include <math.h>

int launcher(FILE *ptr, char *fileName);
int countTotalPages(FILE *input);
FILE *findPage(int pageNum, FILE *input);
int findFrame(void);
int findVictim(struct PCB *p);
void updateFrame(int frameNum, int victimFrame, FILE *page);
void updatePageTable(struct PCB *p, int pageNum, int frameNum, int victimFrame, bool isVictim);



#endif /* memorymanager_h */
