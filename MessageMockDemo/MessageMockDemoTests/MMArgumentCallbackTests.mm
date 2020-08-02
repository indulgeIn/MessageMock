//
//  MMArgumentCallbackTests.m
//  MessageMockDemoTests
//
//  Created by 波儿菜 on 2020/7/4.
//  

#import <XCTest/XCTest.h>
#import <MessageMock/mm_hook_objc_msgsend.h>
#import <MessageMock/mm_method_matcher.h>
#import "TestObject.h"

using namespace mm_method_matcher;

@interface MMArgumentCallbackTests : XCTestCase

@end

@implementation MMArgumentCallbackTests

- (void)setUp {
    MMStartWorking();
}

- (void)tearDown {}

- (void)testArgumentCallback {
    TestObject *argument = [TestObject defaultArgument];
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createWithName:age:phone:isMan:height:weight:extra:));
    matcher->skip = false;
    matcher->limit_hit_count = 1;
    
    ArgumentCallback callBack2 = ^(uintptr_t arg, ArgumentCallbackError error) {
        void *argPointer = (void *)arg;
        NSString *name = (__bridge NSString *)argPointer;
        XCTAssertTrue(name == argument.name);
        NSLog(@"\n callBack2 NSString:%@\n", name);
    };
    ArgumentCallback callBack3 = ^(uintptr_t arg, ArgumentCallbackError error) {
        int age = (int)arg;
        XCTAssertTrue(age == argument.age);
        NSLog(@"\n callBack3 int:%d\n", age);
    };
    ArgumentCallback callBack4 = ^(uintptr_t arg, ArgumentCallbackError error) {
        NSInteger phone = (NSInteger)arg;
        XCTAssertTrue(phone == argument.phone);
        NSLog(@"\n callBack4 NSInteger:%ld\n", phone);
    };
    ArgumentCallback callBack5 = ^(uintptr_t arg, ArgumentCallbackError error) {
        BOOL isMan = (BOOL)arg;
        XCTAssertTrue(isMan == argument.isMan);
        NSLog(@"\n callBack5 BOOL:%@\n", @(isMan));
    };
    ArgumentCallback callBack6 = ^(uintptr_t arg, ArgumentCallbackError error) {
        double height = *((double *)&arg);
        XCTAssertTrue(height == argument.height);
        NSLog(@"\n callBack6 double:%lf\n", height);
    };
    ArgumentCallback callBack7 = ^(uintptr_t arg, ArgumentCallbackError error) {
        float weight = *((float *)&arg);
        XCTAssertTrue(weight == argument.weight);
        NSLog(@"\n callBack7 float:%f\n", weight);
    };
    ArgumentCallback callBack8 = ^(uintptr_t arg, ArgumentCallbackError error) {
        void *argPointer = (void *)arg;
        NSObject *extra = (__bridge NSObject *)argPointer;
        XCTAssertTrue(extra == argument.extra);
        NSLog(@"\n callBack8 NSObject:%@\n", extra);
    };
    ArgumentCallbackMap *callbackMap = new ArgumentCallbackMap();
    (*callbackMap)[2] = callBack2;
    (*callbackMap)[3] = callBack3;
    (*callbackMap)[4] = callBack4;
    (*callbackMap)[5] = callBack5;
    (*callbackMap)[6] = callBack6;
    (*callbackMap)[7] = callBack7;
    (*callbackMap)[8] = callBack8;
    
    matcher->argument_callback_map = callbackMap;
    
    AddMethodMatcher(matcher);

    [TestObject createWithName:argument.name age:argument.age phone:argument.phone isMan:argument.isMan height:argument.height weight:argument.weight extra:argument.extra];
}

@end
