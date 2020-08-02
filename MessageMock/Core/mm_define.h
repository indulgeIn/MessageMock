//
//  mm_define.h
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/10.
//

#ifndef mm_define_h
#define mm_define_h

#define MM_LIKELY(x)   (__builtin_expect(!!(x), 1))
#define MM_UNLIKELY(x) (__builtin_expect(!!(x), 0))

#define MM_ASSUME_NONNULL_BEGIN _Pragma("clang assume_nonnull begin")
#define MM_ASSUME_NONNULL_END   _Pragma("clang assume_nonnull end")

#endif /* mm_define_h */
