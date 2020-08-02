//
//  mm_method_matcher.cpp
//  MessageMock
//
//  Created by 波儿菜 on 2020/6/27.
//

#import "mm_method_matcher.h"
#import <mutex>
#import <dispatch/dispatch.h>
#import <CoreFoundation/CFBase.h>

namespace mm_method_matcher {

static pthread_mutex_t *GetMethodMatcherMutex(void) {
    static pthread_mutex_t method_matcher_lock;
    static dispatch_once_t once_token;
    dispatch_once(&once_token, ^{
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&method_matcher_lock, &attr);
        pthread_mutexattr_destroy(&attr);
    });
    return &method_matcher_lock;
}

typedef std::unordered_map<uintptr_t, std::unordered_map<uintptr_t, MethodMatcher *>> MethodMatcherMap;

MethodMatcherMap &GetMethodMatcherMap() {
    static MethodMatcherMap *matcher_map = NULL;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        matcher_map = new MethodMatcherMap();
    });
    return *matcher_map;
};

#pragma mark - MethodMatcherLock Implementation

MethodMatcherLock::MethodMatcherLock() {
    pthread_mutex_lock(GetMethodMatcherMutex());
}

MethodMatcherLock::~MethodMatcherLock() {
    pthread_mutex_unlock(GetMethodMatcherMutex());
}

#pragma mark - MethodMatcher Implementation

bool MethodMatcher::isValid() {
    return 0 != target && 0 != selector && hit_count < limit_hit_count;
}

bool MethodMatcher::shouldDelete() {
    return hit_count >= limit_hit_count && using_count <= 0 && reference <= 0;
}

MethodMatcher::~MethodMatcher() {
    if (arguments) {
        for (Argument mark : *arguments) {
            if (mark.object) {
                CFRelease(mark.object);
            }
        }
        delete arguments;
        arguments = NULL;
    }
    if (return_object) {
        CFRelease(return_object);
        return_object = NULL;
    }
}

#pragma mark - Matcher Access

void UnsafeRemoveMethodMatcher(uintptr_t target, uintptr_t selector) {
    MethodMatcherMap &map = GetMethodMatcherMap();
    auto iterator = map.find(target);
    if (map.end() != iterator) {
        std::unordered_map<uintptr_t, MethodMatcher *> &matcher_map = iterator->second;
        auto matcher_iterator = matcher_map.find(selector);
        
        if (matcher_map.end() != matcher_iterator) {
            matcher_map.erase(matcher_iterator);
            MethodMatcher *matcher = matcher_iterator->second;
            delete matcher;
        }
    }
}

MethodMatcher * _Nullable UnsafeGetMethodMatcher(uintptr_t target, uintptr_t selector) {
    MethodMatcher *matcher = NULL;
    MethodMatcherMap &map = GetMethodMatcherMap();
    auto iterator = map.find(target);
    if (map.end() != iterator) {
        MethodMatcher *tmp = iterator->second[selector];
        if (tmp && tmp->isValid()) {
            matcher = tmp;
        }
    }
    return matcher;
}

bool AddMethodMatcher(MethodMatcher *matcher) {
    if (MM_UNLIKELY(!matcher || !matcher->isValid())) {
        return false;
    }
    uintptr_t target = matcher->target, selector = matcher->selector;
    
    MethodMatcherLock lock;
    MethodMatcher *old_matcher = UnsafeGetMethodMatcher(target, selector);
    if (old_matcher && !old_matcher->shouldDelete()) {
        return false;  // old_matcher 由于数据安全问题无法删除
    }
    if (old_matcher != matcher) {
        UnsafeRemoveMethodMatcher(target, selector);
        GetMethodMatcherMap()[target][selector] = matcher;
    }
    return true;
}

}  //namespace mm_method_matcher
