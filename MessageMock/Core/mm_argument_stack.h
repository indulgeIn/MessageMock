//
//  mm_argument_stack.h
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/10.
//

#ifndef mm_argument_stack_h
#define mm_argument_stack_h

#include <stdio.h>

typedef struct {
    uintptr_t target;
    uintptr_t selector;
    uintptr_t lr;
} MMArgumentItem;

void MMThreadKeyCreate(void);

void MMArgumentStackPush(MMArgumentItem item);

MMArgumentItem MMArgumentStackPop(void);

#endif /* mm_argument_stack_h */
