//
//  VideoCaptureManagerBase.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 12/8/11.
//

/* 
 Base class for screen video capture. Does not implement any functionality but defines interface
 for video capture manager. There are avfoundation framework on 10.7 Lion which gives screen video 
 capture, so for 10.7 there are separate implemi entation.
 */

@class VideoCaptureManagerBase;

@protocol VideoCaptureManagerDelegate <NSObject>

- (void)videoCaptureManager:(VideoCaptureManagerBase*)manager didSaveMovieToPath:(NSString*)path;

@end

@interface VideoCaptureManagerBase : NSObject {

    id<VideoCaptureManagerDelegate> _delegate;
}

@property (nonatomic, assign) id<VideoCaptureManagerDelegate> delegate;

- (void)startVideoCaptureInRect:(NSRect)rect screen:(NSScreen*)screen;
- (void)saveCurrentMovieToFile:(NSString*)filename exportFormat:(long)exportFormat;
- (void)notifySaveFinishedToPath:(NSString*)path;

@end
