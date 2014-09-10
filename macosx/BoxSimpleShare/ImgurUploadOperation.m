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
#import "Utilities.h"
#import "NSImage+Additions.h"

@implementation ImgurUploadOperation


- (void) start
{
    
    NSString* clientID = @"312ad7467f09b9b";

	
	
    assert([itemsToUpload count] > 0);
    
    operationStatus = STARTED;
    [self triggerUpdate];
    
    NSString *filePath = [itemsToUpload objectAtIndex: 0]  ;
    uploadFileName = [filePath lastPathComponent];
    
    [uploadFileNames removeAllObjects]; // used to delete files after uploading
    [uploadFileNames addObject:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], @"zipped", filePath, @"path", nil]];
    
    NSMutableData* imageData = nil;
    NSImage* image = [[NSImage alloc] initWithContentsOfFile:filePath];
    if(![Utilities isRetinalSupported])
    {
        imageData = [NSMutableData dataWithContentsOfFile:filePath]; //[NSMutableData dataWithData:[image TIFFRepresentation]];//[NSMutableData dataWithContentsOfFile:filePath];
    }
    else
    {
        NSBitmapImageRep* myBitmapImageRep;
        NSSize imageSize = [image size];
        NSSize halfSize = NSMakeSize(imageSize.width /2, imageSize.height/2);
        NSImage * halfSizeImage =    [image cw_imageResizedToSize:halfSize withInterpolationQuality:kCGInterpolationHigh];

//        [halfSizeImage lockFocus];
//        NSRect imageRect = NSMakeRect(0, 0, halfSizeImage.size.width, halfSizeImage.size.height);
//        myBitmapImageRep = [[[NSBitmapImageRep alloc] initWithFocusedViewRect:imageRect] autorelease];
//        [halfSizeImage unlockFocus];
//
//        myBitmapImageRep = [NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];
//        CGFloat imageCompression = .5;
//        NSDictionary* jpegOptions = [NSDictionary dictionaryWithObjectsAndKeys:
//                                     [NSNumber numberWithDouble:imageCompression], NSImageCompressionFactor,
//                                     [NSNumber numberWithBool:NO], NSImageProgressive,
//                                     nil];
//        
//        NSData* data =  [myBitmapImageRep representationUsingType:NSPNGFileType properties:jpegOptions];

       
        
  
        imageData =[NSMutableData dataWithData: [halfSizeImage TIFFRepresentation]];
        // [newRep autorelease];
    }

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
    
    [file setFileName:[[self FileName] copy]];
    [file setShortURL:link];
    [file setIsShared:YES];
    [file setIsImgur:YES];
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
        
//        if (controller.shorten_links_check)
//        {
//            url = URLShortner( url );
//        }
        
        CopyURLToClipBoardForType(url, NSStringPboardType, nil);
    }
    
    BOOL deleteFiles = controller.delete_screenshot_after_upload_check || controller.delete_all_after_upload_check;
    
    if (deleteFiles) {
        [self deleteLastUploadedFilesZippedOnly: NO];
    }
    
    [[[BoxSimpleShareAppDelegate sharedDelegate] filesUploadedInSession] addObject:file];
    [[BoxSimpleShareAppDelegate sharedDelegate] updateHistoryMenu]; // update history menu
    

    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Complete" withDescription:[NSString stringWithFormat:@"Finish uploading file '%@'", [self FileName]]];

}

@end
