//
//  ImageCaptureController.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 11/29/13.
//  Copyright (c) 2013 SAPLogix. All rights reserved.
//

#import "ImageCaptureController.h"
#import "StandardPaths.h"
#import "NSImage+SavingAdditions.h"



@implementation ImageCaptureController

@synthesize delegate = _delegate;

- (NSWindow*)createTransparentNonDecoratedWindowWithFrame:(NSRect)frame {
    NSWindow* result = [[NSWindow alloc] initWithContentRect:frame
                                                   styleMask:NSBorderlessWindowMask
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    NSColor* color = [NSColor clearColor];
    [result setBackgroundColor:color];
    [result setOpaque:NO];
    [result setIgnoresMouseEvents:NO];
    [result setReleasedWhenClosed:NO];
    [result setSharingType:NSWindowSharingNone]; // to avoid visibility if this window on screenshots
    return [result autorelease];
}


- (void)showWindows {
    
    NSArray* screens = [NSScreen screens];
    for (StartImageCaptureView* view in _captureViews) {
        view.delegate = nil;
    }
    [_captureViews release];
    [_transparentWindows release];
    _transparentWindows = [[NSMutableArray alloc] init];
    _captureViews = [[NSMutableArray alloc] init];
    for (NSScreen* screen in screens) {
        NSRect sFrame = [screen frame];
        DbgLog(@"%@", NSStringFromRect(sFrame));
        NSWindow* win = [self createTransparentNonDecoratedWindowWithFrame:[screen frame]];
        [_transparentWindows addObject:win];
        StartImageCaptureView* view = [[StartImageCaptureView alloc] initWithFrame:[[win contentView] frame]];
        view.delegate = self;
        [[win contentView] addSubview:view];
        [_captureViews addObject:view];
    }
    
    [NSApp arrangeInFront:self];
    for (NSWindow* window in _transparentWindows) {
        [window makeKeyAndOrderFront:nil];
        [window setLevel:NSScreenSaverWindowLevel];
    }
    [NSApp activateIgnoringOtherApps:YES];
}

- (void)hideAllWindows {
    for (NSWindow* window in _transparentWindows) {
        [window close];
    }
}

-(void) captureFullScreen {
    
    CGDisplayCount displayCount;
    CGDirectDisplayID displays[32];
    
    // grab the active displays
    CGGetActiveDisplayList(32, displays, &displayCount);
    
    NSString* tempFile = nil;
    // go through the list
    for (int i=0; i<displayCount; i++)
    {
        // make a snapshot of the current display
        NSDateFormatter *dateFormatter = [[[NSDateFormatter alloc] init] autorelease];
        [dateFormatter setDateFormat:@"yyyy-MM-dd hh:mm:ss a"];
        NSString* fileName = [NSString stringWithFormat:@"Screen shot %@.png", [dateFormatter stringFromDate:[NSDate date]]];
        CGImageRef image = CGDisplayCreateImage(displays[i]);
        
        tempFile =   [[[[NSFileManager defaultManager] cacheDataPath]stringByAppendingPathComponent:fileName] retain];
        CGImageWriteToFile(image, tempFile);
        
        break;
    }
    
    if (tempFile) {
        [self.delegate imageCaptureController:self didCaptureVideoToPath:tempFile];
    }
}

- (void)startCaptureView:(StartImageCaptureView*)view didChooseCaptureRect:(NSRect)rect {
    [[NSCursor arrowCursor] set];
    _actualView = nil;
    if (rect.size.width > 0 && rect.size.height > 0) {
        
        for (NSWindow* window in _transparentWindows) {
            [window setIgnoresMouseEvents:YES];
            if ([view window] != window) {
                [window close];
            }
        }
        
        //[view setShouldDrawOnlyFrame:YES];
        _actualView = view;
        
        // Need to transform rect to screen coordinates and make it flipped for screen capture api
        NSPoint newOrigin = [[_actualView window] convertBaseToScreen:rect.origin];
        
        NSScreen* captureScreen = nil;
        NSRect screenRect = NSZeroRect;
        for (NSScreen* screen in [NSScreen screens]) {
            if (NSPointInRect(newOrigin, [screen frame])) {
                screenRect = [screen frame];
                captureScreen = screen;
                break;
            }
        }
        

        
        assert(!NSEqualRects(screenRect, NSZeroRect));
        NSRect captureRect;
        
        captureRect.origin.x = newOrigin.x;
        int zeroBasedY = newOrigin.y - screenRect.origin.y;
        captureRect.origin.y = screenRect.size.height - rect.size.height - zeroBasedY;
        captureRect.origin.y += screenRect.origin.y;
        captureRect.size = rect.size;
        
        
        CGDisplayCount displayCount;
        CGDirectDisplayID displays[32];
        
        // grab the active displays
        CGGetActiveDisplayList(32, displays, &displayCount);
        NSString* tempFile = nil;
        for (int i=0; i<displayCount; i++)
        {

            [_actualView.window close];
            
            NSDateFormatter *dateFormatter = [[[NSDateFormatter alloc] init] autorelease];
            [dateFormatter setDateFormat:@"yyyy-MM-dd hh:mm:ss a"];
            NSString* fileName = [NSString stringWithFormat:@"screen shot %@.png", [dateFormatter stringFromDate:[NSDate date]]];
            CGImageRef image = CGDisplayCreateImageForRect(displays[i], NSRectToCGRect( captureRect));
            
            tempFile =   [[[[NSFileManager defaultManager] cacheDataPath]stringByAppendingPathComponent:fileName] retain];
            CGImageWriteToFile(image, tempFile);
            
            break;
        }
        
        if (tempFile) {
            [self.delegate imageCaptureController:self didCaptureVideoToPath:tempFile];
        }
        [self hideAllWindows];
        [self clear];
    } else {
        [self hideAllWindows];
        [self clear];
    }
}

- (void)clear {
    for (NSWindow* window in _transparentWindows) {
        [window setIgnoresMouseEvents:NO];
    }
    for (StartImageCaptureView* view in _captureViews) {
        [view clear];
    }
}
void CGImageWriteToFile(CGImageRef image, NSString *path) {
    CFURLRef url = (CFURLRef)[NSURL fileURLWithPath:path];
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
    CGImageDestinationAddImage(destination, image, nil);
    
    if (!CGImageDestinationFinalize(destination)) {
        DbgLog(@"Failed to write image to %@", path);
    }
    
    CFRelease(destination);
}
-(void) stopImageCapture {
    
}
@end
