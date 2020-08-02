//
//  MMChangeArgumentTests.m
//  MessageMockDemoTests
//
//  Created by 波儿菜 on 2020/7/10.
//

#import <XCTest/XCTest.h>
#import <MessageMock/mm_hook_objc_msgsend.h>
#import <MessageMock/mm_method_matcher.h>
#import "TestObject.h"

using namespace mm_method_matcher;

@interface MMChangeArgumentTests : XCTestCase

@end

@implementation MMChangeArgumentTests

- (void)setUp {
    MMStartWorking();
}

- (void)tearDown {}

- (void)testChangeArgument {
    MethodMatcher *matcher = new MethodMatcher();
    matcher->target = (uintptr_t)TestObject.self;
    matcher->selector = (uintptr_t)(void *)(@selector(createWithName:age:phone:isMan:height:weight:extra:));
    matcher->skip = false;
    matcher->limit_hit_count = 1;

    TestObject *argument = [TestObject defaultArgument];
    Argument mark0 = {.idx = 2, .object = (__bridge_retained void *)argument.name};
    Argument mark1 = {.idx = 3, .value = (uintptr_t)argument.age};
    Argument mark2 = {.idx = 4, .value = (uintptr_t)argument.phone};
    Argument mark3 = {.idx = 5, .value = (uintptr_t)argument.isMan};
    double height = argument.height;
    Argument mark4 = {.idx = 6, .value = *(uintptr_t *)&height};
    float weight = argument.weight;
    Argument mark5 = {.idx = 7, .value = *(uintptr_t *)&weight};
    Argument mark6 = {.idx = 8, .object = (__bridge_retained void *)argument.extra};
    matcher->arguments = new ArgumentList({mark0, mark1, mark2, mark3, mark4, mark5, mark6});

    AddMethodMatcher(matcher);

    TestObject *res = [TestObject createWithName:@"小芳" age:20 phone:55555555555 isMan:NO height:165 weight:100 extra:[NSObject new]];

    XCTAssertTrue([res isEqual:argument]);
}

@end
