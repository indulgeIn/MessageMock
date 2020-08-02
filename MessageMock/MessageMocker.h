//
//  MessageMocker.h
//  MessageMock
//
//  Created by 波儿菜 on 2020/7/11.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

/// 模拟类，根据 [target selector] 命中目标方法，主要能力：
/// - 修改返回值、参数
/// - 验证返回值、参数
/// - 跳过原方法
/// - 获取方法命中次数
@interface MessageMocker : NSObject

/// 遍历构造（target 目标对象 selector 目标方法）
@property (class, nonatomic, copy, readonly) MessageMocker * _Nullable (^build)(id target, SEL selector);

/// 修改返回值
@property (nonatomic, copy, readonly) MessageMocker *(^mockReturn)(const char *type, ...);
#define mockReturn(arg) mockReturn(@encode(typeof(arg)), arg, nil)

/// 修改调用参数
@property (nonatomic, copy, readonly) MessageMocker *(^mockArgument)(unsigned int idx, const char *type, ...);
#define mockArgument(idx, arg) mockArgument(idx, @encode(typeof(arg)), arg, nil)

/// 检查返回值，callback 是闭包：^(AnyType arg) {}
@property (nonatomic, copy, readonly) MessageMocker *(^checkReturn)(id callback);

/// 检查参数，callback 是闭包：^(AnyType arg) {}
@property (nonatomic, copy, readonly) MessageMocker *(^checkArgument)(unsigned int idx, id callback);

/// 是否跳过该 [target selector] 调用，默认 NO
@property (nonatomic, copy, readonly) MessageMocker *(^skip)(BOOL skip);

/// 限制命中次数，默认 1（超限则该伪装对象失效）
@property (nonatomic, copy, readonly) MessageMocker *(^limitHitCount)(NSInteger count);

/// 方法命中次数（并发过高可能获取的值大于预期）
@property (nonatomic, assign, readonly) long hitCount;

/// 启用（只能启用一次，启用后该对象任何修改无效）
@property (nonatomic, copy, readonly) void(^start)(void);

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
+ (instancetype)new UNAVAILABLE_ATTRIBUTE;

@end

NS_ASSUME_NONNULL_END
