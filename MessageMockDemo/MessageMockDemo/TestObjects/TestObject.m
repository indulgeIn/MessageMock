//
//  TestObject.m
//  MessageMockDemo
//
//  Created by 波儿菜 on 2020/6/27.
//  

#import "TestObject.h"

@implementation TestObject

- (void)dealloc {
    NSLog(@"dealloc: %@", self);
}

+ (TestObject *)createObj {
    TestObject *obj = [TestObject new];
    return obj;
}

+ (char *)createPointer {
    char *value = (char *)malloc(sizeof(char));
    value = "aaaaa";
    return value;
}

+ (long)createLong {
    return 100;
}

+ (int)createInt {
    return -100;
}

+ (int16_t)createInt16 {
    return -100;
}

+ (int8_t)createInt8 {
    return (int8_t)(1 << 7);
}

+ (double)createDouble {
    return 1000000000000.0001192873912731982;
}

+ (float)createFloat {
    return 97249213.23141234;
}

+ (TestObject *)createWithName:(NSString *)name age:(int)age phone:(NSInteger)phone isMan:(BOOL)isMan height:(double)height weight:(float)weight extra:(nonnull NSObject *)extra {
    TestObject *res = [TestObject new];
    res.name = name;
    res.phone = phone;
    res.age = age;
    res.height = height;
    res.weight = weight;
    res.isMan = isMan;
    res.extra = extra;
    return res;
}

+ (TestObject *)defaultArgument {
    TestObject *res = [TestObject new];
    res.name = @"波儿菜";
    res.phone = 66666666666;
    res.age = 27;
    res.isMan = YES;
    res.height = 170;
    res.weight = -105;
    static NSObject *extra = nil;
    if (!extra) {
        extra = [NSObject new];
    }
    res.extra = extra;
    return res;
}

- (BOOL)isEqual:(TestObject *)object {
    return
    self.name == object.name
    && self.phone == object.phone
    && self.age == object.age
    && self.height == object.height
    && self.weight == object.weight
    && self.isMan == object.isMan
    && self.extra == object.extra;
}

@end
