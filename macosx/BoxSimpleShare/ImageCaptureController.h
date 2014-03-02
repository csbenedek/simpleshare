//
//  ImageCaptureController.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 11/29/13.
//  Copyright (c) 2013 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "StartImageCaptureView.h"


@class ImageCaptureController;
@protocol ImageCaptureControllerDelegate <NSObject>

- (void)imageCaptureController:(ImageCaptureController*)controller didCaptureVideoToPath:(NSString*)path;
- (void)imageCaptureControllerDidFailedCapture:(ImageCaptureController*)controller;

@end
@interface ImageCaptureController : NSWindowController<StartImageCaptureViewDelegate>
{
    
    
    NSMutableArray*                     _transparentWindows;
    NSMutableArray*                     _captureViews;
    StartImageCaptureView*              _actualView; // non retained
    BOOL                                _isCapturing;
    id<ImageCaptureControllerDelegate>  _delegate;

}

@property (nonatomic, assign) id<ImageCaptureControllerDelegate> delegate;

- (void)clear;
- (void)stopImageCapture;

- (void)showWindows;
- (void)hideAllWindows;
-(void)captureFullScreen;
void CGImageWriteToFile(CGImageRef image, NSString *path);
@end
