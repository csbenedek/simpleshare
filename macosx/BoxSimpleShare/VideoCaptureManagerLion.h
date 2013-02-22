//
//  VideoCaptureManagerLion.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 12/8/11.
//

#import "VideoCaptureManagerBase.h"

#import <QTKit/QTKit.h>

@interface VideoCaptureManagerLion : VideoCaptureManagerBase {
    
    QTMovie*    _movie;
    
    id          _session;
    id          _movieFileOutput;
    NSString*   _tempFileName;
}

@end
