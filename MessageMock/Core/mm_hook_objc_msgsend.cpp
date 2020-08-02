//
//  mm_hook_objc_msgsend.cpp
//  MessageMock
//
//  Created by 波儿菜 on 2020/6/27.
//

#import "mm_hook_objc_msgsend.h"

#ifdef __aarch64__

#import <objc/objc.h>
#import <dispatch/dispatch.h>
#import <stdio.h>
#import "mm_fishhook.h"
#import "mm_method_matcher.h"
#import "mm_argument_stack.h"
#import "mm_argument_callback.h"
#import "mm_argument_change.h"
#import "mm_runtime.h"

using namespace mm_method_matcher;

#pragma mark - Hook Functions

static id (*origin_objc_msgSend)(id, SEL, ...);

static void beforeObjcMsgSend(uintptr_t self, uintptr_t _cmd) {
    uintptr_t lr = 0;
    __asm volatile ("mov %0, x9" : "=r"(lr));
    {  // 必须是第一个调用，保证寄存器不会被修改
        MMArgumentCallbackIfNeeded(self, _cmd);
    }
    bool skip = false;
    {
        MMArgumentItem item = {
            .target = self,
            .selector = _cmd,
            .lr = lr,
        };
        MMArgumentStackPush(item);
        {
            MethodMatcherLock lock;
            MethodMatcher *matcher = UnsafeGetMethodMatcher(self, _cmd);
            if (matcher) {
                skip = matcher->skip;
            }
        }
    }
    {  // 必须是最后一个调用，保证寄存器不会被修改
        MMSetArgumentRegisters(self, _cmd);
    }
    __asm volatile ("mov w1, %w0" :: "r"(skip));
}

static void AfterObjcMsgSend() {
    uintptr_t lr = 0, x0 = 0, d0 = 0;
    __asm volatile ("mov %0, x0\n"
                    "fmov %1, d0\n"
                    : "=r"(x0), "=r"(d0));
    uintptr_t res_type = 0, res_value = 0;
    {
        MMArgumentItem item = MMArgumentStackPop();
        lr = item.lr;
        
        bool change_return = false;
        void *return_object = NULL;
        uintptr_t return_value = 0;
        ReturnCallback return_callback = NULL;
        {
            MethodMatcherLock lock;
            MethodMatcher *matcher = UnsafeGetMethodMatcher(item.target, item.selector);
            if (matcher) {
                change_return = matcher->change_return;
                return_object = matcher->return_object;
                return_value = matcher->return_value;
                return_callback = matcher->return_callback;
            }
        }
        
        if (change_return || return_callback) {
            Method method = MMGetMethod((id)item.target, (SEL)item.selector);
            char *type = method_copyReturnType(method);
            if (MM_LIKELY(type)) {
                if (change_return) {
                    res_type = (*type == 'f' || *type == 'd') ? 0B10 : 0B1;
                    res_value = return_object ? (uintptr_t)return_object : return_value;
                }

                if (return_callback) {
                    ReturnCallbackError error = kReturnCallbackErrorNone;
                    uintptr_t callback_value = 0;
                    if (MMPtrLengthArgumentTypes().count(*type) == 0) {
                        error = kReturnCallbackErrorNotX0D0;
                    } else if (*type == 'v') {
                        error = kReturnCallbackErrorNoReturn;
                    } else {
                        callback_value = res_type > 0 ? res_value : ((*type == 'f' || *type == 'd') ? d0 : x0);
                    }
                    return_callback(callback_value, error);
                }
                
                free(type);
            }
        }
        
        {   // 尝试删除 matcher
            MethodMatcherLock lock;
            MethodMatcher *matcher = UnsafeGetMethodMatcher(item.target, item.selector);
            if (matcher) {
                ++matcher->hit_count;
                --matcher->using_count;
                if (matcher->shouldDelete()) {
                    UnsafeRemoveMethodMatcher(matcher->target, matcher->selector);
                }
            }
        }
    }
    __asm volatile
    ("mov x0, %0\n"
     "mov x1, %1\n"
     "mov x2, %2\n"
    :: "r"(lr), "r"(res_type), "r"(res_value));
}

#define BLR(func) \
__asm volatile ("stp x8, x9, [sp, #-16]!"); \
__asm volatile ("mov x12, %0" :: "r"(func)); \
__asm volatile ("ldp x8, x9, [sp], #16"); \
__asm volatile ("blr x12");

#define SAVE_REGISTERS() \
__asm volatile ( \
"stp x8, x9, [sp, #-16]!\n" \
"stp x6, x7, [sp, #-16]!\n" \
"stp x4, x5, [sp, #-16]!\n" \
"stp x2, x3, [sp, #-16]!\n" \
"stp x0, x1, [sp, #-16]!\n" \
                            \
"stp q8, q9, [sp, #-32]!\n" \
"stp q6, q7, [sp, #-32]!\n" \
"stp q4, q5, [sp, #-32]!\n" \
"stp q2, q3, [sp, #-32]!\n" \
"stp q0, q1, [sp, #-32]!\n");

