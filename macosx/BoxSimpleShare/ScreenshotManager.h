//
//  ScreenshotManager.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/24/11.
//

@class ScreenshotManager;

@protocol ScreenshotManagerDelegate <NSObject>

- (void)screenshotManager:(ScreenshotManager*)manager didReceiveNewImage:(NSImage*)image;

@end

@interface ScreenshotManager : NSObject {

    id<ScreenshotManagerDelegate> 	_delegate;
    NSRect                          _captureRect;
    NSSize                          _screenshotSize;
    CGDirectDisplayID               _displayId;
    NSTimer*                        _captureTimer;    
}

@property (nonatomic, assign) id<ScreenshotManagerDelegate> delegate;

- (void)startContinuoslyCaptureInRect:(NSRect)rect displayId:(CGDirectDisplayID)displayId;
- (void)stopContinuoslyCapture;

@end
