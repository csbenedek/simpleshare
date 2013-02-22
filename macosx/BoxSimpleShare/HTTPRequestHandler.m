//
//  HTTPRequestHandler.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "HTTPRequestHandler.h"

#import "HTTPRequest.h"
#import "HTTPFormDataRequest.h"
#import "UploadOperation.h"
#import "ASINetworkQueue.h"



@implementation HTTPRequestHandler

@synthesize queue, progress, maxValue, isUploadQueue;

static HTTPRequestHandler *sharedObject;
static HTTPRequestHandler *sharedObject1;

#pragma mark
#pragma mark Init

- (id) init
{
    if ((self = [super init]))
    {
        queue = [ASINetworkQueue new];
        [queue go];
        
        [queue setShowAccurateProgress:YES];
        
        [queue setDelegate:self];
        [queue setRequestDidFinishSelector:@selector(requestDidFinish:)];
        [queue setRequestDidFailSelector:@selector(requestDidFail:)];
        
        [queue setQueueDidFinishSelector:@selector(queueDidFinish:)];
        
        [queue setDownloadProgressDelegate:self];
        [queue setUploadProgressDelegate:self];
        
        [queue setMaxConcurrentOperationCount:MAX_CONCURRENT_OPERATIONS];
    }
    return self;
}

#pragma mark
#pragma mark Singleton

+ (HTTPRequestHandler *) uploadHandler
{
    if (!sharedObject1)
    {
        sharedObject1 = [HTTPRequestHandler new];
        @synchronized(sharedObject1)
        {
            [[sharedObject1 queue] setMaxConcurrentOperationCount:1];
            sharedObject1.isUploadQueue = YES;
        }
    }
    
    return sharedObject1;
}

+ (HTTPRequestHandler *) sharedHandler
{
    if (!sharedObject)
    {
        sharedObject = [HTTPRequestHandler new];
        @synchronized(sharedObject)
        {
        }
    }
    
    return sharedObject;
}

#pragma mark
#pragma mark Progress Reporter

- (long long) bytesUploadedSoFar
{
    return [queue bytesUploadedSoFar];
}

- (long long) totalBytesToUpload
{
    return [queue totalBytesToUpload];
}

- (long long) bytesDownloadedSoFar
{
    return [queue bytesDownloadedSoFar];
}

- (long long) totalBytesToDownload
{
    return [queue totalBytesToDownload];
}

- (void)setDoubleValue:(double)newProgress
{
    progress = newProgress;
    
    if (isUploadQueue)
        PostNotificationWithObject(UPDATE_STATUS_ITEM, self);
}

- (void)setMaxValue:(double)newMax
{
    maxValue = newMax;
}

#pragma mark
#pragma mark Queue Delegate Methods

// Called when the request receives some data - bytes is the length of that data
//- (void)request:(ASIHTTPRequest *)request didReceiveBytes:(long long)bytes
//{
//    
//}

// Called when the request sends some data
// The first 32KB (128KB on older platforms) of data sent is not included in this amount because of limitations with the CFNetwork API
// bytes may be less than zero if a request needs to remove upload progress (probably because the request needs to run again)
- (void)request:(ASIHTTPRequest *)request didSendBytes:(long long)bytes
{	
    PostNotificationWithObject(QUEUE_PROGRESS, self);
    //SSLog(@"1.didSendBytes %lld", bytes);
}

// Called when a request needs to change the length of the content to download
//- (void)request:(ASIHTTPRequest *)request incrementDownloadSizeBy:(long long)newLength
//{
//    
//}

// Called when a request needs to change the length of the content to upload
// newLength may be less than zero when a request needs to remove the size of the internal buffer from progress tracking
//- (void)request:(ASIHTTPRequest *)request incrementUploadSizeBy:(long long)newLength
//{
//    
//}

- (void) requestDidFinish:(id)request
{
    // Finished - Success 
    
    SSLog(@"request data: %@", [request responseString]);
}

- (void) requestDidFail:(id)request
{
    // Finished - Failure
    
    SSLog(@"request failed: %@", [request error]);
}

- (void) queueDidFinish:(id)queue
{
    SSLog(@"queue finished!");
    [[self queue] setTotalBytesToUpload:0];
    [[self queue] setBytesUploadedSoFar:0];
    if (isUploadQueue)
    {
        PostNotificationWithObject(UPDATE_STATUS_ITEM, UPLOAD_FINISHED);
        PostNotificationWithObject(QUEUE_PROGRESS, UPLOAD_FINISHED);
    }
}

#pragma mark
#pragma mark Add HTTP Operations

- (BOOL) addRequest:(id)request
{
    assert([request isKindOfClass:[HTTPRequest class]] || [request isKindOfClass:[HTTPFormDataRequest class]]);
    
    [queue addOperation:request];
    
    return YES;
}

- (BOOL) addRequest:(id)request isHeadOperation:(BOOL)head
{
    assert([request isKindOfClass:[HTTPRequest class]] || [request isKindOfClass:[HTTPFormDataRequest class]]);
    
    if (head)
    {
        [queue addHEADOperation:request];
        return YES;
    }
    
    return [self addRequest:request];
}

#pragma mark
#pragma mark Memory Management

- (void) deallocUploadHandler
{
    if (self == sharedObject1)
        safe_release_sharedObject(sharedObject1);
}

- (void) deallocSharedHandler
{
    if (self == sharedObject)
        safe_release_sharedObject(sharedObject);
}

- (void) dealloc
{
    if ([queue operationCount] > 0)
    {
        for (id request in [queue operations])
            [request reset];
        
        [queue cancelAllOperations];
    }
    
    [queue reset];
    safe_release(queue);
    
    [super dealloc];
}

@end