#define LOAD_REGISTERS() \
__asm volatile ( \
"ldp q0, q1, [sp], #32\n" \
"ldp q2, q3, [sp], #32\n" \
"ldp q4, q5, [sp], #32\n" \
"ldp q6, q7, [sp], #32\n" \
"ldp q8, q9, [sp], #32\n" \
                          \
"ldp x0, x1, [sp], #16\n" \
"ldp x2, x3, [sp], #16\n" \
"ldp x4, x5, [sp], #16\n" \
"ldp x6, x7, [sp], #16\n" \
"ldp x8, x9, [sp], #16\n");

#define MARK_FRAME() \
__asm volatile( \
".cfi_def_cfa w29, 16\n" \
".cfi_offset w30, -8\n" \
".cfi_offset w29, -16\n");

#define RET() __asm volatile ("ret");

__attribute__((__naked__))
static void ObjcMsgSend() {
    MARK_FRAME()
    SAVE_REGISTERS()
    
    __asm volatile ("mov x9, lr");
    BLR(&beforeObjcMsgSend)
    // 是否跳过原函数调用
    __asm volatile ("cbnz w1, 40f");
    // 是否修改入参
    __asm volatile ("cbz x0, 30f");
    
    // 浮点型入参修改
    __asm volatile
    ("0:"
     "and x1, x0, #0b100000000\n"
     "cbz x1, 1f\n"
     "str d0, [sp, #0]");
    __asm volatile
    ("1:"
     "and x1, x0, #0b1000000000\n"
     "cbz x1, 2f\n"
     "str d1, [sp, #16]");
    __asm volatile
    ("2:"
     "and x1, x0, #0b10000000000\n"
     "cbz x1, 3f\n"
     "str d2, [sp, #32]");
    __asm volatile
    ("3:"
     "and x1, x0, #0b100000000000\n"
     "cbz x1, 4f\n"
     "str d3, [sp, #48]");
    __asm volatile
    ("4:"
    "and x1, x0, #0b1000000000000\n"
    "cbz x1, 5f\n"
    "str d4, [sp, #64]");
    __asm volatile
    ("5:"
    "and x1, x0, #0b10000000000000\n"
    "cbz x1, 6f\n"
    "str d5, [sp, #80]");
    __asm volatile
    ("6:"
    "and x1, x0, #0b100000000000000\n"
    "cbz x1, 7f\n"
    "str d6, [sp, #96]");
    __asm volatile
    ("7:"
    "and x1, x0, #0b1000000000000000\n"
    "cbz x1, 10f\n"
    "str d7, [sp, #112]");
    // 通用型入参修改
    __asm volatile
    ("10:"
     "and x1, x0, #0b100\n"
     "cbz x1, 11f\n"
     "str x2, [sp, #176]");
    __asm volatile
    ("11:"
     "and x1, x0, #0b1000\n"
     "cbz x1, 12f\n"
     "str x3, [sp, #184]");
    __asm volatile
    ("12:"
     "and x1, x0, #0b10000\n"
     "cbz x1, 13f\n"
     "str x4, [sp, #192]");
    __asm volatile
    ("13:"
     "and x1, x0, #0b100000\n"
     "cbz x1, 14f\n"
     "str x5, [sp, #200]");
    __asm volatile
    ("14:"
     "and x1, x0, #0b1000000\n"
     "cbz x1, 15f\n"
     "str x6, [sp, #208]");
    __asm volatile
    ("15:"
     "and x1, x0, #0b10000000\n"
     "cbz x1, 30f\n"
     "str x7, [sp, #216]");
    
    __asm volatile ("30:");
    LOAD_REGISTERS()
    BLR(origin_objc_msgSend)
    SAVE_REGISTERS()
    
    __asm volatile ("40:");
    BLR(&AfterObjcMsgSend)
    // 是否修改返回值
    __asm volatile ("cbz w1, 50f");
    
    // 通用型返回值修改
    __asm volatile
    ("and x3, x1, #0b1\n"
     "cbz x3, 41f\n"
     "str x2, [sp, #160]\n");
    // 浮点型返回值修改
    __asm volatile
    ("41:"
     "and x3, x1, #0b10\n"
     "cbz x3, 50f\n"
     "str x2, [sp]\n");
    
    __asm volatile ("50:");
    __asm volatile ("mov lr, x0");
    
    LOAD_REGISTERS()
    RET()
}

#pragma mark - Public

void MMStartWorking() {
    static dispatch_once_t once_token;
    dispatch_once(&once_token, ^{
        MMThreadKeyCreate();
        
        struct mm_rebinding info = {
            .name = "objc_msgSend",
            .replacement = (void *)ObjcMsgSend,
            .replaced = (void **)&origin_objc_msgSend,
        };
        mm_rebind_symbols((struct mm_rebinding[]){info}, 1);
    });
}

#else

void MMStartWorking() {}

#endif
