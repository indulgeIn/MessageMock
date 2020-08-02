//
//  MMMethodReturnTests.m
//  MessageMockDemoTests
//
//  Created by 波儿菜 on 2020/6/28.
//  

#import <XCTest/XCTest.h>
#import <MessageMock/mm_hook_objc_msgsend.h>
#import <MessageMock/mm_method_matcher.h>
#import "TestObject.h"

using namespace mm_method_matcher;

@interface MMMethodReturnTests : XCTestCase

@end

@implementation MMMethodReturnTests

- (void)setUp {
    MMStartWorking();
}

- (void)tearDown {}

#pragma mark - 指针返回值改为其它指针

- (void)testChangeReturnPointer {
    TestObject *returnValue = [TestObject createObj];
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createObj));
    matcher->skip = false;
    matcher->change_return = true;
    matcher->return_object = (__bridge_retained void *)returnValue;
    matcher->limit_hit_count = 1;
    
    AddMethodMatcher(matcher);

    TestObject *value = [TestObject createObj];
    XCTAssertTrue(returnValue == value);

    TestObject *value1 = [TestObject createObj];
    XCTAssertFalse(returnValue == value1);
}

#pragma mark - 指针返回值改为 C 指针

- (void)testChangeReturnCPointer {
    char *returnValue = [TestObject createPointer];
    returnValue = (char *)"dddddddd";

    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createPointer));
    matcher->skip = true;
    matcher->change_return = true;
    matcher->return_value = (uintptr_t)returnValue;
    matcher->limit_hit_count = 1;
    
    AddMethodMatcher(matcher);

    char *value = [TestObject createPointer];
    XCTAssertTrue(returnValue == value);
    
    char *value1 = [TestObject createPointer];
    XCTAssertFalse(returnValue == value1);
}

#pragma mark - 指针返回值改为 nil

- (void)testChangeReturnPointerToNil {
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createObj));
    matcher->skip = true;
    matcher->change_return = true;
    matcher->return_value = (uintptr_t)nil;
    matcher->limit_hit_count = 1;
    
    AddMethodMatcher(matcher);

    TestObject *value = [TestObject createObj];
    XCTAssertTrue(nil == value);
    
    TestObject *value1 = [TestObject createObj];
    XCTAssertFalse(nil == value1);
}

#pragma mark - 整形返回值修改

- (void)testChangeReturnLong {
    long returnValue = [TestObject createLong] + 10086;
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createLong));
    matcher->skip = true;
    matcher->change_return = true;
    matcher->return_value = returnValue;
    matcher->limit_hit_count = 1;
    
    AddMethodMatcher(matcher);

    long value = [TestObject createLong];
    XCTAssertTrue(returnValue == value);
    
    long value1 = [TestObject createLong];
    XCTAssertFalse(returnValue == value1);
}

- (void)testChangeReturnInt {
    int returnValue = [TestObject createInt] - 20000;
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createInt));
    matcher->skip = true;
    matcher->change_return = true;
    matcher->return_value = returnValue;
    matcher->limit_hit_count = 1;
    
    AddMethodMatcher(matcher);

    int value = [TestObject createInt];
    XCTAssertTrue(returnValue == value);
    
    int value1 = [TestObject createInt];
    XCTAssertFalse(returnValue == value1);
}

- (void)testChangeReturnInt16 {
    int returnValue = [TestObject createInt16] - 200;
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createInt16));
    matcher->skip = true;
    matcher->change_return = true;
    matcher->return_value = returnValue;
    matcher->limit_hit_count = 1;
    
    AddMethodMatcher(matcher);

    int value = [TestObject createInt16];
    XCTAssertTrue(returnValue == value);
    
    int value1 = [TestObject createInt16];
    XCTAssertFalse(returnValue == value1);
}

- (void)testChangeReturnInt8 {
    int returnValue = [TestObject createInt8] + 10;
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createInt8));
    matcher->skip = true;
    matcher->change_return = true;
    matcher->return_value = returnValue;
    matcher->limit_hit_count = 1;
    
    AddMethodMatcher(matcher);

    int value = [TestObject createInt8];
    XCTAssertTrue(returnValue == value);
    
    int value1 = [TestObject createInt8];
    XCTAssertFalse(returnValue == value1);
}

#pragma mark - 浮点型返回值修改

- (void)testChangeReturnDouble {
    double returnValue = [TestObject createDouble] - 1000000000000.000002394713984;
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createDouble));
    matcher->skip = true;
    matcher->change_return = true;
    matcher->return_value = *(uintptr_t *)&returnValue;
    matcher->limit_hit_count = 1;
    
    AddMethodMatcher(matcher);

    double value = [TestObject createDouble];
    XCTAssertTrue(returnValue == value);
    
    double value1 = [TestObject createDouble];
    XCTAssertFalse(returnValue == value1);
}

- (void)testChangeReturnFloat {
    float returnValue = [TestObject createFloat] - 2000000000000.000002394713984;
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createFloat));
    matcher->skip = true;
    matcher->change_return = true;
    matcher->return_value = *(uintptr_t *)&returnValue;
    matcher->limit_hit_count = 1;

    AddMethodMatcher(matcher);

    float value = [TestObject createFloat];
    XCTAssertTrue(returnValue == value);

    float value1 = [TestObject createFloat];
    XCTAssertFalse(returnValue == value1);
}

@end
