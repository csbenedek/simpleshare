//
//  HTTPRequestHandler.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "ASIProgressDelegate.h"

@class ASINetworkQueue;

@interface HTTPRequestHandler : NSObject <ASIProgressDelegate> {
    
    ASINetworkQueue *queue;
    double progress;
    double maxValue;
    
    BOOL isUploadQueue;
}

@property (retain, nonatomic) ASINetworkQueue *queue;

@property (nonatomic) double progress;
@property (nonatomic) double maxValue;

@property (nonatomic) BOOL isUploadQueue;

+ (HTTPRequestHandler *) uploadHandler;
+ (HTTPRequestHandler *) sharedHandler;

- (BOOL) addRequest:(id)request;
- (BOOL) addRequest:(id)request isHeadOperation:(BOOL)head;

- (long long) bytesUploadedSoFar;
- (long long) totalBytesToUpload;
- (long long) bytesDownloadedSoFar;
- (long long) totalBytesToDownload;

- (void) deallocUploadHandler;
- (void) deallocSharedHandler;

@end
