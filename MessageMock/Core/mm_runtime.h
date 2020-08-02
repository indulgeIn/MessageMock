//
//  mm_runtime.h
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/11.
//

#ifndef mm_runtime_h
#define mm_runtime_h

#include <stdio.h>
#include <objc/runtime.h>
#include <unordered_set>

Method MMGetMethod(id self, SEL _cmd);

/// 小于等于指针长度的类型前缀集合
std::unordered_set<char> MMPtrLengthArgumentTypes();

#endif /* mm_runtime_h */
