//
//  HTTPRequest.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "HTTPRequest.h"
#import "HTTPRequestHandler.h"

@implementation HTTPRequest

- (id) initWithURL:(NSURL *)requestURL
{
    self = [super initWithURL:requestURL];
    
    [self setDownloadProgressDelegate:self];
    [self setUploadProgressDelegate:self];
    [self setTimeOutSeconds:30];
    
    return self;
}

- (void) addMulticastDelegate:(id)obj
{
    if (!multicastDelegate) multicastDelegate = [NSMutableArray new];
    
    if (![multicastDelegate containsObject:obj])
        [multicastDelegate addObject:obj];
}

- (void) removeMulticastDelegate:(id)obj
{
    if ([multicastDelegate containsObject:obj])
        [multicastDelegate removeObject:obj];
}

#pragma mark
#pragma mark Delegate Proper Assignment Check

- (void) setDelegate:(id)obj
{
    if (![obj isKindOfClass:[HTTPRequestHandler class]])
    {
        @throw [NSException exceptionWithName:@"Assigning not allowed" reason:@"Use 'addMulticastDelegate' method instead." userInfo:nil];
        
        return;
    }
    self.delegate = obj;
}

- (void) reset
{
    [multicastDelegate removeAllObjects];
    [self cancel];
}

#pragma mark
#pragma mark Delegate Overridden 

- (void) requestStarted
{
    [super requestStarted];
    
    if ([multicastDelegate count] > 0)
    {
        for (id obj in multicastDelegate)
        {
            if ([obj respondsToSelector:didStartSelector])
                [obj performSelector:didStartSelector withObject:self];
        }
    } 
}

- (void) requestFinished
{
    [super requestFinished];
    
    if ([multicastDelegate count] > 0)
    {
        for (id obj in multicastDelegate)
        {
            if ([obj respondsToSelector:didFinishSelector])
                [obj performSelector:didFinishSelector withObject:self];
        }
    }
}

- (void) failWithError:(NSError *)theError
{
    [super failWithError:theError];
    
    if ([multicastDelegate count] > 0)
    {
        for (id obj in multicastDelegate)
        {
            if ([obj respondsToSelector:didFailSelector])
                [obj performSelector:didFailSelector withObject:self];
        }
    }
}

- (void) updateDownloadProgress
{
    [super updateDownloadProgress];
    
    if ([multicastDelegate count] > 0 && progressCheck)
    {
        for (id obj in multicastDelegate)
        {
            unsigned long long bytesReadSoFar = totalBytesRead + partialDownloadSize;
            unsigned long long value = 0;
            value = bytesReadSoFar - lastBytesRead;
            
            if ([obj respondsToSelector:@selector(request:didReceiveBytes:)])
                [obj performSelector:@selector(request:didReceiveBytes:) withObject:self withObject:(NSNumber *)&value];
        }
    }
}

- (void) updateUploadProgress
{
    [super updateUploadProgress];
    
    if ([multicastDelegate count] > 0 && progressCheck)
    {
        unsigned long long value = 0;
        value = totalBytesSent - lastBytesSent;
        
        for (id obj in multicastDelegate)
        {
            if ([obj respondsToSelector:@selector(request:didSendBytes:)])
                [obj performSelector:@selector(request:didSendBytes:) withObject:self withObject:(NSNumber *)&value];
        }
    }
}

#pragma mark
#pragma mark Memory Management

- (void) dealloc
{
    if (multicastDelegate && [multicastDelegate count] > 0)
        [multicastDelegate removeAllObjects];
    
    [multicastDelegate release];
    
    [super dealloc];
}

@end
