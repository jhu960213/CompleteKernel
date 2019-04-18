//
//  ram.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef ram_h
#define ram_h

#include <stdio.h>
#include <math.h>

// Simulates OS memory to run processes
extern FILE *ram[10];
void initRAM(void);
int addToRAM(FILE *ptr);
void clean(void);


#endif /* ram_h */
