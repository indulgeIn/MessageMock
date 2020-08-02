//
//  mm_argument_callback.cpp
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/10.
//

#import "mm_argument_callback.h"

#ifdef __aarch64__

#import <stdlib.h>
#import "mm_define.h"
#import "mm_method_matcher.h"
#import "mm_runtime.h"

using namespace mm_method_matcher;

void MMArgumentCallbackIfNeeded(uintptr_t self, uintptr_t _cmd) {
#define XS_SIZE 6
#define DS_SIZE 8
    uintptr_t xs[XS_SIZE], ds[DS_SIZE];
    __asm volatile
    ("mov %0, x2\n"
     "mov %1, x3\n"
     "mov %2, x4\n"
     "mov %3, x5\n"
     "mov %4, x6\n"
     "mov %5, x7\n"
     : "=r"(xs[0]), "=r"(xs[1]), "=r"(xs[2]), "=r"(xs[3]), "=r"(xs[4]), "=r"(xs[5]));
    __asm volatile
    ("fmov %0, d0\n"
     "fmov %1, d1\n"
     "fmov %2, d2\n"
     "fmov %3, d3\n"
     "fmov %4, d4\n"
     "fmov %5, d5\n"
     "fmov %6, d6\n"
     "fmov %7, d7\n"
     : "=r"(ds[0]), "=r"(ds[1]), "=r"(ds[2]), "=r"(ds[3]), "=r"(ds[4]), "=r"(ds[5]), "=r"(ds[6]), "=r"(ds[7]));
    
    ArgumentCallbackMap *callback_map = NULL;
    {
        MethodMatcherLock lock;
        MethodMatcher *matcher = UnsafeGetMethodMatcher(self, _cmd);
        if (MM_UNLIKELY(matcher)) {
            callback_map = matcher->argument_callback_map;
            
            ++matcher->using_count;
        }
    }
    if (MM_LIKELY(!callback_map || callback_map->size() <= 0)) {
        return;
    }

    Method method = MMGetMethod((id)self, (SEL)_cmd);
    unsigned int arg_number = method_getNumberOfArguments(method);
    
    ArgumentCallbackError error = kArgumentCallbackErrorNone;
    unsigned int x_idx = 0, d_idx = 0;
    for (unsigned int i = 2; i < arg_number; ++i) {
        ArgumentCallback callback = callback_map->find(i)->second;
        if (!callback) {
            continue;
        }
        
        if (error == kArgumentCallbackErrorUnsupported) {
            callback(0, error);
            continue;
        }
        
        char *type = method_copyArgumentType(method, i);
        if (MM_UNLIKELY(!type)) { // 该路径理论上不会走
            callback(0, kArgumentCallbackErrorNoIndex);
            continue;
        }
        
        uintptr_t arg = 0;
        if (MM_UNLIKELY(MMPtrLengthArgumentTypes().count(*type) > 0)) {
            if (*type == 'f' || *type == 'd') {
                if (MM_UNLIKELY(d_idx >= DS_SIZE)) {
                    error = kArgumentCallbackErrorOverD7;
                } else {
                    arg = ds[d_idx++];
                }
            } else {
                if (MM_UNLIKELY(x_idx >= XS_SIZE)) {
                    error = kArgumentCallbackErrorOverX7;
                } else {
                    arg = xs[x_idx++];
                }
            }
        } else {
            error = kArgumentCallbackErrorUnsupported;
        }
        
        callback(arg, error);
        free(type);
    }
    
#undef XS_SIZE
#undef DS_SIZE
}

#else

void MMArgumentCallbackIfNeeded(uintptr_t self, uintptr_t _cmd) {}

#endif
