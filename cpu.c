//
//  cpu.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#include "cpu.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// to handle what error code is returned during exec when commands have been parsed
int errorCode;

// Mallocs a CPU to run programs
struct CPU* makeCPU() {
    struct CPU *cpu = (struct CPU*) malloc(sizeof(struct CPU));
    cpu->quanta = 2;
    return cpu;
}

// Runs my CPU for a quanta = 2 (execute 2 lines of instructions)
struct CPU* run(struct CPU* cpu) {
    while(cpu->quanta != 0) {
        if (fgets(cpu->instructionReg, sizeof(cpu->instructionReg) , cpu->IP) == NULL) {
            printf("\nPage fault interrupt! Need to find the next page!\n");
            printf("OS starting page fault sequence.\n");
            cpu->offset = 0;
            cpu->quanta += 2;
            cpu->IP = NULL;
            return cpu;
        }
        cpu->offset++;
         errorCode = parse(cpu->instructionReg);
        if (errorCode == -1) {
            exit(0);
        }
        memset(cpu->instructionReg,'\0', 1000);
        cpu->quanta--;
    }
    if (cpu->offset == 4) {
        printf("\nPage fault interrupt! Need to find the next page!\n");
        printf("OS starting page fault sequence.\n");
        cpu->offset = 0;
    }
    memset(cpu->instructionReg,'\0', 1000);
    cpu->quanta+=2;
    return cpu;
}



