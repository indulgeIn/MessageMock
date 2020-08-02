//
//  TestObject.h
//  MessageMockDemo
//
//  Created by 波儿菜 on 2020/6/27.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface TestObject : NSObject

+ (TestObject *)createObj;

+ (char *)createPointer;

+ (long)createLong;

+ (int)createInt;

+ (int16_t)createInt16;

+ (int8_t)createInt8;

+ (double)createDouble;

+ (float)createFloat;

/*
参数基本规则
仅使用寄存器情况：
通用寄存器参数最多 6 个 x2 - x7
浮点寄存器参数最多 8 个 d0 - d7 （编译器限制不能连续超过6个）
*/

@property (nonatomic, copy) NSString *name;
@property (nonatomic, assign) int age;
@property (nonatomic, assign) NSInteger phone;
@property (nonatomic, assign) BOOL isMan;
@property (nonatomic, assign) double height;
@property (nonatomic, assign) float weight;
@property (nonatomic, strong) NSObject *extra;

+ (TestObject *)createWithName:(NSString *)name
                           age:(int)age
                         phone:(NSInteger)phone
                         isMan:(BOOL)isMan
                        height:(double)height
                        weight:(float)weight
                         extra:(NSObject *)extra;

+ (TestObject *)defaultArgument;

@end

NS_ASSUME_NONNULL_END
