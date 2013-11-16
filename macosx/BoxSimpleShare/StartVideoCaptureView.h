//
//  StartVideoCaptureView.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/24/11.
//

@class StartVideoCaptureView;

@protocol StartVideoCaptureViewDelegate <NSObject>

- (void)startCaptureView:(StartVideoCaptureView*)view didChooseCaptureRect:(NSRect)rect;

@end

@interface StartVideoCaptureView : NSView {
    
    NSPoint                             _startPoint;
    NSPoint                             _endPoint;
    NSRect                              _captureRect;
    NSColor*                            _captureFrameColor;
    id<StartVideoCaptureViewDelegate>   _delegate;
    
    
    BOOL                                _shouldDrawOnlyFrame;
}

@property (nonatomic, assign) id<StartVideoCaptureViewDelegate> delegate;

- (void)setShouldDrawOnlyFrame:(BOOL)shouldDraw;
- (void)clear;

@end
