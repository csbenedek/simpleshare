//
//  ScreenshotManager.m
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/24/11.
//

#import "ScreenshotManager.h"

static const NSTimeInterval ScreenShotTimeInterval = 0.05;

@interface ScreenshotManager (Private)

- (void)appendMouseCursor:(NSImage*)sourceImage;
- (NSRect)captureRect;

@end

@implementation ScreenshotManager

@synthesize delegate = _delegate;

- (id)init {
    if (self = [super init]) {
        _screenshotSize = NSZeroSize;
    }
    return self;
}

- (void)dealloc {
    [self stopContinuoslyCapture];
    [super dealloc];
}

#pragma mark -

- (void)startContinuoslyCaptureInRect:(NSRect)rect displayId:(CGDirectDisplayID)displayId {
    _captureRect = rect;
    _captureTimer = [[NSTimer scheduledTimerWithTimeInterval:ScreenShotTimeInterval
                                                     target:self
                                                   selector:@selector(captureScreen)
                                                   userInfo:nil
                                                    repeats:YES] retain];
    _displayId = displayId;
}

- (NSRect)captureRect {
    return _captureRect;
}

- (void)stopContinuoslyCapture {
    [_captureTimer invalidate];
    [_captureTimer release];
    _captureTimer = nil;
}

- (void)captureScreen {
    CGImageRef screenShot = CGDisplayCreateImageForRect(_displayId, NSRectToCGRect(_captureRect));
//    CGImageRef screenShot = CGWindowListCreateImage(NSRectToCGRect(_captureRect), kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);
    if (screenShot) {
        NSBitmapImageRep* bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:screenShot];
        NSImage* image = [[[NSImage alloc] init] autorelease];
        [image addRepresentation:bitmapRep];
        [bitmapRep release];
        [self appendMouseCursor:image];
        [_delegate screenshotManager:self didReceiveNewImage:image];
        CGImageRelease(screenShot);
    }
}

- (void)appendMouseCursor:(NSImage*)sourceImage {
    // get the cursor image 
    NSPoint mouseLoc = [NSEvent mouseLocation]; 
        
    NSImage* overlay = [[NSCursor currentCursor] image];
    NSRect screenRect = [[NSScreen mainScreen] frame];
    // convert to flipped coordinate system
    mouseLoc.y = screenRect.size.height - mouseLoc.y;
    
    NSRect cursorRect;
    cursorRect.origin = mouseLoc;
    cursorRect.size = [overlay size];
    
    if (NSIntersectsRect(cursorRect, _captureRect)) {
        // Next we need to convert rect from screen coordinate system to image
        cursorRect.origin.x -= _captureRect.origin.x;
        cursorRect.origin.y -= _captureRect.origin.y;
        // convert for non-flipped coordinates because of non-flipped image coordinate system
        cursorRect.origin.y = [sourceImage size].height - cursorRect.origin.y - cursorRect.size.height;
        [sourceImage lockFocus];
        [overlay drawInRect:cursorRect fromRect:NSMakeRect(0, 0, [overlay size].width, [overlay size].height) operation:NSCompositeSourceOver fraction:1.0];
        [sourceImage unlockFocus];
    }
}

@end
