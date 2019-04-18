//
//  IOSCHEDUELER.h
//  Kernel_Process
//
//  Created by Jack Hu on 2019-04-13.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#ifndef IOSCHEDUELER_h
#define IOSCHEDUELER_h

#include <stdio.h>
#include "pcb.h"

void initIOQ(void);
char *IOscheduler(char *data[],struct PCB *ptr, int cmd);
void addLastIOQ(char *data[], struct PCB *ptr, int cmd);
struct IONode* removeFirstIO(void);

#endif /* IOSCHEDUELER_h */
