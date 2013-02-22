//
//  StartVideoCaptureController.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/24/11.
//

#import "StartVideoCaptureView.h"
#import "VideoCaptureManagerBase.h"


@class StartVideoCaptureController;

@protocol VideoCaptureControllerDelegate <NSObject>

- (void)videoCaptureController:(StartVideoCaptureController*)controller didCaptureVideoToPath:(NSString*)path;
- (void)videoCaptureControllerDidFailedStartVideoCapture:(StartVideoCaptureController*)controller;
- (void)videoCaptureControllerDidStartVideoCapture:(StartVideoCaptureController*)controller;
- (void)videoCapturControlllerDidPressStopCapture:(StartVideoCaptureController*)controller;

@end

@interface StartVideoCaptureController : NSWindowController <StartVideoCaptureViewDelegate, VideoCaptureManagerDelegate> {

//    NSWindow*                           _transparentWindow;
    
    NSMutableArray*                     _transparentWindows;
    NSMutableArray*                     _captureViews;
    StartVideoCaptureView*              _actualView; // non retained
    
    NSWindow*                           _stopCaptureWindow;
    NSButton*                           _stopCaptureButton;
//    StartVideoCaptureView*              _startCaptureView;
        
    VideoCaptureManagerBase*            _videoCaptureManager;
    
    id<VideoCaptureControllerDelegate>  _delegate;
        
    BOOL                                _isCapturing;
}

@property (nonatomic, assign) id<VideoCaptureControllerDelegate> delegate;
           
- (void)clear;
- (void)stopVideoRecording;
- (BOOL)isCapturing;

- (void)showWindows;
- (void)hideAllWindows;


@end
