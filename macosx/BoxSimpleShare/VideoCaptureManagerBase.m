//
//  VideoCaptureManagerBase.m
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 12/8/11.
//

#import "VideoCaptureManagerBase.h"

@implementation VideoCaptureManagerBase

@synthesize delegate = _delegate;

- (void)notifySaveFinishedToPath:(NSString*)path {
    [self.delegate videoCaptureManager:self didSaveMovieToPath:path];
}

- (void)startVideoCaptureInRect:(NSRect)rect screen:(NSScreen*)screen {
    
}

- (void)saveCurrentMovieToFile:(NSString*)filename exportFormat:(long)exportFormat {
    
}

@end

