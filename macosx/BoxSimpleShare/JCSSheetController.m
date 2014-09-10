

#import "JCSSheetController.h"

@interface JCSSheetController ()

@property (nonatomic, copy) JCSCompletionHandler handler;
- (void)sheetDidEnd:(NSWindow *)sheet returnCode:(NSUInteger)returnCode contextInfo:(void *)contextInfo;

@end

@implementation JCSSheetController

@synthesize handler;

- (void)beginSheetModalForWindow:(NSWindow *)window completionHandler:(JCSCompletionHandler)aHandler {
    self.handler = aHandler;
    
    [self sheetWillDisplay];
    
    [NSApp beginSheet:self.window modalForWindow:window modalDelegate:self didEndSelector:@selector(sheetDidEnd:returnCode:contextInfo:) contextInfo:NULL];
}

- (void)endSheetWithReturnCode:(NSUInteger)result {
    [NSApp endSheet:[self window] returnCode:result];
}

- (void)sheetWillDisplay {
    // Default does notlbprogressbar.m
    // Convenience for subclasses to override.
}

// Mark: -
// Mark: Class Extension methods

- (void)sheetDidEnd:(NSWindow *)sheet returnCode:(NSUInteger)returnCode contextInfo:(void *)contextInfo {
    [sheet orderOut:self];
    
    if (self.handler) {
        handler(returnCode);
    }
}

@end