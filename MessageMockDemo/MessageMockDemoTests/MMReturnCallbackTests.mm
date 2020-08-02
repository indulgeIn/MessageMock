//
//  MMReturnCallbackTests.m
//  MessageMockDemoTests
//
//  Created by 波儿菜 on 2020/7/18.
//  

#import <XCTest/XCTest.h>
#import <MessageMock/mm_hook_objc_msgsend.h>
#import <MessageMock/mm_method_matcher.h>
#import "TestObject.h"

using namespace mm_method_matcher;

@interface MMReturnCallbackTests : XCTestCase

@end

@implementation MMReturnCallbackTests

- (void)setUp {
    MMStartWorking();
}

- (void)tearDown {}

- (void)testReturnObjectCallback {
    __block TestObject *checkObj = nil;
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createObj));
    matcher->skip = false;
    
    ReturnCallback returnCallback = ^(uintptr_t arg, ReturnCallbackError error) {
        void *argPointer = (void *)arg;
        checkObj = (__bridge TestObject *)argPointer;
    };
    matcher->return_callback = returnCallback;
    
    AddMethodMatcher(matcher);
    
    TestObject *returnObj = [TestObject createObj];
    
    XCTAssertTrue(returnObj == checkObj);
}

- (void)testReturnFloatCallback {
    __block float value = 0;
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createFloat));
    matcher->skip = false;
    
    ReturnCallback returnCallback = ^(uintptr_t arg, ReturnCallbackError error) {
        float tmp = *(float *)&arg;
        value = tmp;
    };
    matcher->return_callback = returnCallback;
    
    AddMethodMatcher(matcher);
    
    float res = [TestObject createFloat];
    
    XCTAssertTrue(value == res);
}

- (void)testChangeReturnObjectCallback {
    TestObject *changeObj = [TestObject new];
    
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createObj));
    matcher->skip = false;
    matcher->change_return = true;
    matcher->return_object = (__bridge_retained void *)changeObj;
    
    ReturnCallback returnCallback = ^(uintptr_t arg, ReturnCallbackError error) {
        void *argPointer = (void *)arg;
        TestObject *tmp = (__bridge TestObject *)argPointer;
        XCTAssertTrue(changeObj == tmp);
    };
    matcher->return_callback = returnCallback;
    
    AddMethodMatcher(matcher);
    
    TestObject *returnObj = [TestObject createObj];
    
    XCTAssertTrue(returnObj == changeObj);
}

@end
