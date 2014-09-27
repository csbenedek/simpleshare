//
//  BoxNetFileInfoLoader.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 26/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BoxNetFileInfoLoader : NSObject <NSURLConnectionDelegate, NSURLConnectionDataDelegate>

@property(nonatomic, retain) NSImage *thumbnailImage;
@property (nonatomic, retain) NSURLConnection *loadThumbnailURLConnection;

@property (nonatomic, retain) NSMutableData *thumbnailImageData;






-(void)loadThumbnailImageForID:(NSString *)ID;

-(void)retryTimerMethod:(NSTimer *)timer;






@end
