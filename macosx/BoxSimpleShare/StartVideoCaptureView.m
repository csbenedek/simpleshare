//
//  StartVideoCaptureView.m
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/24/11.
//

#import "StartVideoCaptureView.h"

@implementation StartVideoCaptureView

@synthesize delegate = _delegate;

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        _startPoint = NSZeroPoint;
        _endPoint = NSZeroPoint;
        _captureRect = NSZeroRect;
        _captureFrameColor = [[NSColor colorWithDeviceWhite:0 alpha:0.5] retain];
    }
    
    return self;
}

- (void)dealloc {
    [_captureFrameColor release];
    [super dealloc];
}

- (void)setShouldDrawOnlyFrame:(BOOL)shouldDraw {
    if (_shouldDrawOnlyFrame != shouldDraw) {
        _shouldDrawOnlyFrame = shouldDraw;
        [self setNeedsDisplay:YES];
    }
}

- (void)clear {
    _captureRect = NSZeroRect;
    [self setShouldDrawOnlyFrame:NO];
    [self setNeedsDisplay:YES];
}

#pragma mark -

- (void)drawRect:(NSRect)dirtyRect {
    if (!NSEqualRects(_captureRect, NSZeroRect)) {
        if (_shouldDrawOnlyFrame) {
            [[NSColor grayColor] set];
            [NSBezierPath setDefaultLineCapStyle:NSRoundLineCapStyle];
            [NSBezierPath setDefaultLineJoinStyle:NSRoundLineJoinStyle];
            [NSBezierPath setDefaultLineWidth:4.0];
            [NSBezierPath strokeRect:_captureRect];
        } else {
            [_captureFrameColor set];
            NSRectFill(_captureRect);
        }
        
    }
}

- (void)mouseDown:(NSEvent*)theEvent {
    if (!_shouldDrawOnlyFrame) {
        _captureRect = NSZeroRect;
        _startPoint = [theEvent locationInWindow];
        _startPoint.x = floorf(_startPoint.x);
        _startPoint.y = floorf(_startPoint.y);
    }
}

- (void)mouseDragged:(NSEvent*)theEvent {
    if (!_shouldDrawOnlyFrame) {
        _endPoint = [theEvent locationInWindow];
        _endPoint.x = floorf(_endPoint.x);
        _endPoint.y = floorf(_endPoint.y);
        _captureRect.origin.x = _startPoint.x > _endPoint.x ? _endPoint.x : _startPoint.x;
        _captureRect.origin.y = _startPoint.y > _endPoint.y ? _endPoint.y : _startPoint.y;
        _captureRect.size.width = abs(_startPoint.x - _endPoint.x);
        _captureRect.size.height = abs(_startPoint.y - _endPoint.y);
        [self setNeedsDisplay:YES];
    }
}

- (void)mouseUp:(NSEvent*)theEvent {
    if (!_shouldDrawOnlyFrame) {
        [_delegate startCaptureView:self didChooseCaptureRect:_captureRect];
        _startPoint = NSZeroPoint;
        _endPoint = NSZeroPoint;
    }
}

@end
