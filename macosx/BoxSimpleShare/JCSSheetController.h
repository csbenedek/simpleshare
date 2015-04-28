
#import <Cocoa/Cocoa.h>

typedef void (^JCSCompletionHandler)(NSUInteger result);

@interface JCSSheetController : NSWindowController 



- (void)beginSheetModalForWindow:(NSWindow *)window completionHandler:(JCSCompletionHandler)aHandler;

// Convenience methods for subclasses to use
- (void)endSheetWithReturnCode:(NSUInteger)result;
- (void)sheetWillDisplay; // Default does nothing

@end
