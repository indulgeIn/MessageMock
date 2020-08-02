
# MessageMock

#### 优雅的模拟 Objective-C 方法，对参数、返回值进行任意修改和检查。

#### 原理分析文章：[MessageMock : 优雅的模拟 Objective-C 方法](https://juejin.im/post/6856324772303273992)


# 特性

通过任意`[target selector]`调用命中目标方法。

**功能：**
- 修改目标方法返回值、参数
- 验证目标方法返回值、参数
- 跳过目标方法调用
- 获取目标方法命中次数

**TODO**
- 支持 x86
- 支持大于指针长度的返回值修改
- 支持大于指针长度的参数修改


# 安装

## CocoaPods

TODO

## 手动导入

1. 把 MessageMock 源码文件夹整个拖入工程。
2. 导入文件`#import "MessageMocker.h"`


# 基本用法

通过`MessageMocker.h`的接口进行链式配置（参考`MessageMockerTests.m`的单元测试代码）。

### 修改返回值

```
    NSObject *value = [NSObject new];
    
    MessageMocker.build(NSNotificationCenter.self, @selector(defaultCenter)).mockReturn(value).start();

    XCTAssertTrue(value == [NSNotificationCenter defaultCenter]);
```

### 修改参数
```
	UIView *view = [UIView new];
    NSInteger value = 999;
    
    MessageMocker.build(view, @selector(setTag:)).mockArgument(0, value).start();
    
    view.tag = 666;
    XCTAssertTrue(view.tag == value);
```

### 检查返回值

```
	__block unsigned long value = 0;
    
    NSObject *obj = [NSObject new];
    
    MessageMocker.build(obj, @selector(hash))
    .checkReturn(^(unsigned long arg) {
        value = arg;
    })
    .start();
    
    unsigned long res = [obj hash];
    XCTAssertTrue(value == res);
```

### 检查参数

```
	Class anyCls = NSString.self;
    
    MessageMocker.build(NSObject.self, @selector(isSubclassOfClass:))
    .checkArgument(0, ^(Class cls){
        XCTAssertTrue(anyCls == cls);
    })
    .start();
    
    [NSObject isSubclassOfClass:anyCls];
```

### 跳过方法调用

```
    UIView *view = [UIView new];
    NSInteger tag = 666;
    view.tag = tag;
    
    MessageMocker.build(view, @selector(setTag:)).skip(YES).start();
    
    view.tag = 999;
    XCTAssertTrue(view.tag = tag);
```

### 设置/读取命中次数

```
	NSObject *value = [NSObject new];
    
    MessageMocker *mocker = MessageMocker.build(TestObject.self, @selector(createObj))
    .mockReturn(value)
    .limitHitCount(2);
    mocker.start();
    
    XCTAssertTrue(value == [TestObject createObj]);
    XCTAssertTrue(mocker.hitCount == 1);
    XCTAssertTrue(value == [TestObject createObj]);
    XCTAssertTrue(mocker.hitCount == 2);
    
    XCTAssertTrue(value != [TestObject createObj]);
```
