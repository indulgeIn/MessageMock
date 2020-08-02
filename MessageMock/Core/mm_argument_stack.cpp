//
//  mm_argument_stack.cpp
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/10.
//

#import "mm_argument_stack.h"
#import <pthread.h>
#import <stdlib.h>
#import "mm_define.h"

typedef struct {
    MMArgumentItem *items;
    int index;
    int mask;
} MMArgumentStack;

static pthread_key_t argument_stack_thread_key;

static void ArgumentStackThreadRelease(void *ptr) {
    MMArgumentStack *stack = (MMArgumentStack *)ptr;
    if (MM_LIKELY(stack)) {
        if (MM_LIKELY(stack->items)) {
            free(stack->items);
        }
        free(stack);
    }
}

void MMThreadKeyCreate(void) {
    pthread_key_create(&argument_stack_thread_key, &ArgumentStackThreadRelease);
}

static MMArgumentStack *GetArgumentStack() {
    MMArgumentStack *stack = (MMArgumentStack *)pthread_getspecific(argument_stack_thread_key);
    if (MM_UNLIKELY(!stack)) {
        stack = (MMArgumentStack *)malloc(sizeof(MMArgumentStack));
        stack->mask = 128;
        stack->index = -1;
        stack->items = (MMArgumentItem *)calloc(stack->mask, sizeof(MMArgumentItem));
        pthread_setspecific(argument_stack_thread_key, stack);
    }
    return stack;
}

void MMArgumentStackPush(MMArgumentItem item) {
    MMArgumentStack *stack = GetArgumentStack();
    int index = ++stack->index;
    if (MM_UNLIKELY(index >= stack->mask)) {
        stack->mask += 128;
        stack->items = (MMArgumentItem *)realloc(stack->items, stack->mask * sizeof(MMArgumentItem));
    }
    stack->items[index] = item;
}

MMArgumentItem MMArgumentStackPop() {
    MMArgumentStack *stack = GetArgumentStack();
    MMArgumentItem item = stack->items[stack->index--];
    return item;
}
