//
//  ImgurThumbnailLoader.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 04/10/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ImgurThumbnailLoader : NSObject <NSURLConnectionDataDelegate, NSURLConnectionDelegate>

@property (nonatomic, retain) NSMutableData *data;

@property (nonatomic, retain) NSImage *thumbnail;

-(void)loadThumbnailForImageWithID:(NSString *) ID;

-(void)retryTimerMethod:(NSTimer *)timer;



@end
