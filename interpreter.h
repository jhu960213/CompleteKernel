//
//  interpreter.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef interpreter_h
#define interpreter_h

#include <stdio.h>
#include <math.h>
// These functions are now public
int quit(void);
int help(void);
int set(char *words[]);
int print(char *words[]);
int script(char *words[]);
int exec(char *words[]);
int Mount(char *words[]);
int Write(char *words[]);
int Read(char *words[]);
int printProcessControlBlock(void);


#endif /* interpreter_h */
