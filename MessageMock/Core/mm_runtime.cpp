//
//  mm_runtime.cpp
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/11.
//

#import "mm_runtime.h"
#import <dispatch/dispatch.h>

Method MMGetMethod(id self, SEL _cmd) {
    Method method = NULL;
    if (object_isClass(self)) {
        method = class_getClassMethod((Class)self, _cmd);
    } else {
        method = class_getInstanceMethod(object_getClass(self), _cmd);
    }
    return method;
}

std::unordered_set<char> MMPtrLengthArgumentTypes() {
    static dispatch_once_t once_token;
    static std::unordered_set<char> *types;
    dispatch_once(&once_token, ^{
        types = new std::unordered_set<char>(
        {'c', 'i', 's', 'l', 'q', 'C', 'I', 'S', 'L',
         'Q', 'f', 'd', 'B', 'v', '*', '@', '#', ':', '^'});
    });
    return *types;
}

