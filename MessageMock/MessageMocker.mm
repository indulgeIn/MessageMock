//
//  MessageMocker.m
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/11.
//

#import "MessageMocker.h"
#import "mm_method_matcher.h"
#import "mm_hook_objc_msgsend.h"
#import "mm_runtime.h"

#define MM_CHECK_START if (self->_isStarted) { return self; }

using namespace mm_method_matcher;

static void AutoCallback(const char * _Nullable type, uintptr_t arg, id callback) {
    switch (*type) {
        case 'f': {
            float tmp = *((float *)&arg);
            ((void(^)(float))callback)(tmp);
        } break;
        case 'd': {
            double tmp = *((double *)&arg);
            ((void(^)(double))callback)(tmp);
        } break;
        default: {
            ((void(^)(uintptr_t))callback)(arg);
        } break;
    }
}

static BOOL SetNextArgument(const char *type, va_list argList, uintptr_t *nextValue, void **nextObject) {
    if (!type || MMPtrLengthArgumentTypes().count(*type) == 0) {
        return NO;
    }
    switch (*type) {
        case 'f': {
#pragma clang push
#pragma clang diagnostic ignored "-Wvarargs"
            float arg = va_arg(argList, float);
#pragma clang pop
            *nextValue = *(uintptr_t *)&arg;
        } break;
        case 'd': {
            double arg = va_arg(argList, double);
            *nextValue = *(uintptr_t *)&arg;
        } break;
        case '@': {
            if (*nextObject) {  //把之前的对象清理掉
                CFRelease(*nextObject);
            }
            id arg = va_arg(argList, id);
            *nextObject = (__bridge_retained void *)arg;
        } break;
        default: {
            uintptr_t arg = va_arg(argList, uintptr_t);
            *nextValue = (uintptr_t)arg;
        } break;
    }
    return YES;
}

@implementation MessageMocker {
    MethodMatcher *_matcher;
    BOOL _isStarted;
    id _target;  //避免 target 先于 mocker 释放
    SEL _selector;
}

#pragma mark - Life Cycle

- (nullable instancetype)initWithTarget:(id)target selector:(SEL)selector {
    if (!target || !selector) {
        return nil;
    }
    self = [super init];
    if (self) {
        _target = target;
        _selector = selector;
        
        _matcher = new MethodMatcher();
        _matcher->target = (uintptr_t)target;
        _matcher->selector = (uintptr_t)selector;
        ++_matcher->reference;
    }
    return self;
}

- (void)dealloc {
    MethodMatcherLock lock;
    if (_matcher) {
        --_matcher->reference;
        if (_matcher->shouldDelete()) {
            UnsafeRemoveMethodMatcher(_matcher->target, _matcher->selector);
        }
    }
}

#pragma mark - Getters

+ (MessageMocker * _Nullable (^)(id _Nonnull, SEL _Nonnull))build {
    return ^MessageMocker *(id target, SEL selector) {
        return [[MessageMocker alloc] initWithTarget:target selector:selector];
    };
}

- (MessageMocker * _Nonnull (^)(const char * _Nonnull, ...))mockReturn {
    return ^MessageMocker *(const char *type, ...) {
        MM_CHECK_START
        
        va_list argList;
        va_start(argList, type);
        BOOL succeed = SetNextArgument(type, argList, &(self->_matcher->return_value), &(self->_matcher->return_object));
        va_end(argList);
  
        if (succeed) {
            self->_matcher->change_return = true;
        }
        return self;
    };
}

- (MessageMocker * _Nonnull (^)(unsigned int, const char * _Nonnull, ...))mockArgument {
    return ^MessageMocker *(unsigned int idx, const char *type, ...) {
        MM_CHECK_START
        
        Argument mark;
        mark.idx = idx + 2;
        
        va_list argList;
        va_start(argList, type);
        BOOL succeed = SetNextArgument(type, argList, &(mark.value), &(mark.object));
        va_end(argList);
        
        if (succeed) {
            if (!self->_matcher->arguments) {
                self->_matcher->arguments = new ArgumentList();
            }
            self->_matcher->arguments->push_back(mark);
        }
        return self;
    };
}

- (MessageMocker * _Nonnull (^)(id _Nonnull))checkReturn {
    return ^MessageMocker *(id callback) {
        MM_CHECK_START
        
        Method method = MMGetMethod(self->_target, self->_selector);
        char *type = method_copyReturnType(method);
        NSString *typeStr = nil;
        if (type) {
            typeStr = [NSString stringWithUTF8String:type];
            free(type);
        }
        
        self->_matcher->return_callback = ^(uintptr_t arg, ReturnCallbackError error) {
            if (error == kReturnCallbackErrorNone && typeStr.length > 0) {
                AutoCallback(typeStr.UTF8String, arg, callback);
            }
        };
        return self;
    };
}

- (MessageMocker * _Nonnull (^)(unsigned int, id _Nonnull))checkArgument {
    return ^MessageMocker *(unsigned int idx, id callback) {
        MM_CHECK_START
        
        unsigned int realIdx = idx + 2;
        
        Method method = MMGetMethod(self->_target, self->_selector);
        char *type = method_copyArgumentType(method, realIdx);
        NSString *typeStr = nil;
        if (type) {
            typeStr = [NSString stringWithUTF8String:type];
            free(type);
        }
        
        ArgumentCallback insideCallback = ^(uintptr_t arg, ArgumentCallbackError error) {
            if (error == kArgumentCallbackErrorNone && typeStr.length > 0) {
                AutoCallback(typeStr.UTF8String, arg, callback);
            }
        };
        if (!self->_matcher->argument_callback_map) {
            self->_matcher->argument_callback_map = new ArgumentCallbackMap();
        }
        self->_matcher->argument_callback_map->insert(std::make_pair(realIdx, insideCallback));
        return self;
    };
}

- (MessageMocker * _Nonnull (^)(BOOL))skip {
    return ^MessageMocker *(BOOL skip) {
        MM_CHECK_START
        
        self->_matcher->skip = skip;
        return self;
    };
}

- (MessageMocker * _Nonnull (^)(NSInteger))limitHitCount {
    return ^MessageMocker *(NSInteger count) {
        MM_CHECK_START
        
        self->_matcher->limit_hit_count = count;
        return self;
    };
}

- (long)hitCount {
    return self->_matcher->hit_count;
}

- (void (^)())start {
    return ^void() {
        if (self->_isStarted) {
            return;
        }
        self->_isStarted = YES;
        
        AddMethodMatcher(self->_matcher);
        MMStartWorking();
    };
}

@end

