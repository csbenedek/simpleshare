//
//  StartImageCaptureView.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 11/29/13.
//  Copyright (c) 2013 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>


@class StartImageCaptureView;
@protocol StartImageCaptureViewDelegate <NSObject>

- (void)startCaptureView:(StartImageCaptureView*)view didChooseCaptureRect:(NSRect)rect;

@end
@interface StartImageCaptureView : NSView
{
    
    
    NSPoint                             _startPoint;
    NSPoint                             _endPoint;
    NSRect                              _captureRect;
    NSColor*                            _captureFrameColor;
    id<StartImageCaptureViewDelegate>   _delegate;
    
    
    BOOL                                _shouldDrawOnlyFrame;

}

@property (nonatomic, assign) id<StartImageCaptureViewDelegate> delegate;

- (void)setShouldDrawOnlyFrame:(BOOL)shouldDraw;
- (void)clear;
@end
