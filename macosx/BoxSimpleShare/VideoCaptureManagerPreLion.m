//
//  VideoCaptureManagerPreLion.m
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/25/11.
//

#import "VideoCaptureManagerPreLion.h"

@interface VideoCaptureManagerPreLion ()

@property (assign) BOOL stopped;

@end

@implementation VideoCaptureManagerPreLion

@synthesize stopped = _stopped;

- (id)init {
    if (self = [super init]) {
        _parametrsDict = [[NSDictionary dictionaryWithObjectsAndKeys:@"avc1",
                                QTAddImageCodecType,
                                [NSNumber numberWithLong:codecHighQuality],
                                QTAddImageCodecQuality,
                                nil] retain];    
        _time = QTMakeTime(1, 10);
    }
    return self;
}

- (void)dealloc {
    [_parametrsDict release];
    [_movie release];
    [super dealloc];
}

- (void)beginNewMovie {
    [_movie release];
    [_tempFileName release];
    _tempFileName = [[NSTemporaryDirectory() stringByAppendingPathComponent:@"movie_tmp"] retain];
    if ([[NSFileManager defaultManager] fileExistsAtPath:_tempFileName]) {
        [[NSFileManager defaultManager] removeItemAtURL:[NSURL fileURLWithPath:_tempFileName] error:NULL];
    }
    
    _movie = [[QTMovie alloc] initToWritableFile:_tempFileName/*@"/Users/kiev/Documents/tmp_simpleshare/movie_tmp.mov"*/ error:NULL];
    [_movie setAttribute:[NSNumber numberWithBool:YES] forKey:QTMovieEditableAttribute];
    self.stopped = YES;
}

- (void)startVideoCaptureInRect:(NSRect)rect screen:(NSScreen*)screen {
    if (!_screenshotManager) {
        _screenshotManager = [[ScreenshotManager alloc] init];
        _screenshotManager.delegate = self;
        
    }
    NSRect screenFrame = [screen frame];
    rect.origin.x -= screenFrame.origin.x;
    rect.origin.y -= screenFrame.origin.y;
    [self beginNewMovie];
    CGDirectDisplayID displayId = [[[screen deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
    [_screenshotManager startContinuoslyCaptureInRect:rect displayId:displayId];
}

- (void)addFrame:(NSImage*)frame timeIntervalSinceLastFrame:(NSTimeInterval)interval {
    assert(_movie != nil);

    if (interval > 0) {
        _time.timeScale = (long)ceil(1.0 / interval);
    } else {
        _time.timeScale = 10;
    }    
//    NSLog(@"qttime is %@", QTStringFromTime(duration));
    [_movie addImage:frame forDuration:_time withAttributes:_parametrsDict];
}

#pragma mark -

- (void)saveCurrentMovieToFile:(NSString*)filename exportFormat:(long)exportFormat {
    self.stopped = YES;
    [_screenshotManager stopContinuoslyCapture];
    assert(_movie != nil);
    NSDictionary* dict = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES], QTMovieFlatten, 
                                                                    [NSNumber numberWithBool:YES], QTMovieExport,
                                                                    [NSNumber numberWithLong:exportFormat], QTMovieExportType,
                                                                    nil];
    NSError* error;
//    filename = @"/Users/kiev/Documents/tmp_simpleshare/movie.mov";
    [_movie writeToFile:filename withAttributes:dict error:&error];
    [[NSFileManager defaultManager] removeItemAtURL:[NSURL fileURLWithPath:_tempFileName] error:NULL];
    [_tempFileName release];
    _tempFileName = nil;
    [self notifySaveFinishedToPath:filename];
}

#pragma mark -

- (void)screenshotManager:(ScreenshotManager*)manager didReceiveNewImage:(NSImage*)image {
    static NSDate* lastDate = nil;
    NSDate* date = [NSDate date];
    
    NSTimeInterval interval = -1;
    if (lastDate) {
        interval = [date timeIntervalSinceDate:lastDate];
    }
    
    /*       [_cachedFrames addObject:image];
     
     if ([_cachedFrames count] == 10) {
     NSLog(@"Flushing");
     //            [_videoManager addFrames:_cachedFrames];
     for (NSImage* img in _cachedFrames) {
     [_videoManager addFrame:img timeIntervalSinceLastFrame:0.1];
     }
     [_cachedFrames removeAllObjects];
     }*/
    
    [self addFrame:image timeIntervalSinceLastFrame:interval];        
    
    //        static int i = 0;
    //        NSData* data = [image TIFFRepresentationUsingCompression:NSTIFFCompressionNone factor:0];
    //        [data writeToFile:[NSString stringWithFormat:@"/Users/kiev/Documents/tmp_simpleshare/img_%d.tiff", i] atomically:NO];
    //        i++;
//    if (lastDate) {
//        NSLog(@"time passed : %.2f", interval);
//    }
    [lastDate release];
    lastDate = [date retain];
}


@end
