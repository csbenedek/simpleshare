//
//  ImgurUploadOperation.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 3/2/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "ImgurUploadOperation.h"
#import "BoxSimpleShareAppDelegate.h"
#import "JSON.h"
#import "HTTPRequestHandler.h"
#import "HTTPFormDataRequest.h"
#import "HTTPRequest.h"
#import "AppConstants.h"
#import "BoxFile.h"
#import "BoxSimpleShareAppDelegate.h"
#import "MainController.h"

@implementation ImgurUploadOperation


- (void) start
{
    
    NSString* clientID = @"312ad7467f09b9b";

	
	
    assert([itemsToUpload count] > 0);
    
    operationStatus = STARTED;
    [self triggerUpdate];
    
    NSString *filePath = [itemsToUpload objectAtIndex: 0]  ;
    uploadFileName = [filePath lastPathComponent];
    NSMutableData* imageData = [NSMutableData dataWithContentsOfFile:filePath];


        // PREPARE FOR UPLOADING
        operationStatus = EXECUTING;
        [self triggerUpdate];
    
        NSURL* url = [NSURL URLWithString: @"https://api.imgur.com/3/upload.json"];
        request = [[HTTPFormDataRequest requestWithURL:url] retain];
		
		[request addRequestHeader:@"Authorization"
							value: [NSString stringWithFormat:@"Client-ID %@", clientID]];

        [request setQueue:[[HTTPRequestHandler uploadHandler] queue]];
        [request setRequestMethod:@"POST"];
        [request setPostBody:imageData];
        [request addMulticastDelegate:self];

        // To mark the start
        PostNotificationWithObject(QUEUE_PROGRESS, self);
        [self triggerUpdate];
        
        [[HTTPRequestHandler uploadHandler] addRequest:request];
        
 
    
    safe_release(filePath);
   // fileName = nil;
    
    // PREPARE FOR UPLOADING
    operationStatus = QUEUED;
    [self triggerUpdate];
    
    while (![self isFinished]) 
    {
        sleep(5);
    }
}

- (void) requestFinished:(id)request1
{
    
    NSLog(@"%@",[request1 responseString]);
    NSDictionary* dict = [[request1 responseString]JSONValue]  ;
    
    if([[dict objectForKey:@"status"] intValue] != 200 && [[dict objectForKey:@"success"] intValue] != 1)
    {
        [self requestFailed:request1];
        return;
    }
    
    
    NSString* link = [[dict objectForKey:@"data" ] objectForKey:@"link"];
    
    
    BoxFile* file = [BoxFile new];
    
    [file setUploadStatus:@"upload_ok"];
    
    [file setFileName:[self FileName]];
    [file setShortURL:link];
    [file setIsShared:YES];
    operationStatus = COMPLETED;
    [self triggerUpdate];
    
    MainController *controller = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    // Copy URL to clipboard
    if (controller.copy_url_to_clipboard_check)
    {
        NSString* url = nil;
        
        if ([file shortURL])
        {
            url = [file shortURL];
            
        }
        else
        {
            
            url = [file publicName];
        }
        
        if (controller.shorten_links_check)
        {
            url = URLShortner( url );
        }
        
        CopyURLToClipBoardForType(url, NSStringPboardType, nil);
    }
    
    
    [[[BoxSimpleShareAppDelegate sharedDelegate] filesUploadedInSession] addObject:file];
    [[BoxSimpleShareAppDelegate sharedDelegate] updateHistoryMenu]; // update history menu
    

    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Complete" withDescription:[NSString stringWithFormat:@"Finish uploading file '%@'", [self FileName]]];

}

@end
