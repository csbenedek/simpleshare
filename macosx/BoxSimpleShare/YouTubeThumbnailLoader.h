//
//  YouTubeThumbnailLoader.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 01/10/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface YouTubeThumbnailLoader : NSObject <NSURLConnectionDataDelegate, NSURLConnectionDelegate>

@property (nonatomic, retain) NSMutableData *data;

@property (nonatomic, retain) NSImage *thumbnail;

-(void)loadThumbnailForVideoWithID:(NSString *) ID;

-(void)retryTimerMethod:(NSTimer *)timer;





@end
