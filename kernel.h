//
//  kernel.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef kernel_h
#define kernel_h

#include <stdio.h>


void myInit(FILE *p, char *pcbName, int maxPages);
void printPCB(void);
void scheduler(void);


#endif /* kernel_h */
