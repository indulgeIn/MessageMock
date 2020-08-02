//
//  MessageMockerTests.m
//  MessageMockDemoTests
//
//  Created by 波儿菜 on 2020/7/12.
//

#import <XCTest/XCTest.h>
#import <MessageMocker.h>
#import "TestObject.h"

@interface MessageMockerTests : XCTestCase

@end

@implementation MessageMockerTests

- (void)setUp {}

- (void)tearDown {}

- (void)testChangeReturnFloat {
    float value = 0.384189347192384789;
    
    MessageMocker.build(NSObject.self, @selector(new)).mockReturn(nil).skip(YES).start();
    
    MessageMocker.build(TestObject.self, @selector(createFloat)).mockReturn(value).start();
    
    XCTAssertTrue(value == [TestObject createFloat]);
    XCTAssertTrue(value != [TestObject createFloat]);
}

- (void)testChangeReturnDouble {
    double value = 0.48591734913287498;
    
    MessageMocker.build(TestObject.self, @selector(createDouble)).mockReturn(value).start();
    
    XCTAssertTrue(value == [TestObject createDouble]);
    XCTAssertTrue(value != [TestObject createDouble]);
}

- (void)testChangeReturnObject {
    NSObject *value = [NSObject new];
    
    MessageMocker.build(TestObject.self, @selector(createObj)).mockReturn(value).start();
    
    XCTAssertTrue(value == [TestObject createObj]);
    XCTAssertTrue(value != [TestObject createObj]);
}

- (void)testChangeReturnObject1 {
    NSObject *value = [NSObject new];
    
    MessageMocker.build(NSNotificationCenter.self, @selector(defaultCenter)).mockReturn(value).start();
    
    XCTAssertTrue(value == [NSNotificationCenter defaultCenter]);
    XCTAssertTrue(value != [NSNotificationCenter defaultCenter]);
    
    MessageMocker.build([NSUserDefaults standardUserDefaults], @selector(objectForKey:)).mockReturn(value).start();
    
    XCTAssertTrue(value == [[NSUserDefaults standardUserDefaults] objectForKey:@"AnyKey"]);
    XCTAssertTrue(value != [[NSUserDefaults standardUserDefaults] objectForKey:@"AnyKey"]);
}

- (void)testChangeArgument {
    TestObject *value = [TestObject defaultArgument];
    
    MessageMocker.build(TestObject.self, @selector(createWithName:age:phone:isMan:height:weight:extra:))
    .mockArgument(0, value.name)
    .mockArgument(1, value.age)
    .mockArgument(2, value.phone)
    .mockArgument(3, value.isMan)
    .mockArgument(4, value.height)
    .mockArgument(5, value.weight)
    .mockArgument(6, value.extra)
    .start();
    
    TestObject *res = [TestObject createWithName:@"小芳" age:20 phone:55555555555 isMan:NO height:165 weight:100 extra:[NSObject new]];
    
    XCTAssertTrue([res isEqual:value]);
}

- (void)testChangeArgument1 {
    UIView *view = [UIView new];
    NSInteger value = 999;
    
    MessageMocker.build(view, @selector(setTag:)).mockArgument(0, value).start();
    
    view.tag = 666;
    XCTAssertTrue(view.tag == value);
}

- (void)testCheckReturnObject {
    __block id value = nil;
    
    MessageMocker.build(TestObject.self, @selector(createObj))
    .checkReturn(^(TestObject *arg) {
        value = arg;
    })
    .start();
    
    id res = [TestObject createObj];
    XCTAssertTrue(value == res);
}

- (void)testCheckReturnInt {
    __block int value = 0;
    
    MessageMocker.build(TestObject.self, @selector(createInt))
    .checkReturn(^(int arg) {
        value = arg;
    })
    .start();
    
    int res = [TestObject createInt];
    XCTAssertTrue(value == res);
}

- (void)testCheckReturnFloat {
    __block float value = 0;
    
    MessageMocker.build(TestObject.self, @selector(createFloat))
    .checkReturn(^(float arg) {
        value = arg;
    })
    .start();
    
    float res = [TestObject createFloat];
    XCTAssertTrue(value == res);
}

- (void)testCheckReturnLong {
    __block unsigned long value = 0;
    
    NSObject *obj = [NSObject new];
    
    MessageMocker.build(obj, @selector(hash))
    .checkReturn(^(unsigned long arg) {
        value = arg;
    })
    .start();
    
    unsigned long res = [obj hash];
    XCTAssertTrue(value == res);
}

- (void)testCheckArgument {
    TestObject *argument = [TestObject defaultArgument];
    
    MessageMocker.build(TestObject.self, @selector(createWithName:age:phone:isMan:height:weight:extra:))
    .checkArgument(0, ^(NSString *arg) {
        XCTAssertTrue(arg == argument.name);
    })
    .checkArgument(1, ^(int arg) {
        XCTAssertTrue(arg == argument.age);
    })
    .checkArgument(2, ^(NSInteger arg) {
        XCTAssertTrue(arg == argument.phone);
    })
    .checkArgument(3, ^(BOOL arg) {
        XCTAssertTrue(arg == argument.isMan);
    })
    .checkArgument(4, ^(double arg) {
        XCTAssertTrue(arg == argument.height);
    })
    .checkArgument(5, ^(float arg) {
        XCTAssertTrue(arg == argument.weight);
    })
    .checkArgument(6, ^(id arg) {
        XCTAssertTrue(arg == argument.extra);
    })
    .start();
    
    [TestObject createWithName:argument.name age:argument.age phone:argument.phone isMan:argument.isMan height:argument.height weight:argument.weight extra:argument.extra];
}

- (void)testCheckArgument1 {
    Class anyCls = NSString.self;
    
    MessageMocker.build(NSObject.self, @selector(isSubclassOfClass:))
    .checkArgument(0, ^(Class cls){
        XCTAssertTrue(anyCls == cls);
    })
    .start();
    
    [NSObject isSubclassOfClass:anyCls];
}

- (void)testSkip {
    UIView *view = [UIView new];
    NSInteger tag = 666;
    view.tag = tag;
    
    MessageMocker.build(view, @selector(setTag:)).skip(YES).start();
    
    view.tag = 999;
    XCTAssertTrue(view.tag = tag);
}

- (void)testHitCount {
    NSObject *value = [NSObject new];
    
    MessageMocker *mocker = MessageMocker.build(TestObject.self, @selector(createObj)).mockReturn(value).limitHitCount(3);
    mocker.start();
    
    XCTAssertTrue(value == [TestObject createObj]);
    XCTAssertTrue(mocker.hitCount == 1);
    XCTAssertTrue(value == [TestObject createObj]);
    XCTAssertTrue(mocker.hitCount == 2);
    XCTAssertTrue(value == [TestObject createObj]);
    XCTAssertTrue(mocker.hitCount == 3);
    
    XCTAssertTrue(value != [TestObject createObj]);
}

@end
