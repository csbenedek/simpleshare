//
//  StartVideoCaptureController.m
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/24/11.
//

#import "StartVideoCaptureController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "MainController.h"
#import "Extended.h"
#import "Utilities.h"
#import "VideoCaptureManagerLion.h"
#import <AVFoundation/AVFoundation.h>
#import "StandardPaths.h"



static const int MinCaptureRectWidth = 30;
static const int MinCaptureRectHeight = 30;
static const int FrameHalfWidth = 2;

@interface StartVideoCaptureController (Private)

- (void)startCaptureViewDidChooseCaptureRect:(NSRect)rect;
- (void)hideStopButton;

@end

@implementation StartVideoCaptureController

@synthesize delegate = _delegate;

- (void)dealloc {
    for (StartVideoCaptureView* view in _captureViews) {
        view.delegate = nil;
    }
    [_captureViews release];
        
    [_transparentWindows release];
    
    [_videoCaptureManager release];
    
    [super dealloc];
}

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

- (void)stopVideoRecording {

    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyy-MM-dd HH-mm-ss"];
    NSString* extension = [[[BoxSimpleShareAppDelegate sharedDelegate] mainController] videoFormatExtension];
    long format = 1;//kQTFileTypeMP4;;
//    if ([extension isEqualToString:@"flv"]) {
//        format = kQTFileTypeFlash;
//    } else if ([extension isEqualToString:@"mp4"]) {
//        format = kQTFileTypeMP4;
//    } else if ([extension isEqualToString:@"avi"]) {
//        format = kQTFileTypeAVI;
//    }
    NSString* filename = [NSString stringWithFormat:@"Screencast %@.%@", [formatter stringFromDate:[NSDate date]], extension];
    [formatter release];
    NSString* savedPath = [[[NSFileManager defaultManager] cacheDataPath] stringByAppendingPathComponent:filename];
     [_videoCaptureManager saveCurrentMovieToFile:savedPath exportFormat:format];   
    _isCapturing = NO;
    
    [self hideStopButton];
}

- (BOOL)isCapturing {
    return _isCapturing;
}

- (void)clear {
    for (NSWindow* window in _transparentWindows) {
        [window setIgnoresMouseEvents:NO];
    }
    for (StartVideoCaptureView* view in _captureViews) {
        [view clear];
    }
}

#pragma mark -

- (void)showStopButtonForCaptureRect:(NSRect)captureRect screenFrame:(NSRect)screenFrame {
    if (!_stopCaptureButton) {
        _stopCaptureButton = [[NSButton alloc] init];
        [_stopCaptureButton setTitle:InterfaceString(@"StopCaptureButtonTitle")];
        [_stopCaptureButton setTarget:self];
        [_stopCaptureButton setAction:@selector(performStopVideoCapture)];
        [_stopCaptureButton setBezelStyle:NSRoundedBezelStyle];
        [_stopCaptureButton sizeToFit];
    }
    NSRect frame;
    frame.size = _stopCaptureButton.frame.size;
    frame.origin.y = NSMinY(captureRect) - frame.size.height - FrameHalfWidth;
    frame.origin.x = NSMaxX(captureRect) - frame.size.width;
    
    if (!_stopCaptureWindow) {      
        _stopCaptureWindow = [[self createTransparentNonDecoratedWindowWithFrame:frame] retain];
        [_stopCaptureWindow setLevel:NSScreenSaverWindowLevel];
        [[_stopCaptureWindow contentView] addSubview:_stopCaptureButton];
    } else {
        [_stopCaptureWindow setFrame:frame display:YES];
    }
    int minY = NSMinY(captureRect) - _stopCaptureButton.frame.size.height - FrameHalfWidth;
    if (minY > screenFrame.origin.y) {
        [_stopCaptureWindow makeKeyAndOrderFront:self];
    }
}

- (void)hideStopButton {
    [_stopCaptureWindow close];
}

- (void)performStopVideoCapture {
    [_delegate videoCapturControlllerDidPressStopCapture:self];
}

- (void)showWindows {
    
    NSArray* screens = [NSScreen screens];
    for (StartVideoCaptureView* view in _captureViews) {
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
        StartVideoCaptureView* view = [[StartVideoCaptureView alloc] initWithFrame:[[win contentView] frame]];
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

#pragma mark -

- (void)startCaptureView:(StartVideoCaptureView*)view didChooseCaptureRect:(NSRect)rect {
    
    [[NSCursor arrowCursor] set];
    _actualView = nil;
    if (rect.size.width > MinCaptureRectWidth && rect.size.height > MinCaptureRectHeight) {
    
        for (NSWindow* window in _transparentWindows) {
            [window setIgnoresMouseEvents:YES];
            if ([view window] != window) {
                [window close];
            }
        }
        
        [view setShouldDrawOnlyFrame:YES];
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
        
        [self showStopButtonForCaptureRect:NSMakeRect(newOrigin.x, newOrigin.y, rect.size.width, rect.size.height) screenFrame:screenRect];
    
        assert(!NSEqualRects(screenRect, NSZeroRect));
        NSRect captureRect;
               
        captureRect.origin.x = newOrigin.x;
        int zeroBasedY = newOrigin.y - screenRect.origin.y;
        captureRect.origin.y = screenRect.size.height - rect.size.height - zeroBasedY;
        captureRect.origin.y += screenRect.origin.y;
        captureRect.size = rect.size;
        
        
        if (!_videoCaptureManager) {
            if ([Utilities isRunningOnLion]) {
                _videoCaptureManager = [[VideoCaptureManagerLion alloc] init];
            } else {
                _videoCaptureManager =  nil; // [[VideoCaptureManagerPreLion alloc] init];
            }
            _videoCaptureManager.delegate = self;
        }
                               
        [_videoCaptureManager startVideoCaptureInRect:captureRect screen:captureScreen];
        _isCapturing = YES;
        [_delegate videoCaptureControllerDidStartVideoCapture:self];
    } else {
        [self hideAllWindows];
        [self clear];
        [_delegate videoCaptureControllerDidFailedStartVideoCapture:self];
    }
}

#pragma mark -

- (void)videoCaptureManager:(VideoCaptureManagerBase*)manager didSaveMovieToPath:(NSString*)path {
    [_delegate videoCaptureController:self didCaptureVideoToPath:path];    
}

@end
