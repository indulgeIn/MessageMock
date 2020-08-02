//
//  mm_method_matcher.h
//  MessageMock
//
//  Created by 波儿菜 on 2020/6/27.
//

#ifndef mm_method_matcher_h
#define mm_method_matcher_h

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include "mm_define.h"

MM_ASSUME_NONNULL_BEGIN

namespace mm_method_matcher {

typedef struct {
    /// 需要改变的参数下标
    unsigned int idx = 0;
    /// 需要改变的参数值
    uintptr_t value = 0;
    /// 需要改变的参数指针（用于接管 objc 对象引用计数）
    void * _Nullable object = NULL;
} Argument;

typedef std::vector<Argument> ArgumentList;

typedef enum {
    /// 无错误
    kArgumentCallbackErrorNone,
    /// 参数寄存器超过 x0 - x7
    kArgumentCallbackErrorOverX7,
    /// 参数寄存器超过 d0 - d7
    kArgumentCallbackErrorOverD7,
    /// 参数列表存在不支持的类型
    kArgumentCallbackErrorUnsupported,
    /// 没有这个下标的参数
    kArgumentCallbackErrorNoIndex,
} ArgumentCallbackError;

typedef void(^ArgumentCallback)(uintptr_t arg, ArgumentCallbackError error);

typedef std::unordered_map<unsigned int, ArgumentCallback> ArgumentCallbackMap;

typedef enum {
    /// 无错误
    kReturnCallbackErrorNone,
    /// 返回值不是通过 x0/d0 传递
    kReturnCallbackErrorNotX0D0,
    /// 没有返回值
    kReturnCallbackErrorNoReturn,
} ReturnCallbackError;

typedef void(^ReturnCallback)(uintptr_t arg, ReturnCallbackError error);

/// 方法匹配配置对象
class MethodMatcher {
public:
    /// 是否跳过该 [target selector] 调用
    bool skip = false;
    /// 是否改变返回值
    bool change_return = false;
    /// 被引用的次数（用于上层代码不期望该内存释放）
    long reference = 0;
    /// 命中次数
    long hit_count = 0;
    /// 限制命中次数
    long limit_hit_count = 1;
    /// 正在使用的数量
    long using_count = 0;
    /// 目标 id 地址
    uintptr_t target = 0;
    /// 目标 SEL 地址
    uintptr_t selector = 0;
    /// 需要改变的返回值
    uintptr_t return_value = 0;
    /// 需要改变的返回指针（用于接管 objc 对象引用计数）
    void * _Nullable return_object = NULL;
    /// 需要修改的参数表
    ArgumentList * _Nullable arguments = NULL;
    /// 调用时检查参数的函数指针表
    ArgumentCallbackMap * _Nullable argument_callback_map = NULL;
    /// 检查返回值的函数指针
    ReturnCallback _Nullable return_callback = NULL;
    
    /// 该 matcher 是否有效
    bool isValid();
    /// 是否可以删除
    bool shouldDelete();
    
    ~MethodMatcher();
};

/// matcher 读写锁
class MethodMatcherLock {
public:
    MethodMatcherLock();
    ~MethodMatcherLock();
};

/// 安全的添加 matcher，返回是否成功
bool AddMethodMatcher(MethodMatcher *matcher);
/// 不安全的移除 matcher
void UnsafeRemoveMethodMatcher(uintptr_t target, uintptr_t selector);
/// 不安全的获取 matcher
MethodMatcher * _Nullable UnsafeGetMethodMatcher(uintptr_t target, uintptr_t selector);

}  //namespace mm_method_matcher

MM_ASSUME_NONNULL_END

#endif /* mm_method_matcher_h */
