//
//  VideoCaptureManagerPreLion.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/25/11.
//

#import <QTKit/QTKit.h>

#import "VideoCaptureManagerBase.h"
#import "ScreenshotManager.h"

@interface VideoCaptureManagerPreLion : VideoCaptureManagerBase <ScreenshotManagerDelegate> {
    
    QTMovie*            _movie;
    ScreenshotManager*  _screenshotManager;
    
    NSDictionary*       _parametrsDict;
    QTTime              _time;
    BOOL                _stopped;
    NSString*           _tempFileName;
    
}

@end
