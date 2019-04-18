//
//  cpu.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef cpu_h
#define cpu_h

#include <stdio.h>

// Instantiating CPU "object"
struct CPU {
    FILE *IP;
    int offset;
    char instructionReg[1000];
    int quanta;
};

struct CPU* makeCPU(void);
struct CPU* run(struct CPU* cpu);

#endif /* cpu_h */
