#ifdef __OBJC__
#import <UIKit/UIKit.h>
#else
#ifndef FOUNDATION_EXPORT
#if defined(__cplusplus)
#define FOUNDATION_EXPORT extern "C"
#else
#define FOUNDATION_EXPORT extern
#endif
#endif
#endif

#import "mm_argument_callback.h"
#import "mm_argument_change.h"
#import "mm_argument_stack.h"
#import "mm_define.h"
#import "mm_hook_objc_msgsend.h"
#import "mm_method_matcher.h"
#import "mm_runtime.h"
#import "mm_fishhook.h"
#import "MessageMocker.h"

FOUNDATION_EXPORT double MessageMockVersionNumber;
FOUNDATION_EXPORT const unsigned char MessageMockVersionString[];

