//
//  HTTPFormDataRequest.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "HTTPFormDataRequest.h"
#import "HTTPRequestHandler.h"

@implementation HTTPFormDataRequest

- (id) initWithURL:(NSURL *)requestURL
{
    if (self = [super initWithURL:requestURL]) {
    
        [self setTimeOutSeconds:30];    
        [self setDownloadProgressDelegate:self];
        [self setUploadProgressDelegate:self];
    }    
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
    [super setDelegate:obj];
    
    if (![obj isKindOfClass:[HTTPRequestHandler class]])
    {
        @throw [NSException exceptionWithName:@"Assigning not allowed" reason:@"Use 'addMulticastDelegate' method instead." userInfo:nil];
        
        return;
    }
}

- (void) reset
{
    [multicastDelegate removeAllObjects];
    [self cancel];
}

- (void) setDownloadProgressDelegate:(id)obj
{
    [super setDownloadProgressDelegate:obj];
    
    if (![obj isKindOfClass:[self class]])
    {
        @throw [NSException exceptionWithName:@"Assigning not allowed" reason:@"Use 'addMulticastDelegate' method instead." userInfo:nil];
        
        return;
    }
}

- (void) setUploadProgressDelegate:(id)obj
{
    [super setUploadProgressDelegate:obj];
    
    if (![obj isKindOfClass:[self class]])
    {
        @throw [NSException exceptionWithName:@"Assigning not allowed" reason:@"Use 'addMulticastDelegate' method instead." userInfo:nil];
        
        return;
    }
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

- (void) request:(id)request1 didReceiveBytes:(long long)bytes
{
    if ([multicastDelegate count] > 0)
    {
        for (id obj in multicastDelegate)
        {
            SEL selector = @selector(request:didReceiveBytes:);
            if ([obj respondsToSelector:selector])
            {
                NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[obj methodSignatureForSelector:selector]];
                [invocation setTarget:obj];
                [invocation setSelector:selector];
                [invocation setArgument:request1 atIndex:2];
                [invocation setArgument:&bytes atIndex:3];
                
                [invocation invoke];
            }
        }
    }
}

- (void) request:(id)request1 incrementDownloadSizeBy:(long long)newLength
{
    if ([multicastDelegate count] > 0)
    {
        for (id obj in multicastDelegate)
        {
            SEL selector = @selector(request:incrementDownloadSizeBy:);
            if ([obj respondsToSelector:selector])
            {
                NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[obj methodSignatureForSelector:selector]];
                [invocation setTarget:obj];
                [invocation setSelector:selector];
                [invocation setArgument:request1 atIndex:2];
                [invocation setArgument:&newLength atIndex:3];
                
                [invocation invoke];
            }
        }
    }
}

- (void) request:(id)request1 didSendBytes:(long long)bytes
{
    if ([multicastDelegate count] > 0)
    {
        for (id obj in multicastDelegate)
        {
            SEL selector = @selector(request:didSendBytes:);
            if ([obj respondsToSelector:selector])
            {
                NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[obj methodSignatureForSelector:selector]];
                [invocation setTarget:obj];
                [invocation setSelector:selector];
                [invocation setArgument:request1 atIndex:2];
                [invocation setArgument:&bytes atIndex:3];
                
                [invocation invoke];
            }
        }
    }
}

- (void) request:(id)request1 incrementUploadSizeBy:(long long)newLength
{
    if ([multicastDelegate count] > 0)
    {
        for (id obj in multicastDelegate)
        {
            SEL selector = @selector(request:incrementUploadSizeBy:);
            if ([obj respondsToSelector:selector])
            {
                NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[obj methodSignatureForSelector:selector]];
                [invocation setTarget:obj];
                [invocation setSelector:selector];
                [invocation setArgument:request1 atIndex:2];
                [invocation setArgument:&newLength atIndex:3];
                                            
                [invocation invoke];
            }
        }
    }
}

- (void) setDoubleValue:(double)newProgress
{
    if ([multicastDelegate count] > 0)
    {
        for (id obj in multicastDelegate)
        {
            SEL selector = @selector(setDoubleValue:);
            if ([obj respondsToSelector:selector])
            {
                NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[obj methodSignatureForSelector:selector]];
                [invocation setTarget:obj];
                [invocation setSelector:selector];
                [invocation setArgument:&newProgress atIndex:3];
                
                [invocation invoke];
            }
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
