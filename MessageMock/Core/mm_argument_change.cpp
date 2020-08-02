//
//  mm_argument_change.cpp
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/13.
//

#import "mm_argument_change.h"

#ifdef __aarch64__

#import <unordered_map>
#import "mm_define.h"
#import "mm_runtime.h"
#import "mm_method_matcher.h"

using namespace mm_method_matcher;

void MMSetArgumentRegisters(uintptr_t self, uintptr_t _cmd) {
    uintptr_t arg_label = 0;
    
#define XS_SIZE 6
#define DS_SIZE 8
    //通用/浮点寄存器值暂存数组
    uintptr_t xs[XS_SIZE] = {0}, ds[DS_SIZE] = {0}, mask = 1;
    {
        ArgumentList *arguments = NULL;
        {
            MethodMatcherLock lock;
            MethodMatcher *matcher = UnsafeGetMethodMatcher(self, _cmd);
            if (MM_UNLIKELY(matcher)) {
                arguments = matcher->arguments;
            }
        }
        if (!arguments || arguments->size() <= 0) {
            __asm volatile ("b 16f");
        }

        Method method = MMGetMethod((id)self, (SEL)_cmd);
        unsigned int arg_number = method_getNumberOfArguments(method);

        std::unordered_map<unsigned int, uintptr_t> x_register_map, d_register_map;
        uintptr_t x_idx = 2, d_idx = 0;
        for (unsigned int i = 2; i < arg_number; ++i) {
            char *type = method_copyArgumentType(method, i);
            if (MM_UNLIKELY(!type || MMPtrLengthArgumentTypes().count(*type) == 0)) {
                // Unsupported argument type.
                __asm volatile ("b 16f");
            }
            if (*type == 'f' || *type == 'd') {
                d_register_map[i] = d_idx++;
            } else {
                x_register_map[i] = x_idx++;
            }
            free(type);
        }

        for (Argument arg : *arguments) {
            char *type = method_copyArgumentType(method, arg.idx);
            if (MM_UNLIKELY(!type)) {
                continue;
            }
            uintptr_t res_arg = arg.value;
            if (arg.object) {
                res_arg = (uintptr_t)arg.object;
            }
            if (*type == 'f' || *type == 'd') {
                uintptr_t register_idx = d_register_map[arg.idx];
                if (MM_UNLIKELY(register_idx >= DS_SIZE)) {
                    continue;
                }
                arg_label |= (mask << (8 + register_idx));
                ds[register_idx] = res_arg;
            } else {
                uintptr_t register_idx = x_register_map[arg.idx];
                if (MM_UNLIKELY(register_idx - 2 >= XS_SIZE)) {
                    continue;
                }
                arg_label |= (mask << register_idx);
                xs[register_idx - 2] = res_arg;
            }
            free(type);
        }
    }
#undef XS_SIZE
#undef DS_SIZE

    __asm volatile
    ("mov x2, %0\n"
     "mov x3, %1\n"
     "mov x4, %2\n"
     "mov x5, %3\n"
     "mov x6, %4\n"
     "mov x7, %5\n"
     :: "r"(xs[0]), "r"(xs[1]), "r"(xs[2]), "r"(xs[3]), "r"(xs[4]), "r"(xs[5]));
    __asm volatile
    ("fmov d0, %0\n"
     "fmov d1, %1\n"
     "fmov d2, %2\n"
     "fmov d3, %3\n"
     "fmov d4, %4\n"
     "fmov d5, %5\n"
     "fmov d6, %6\n"
     "fmov d7, %7\n"
     :: "r"(ds[0]), "r"(ds[1]), "r"(ds[2]), "r"(ds[3]), "r"(ds[4]), "r"(ds[5]), "r"(ds[6]), "r"(ds[7]));
    
    __asm volatile ("16:");
    __asm volatile ("mov x0, %0" :: "r"(arg_label));
}

#else

void MMSetArgumentRegisters(uintptr_t self, uintptr_t _cmd) {}

#endif
