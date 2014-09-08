//
//  UploadOperation.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/8/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "ASIHTTPRequestDelegate.h"
#import "ASIProgressDelegate.h"
#import "AppConstants.h"

// NOTIFICATIONS 

#define REQUEST_PROGRESS @"REQUEST_PROGRESS"
#define QUEUE_PROGRESS @"QUEUE_PROGRESS"

@interface UploadOperation : NSOperation <ASIHTTPRequestDelegate, ASIProgressDelegate> {
@public
    
    NSMutableArray *itemsToUpload;
    NSString *uploadToFolder;
    OperationStatus operationStatus;
    
    id request;
    
    NSString *uploadFileName;
    
    NSMutableArray *uploadFileNames;
    
    BOOL isZipped;
    BOOL isScreenshot;
}

@property (retain, nonatomic) NSMutableArray *itemsToUpload;
@property (retain, nonatomic) NSString *uploadToFolder;

@property (nonatomic) BOOL isScreenshot;
@property (nonatomic) OperationStatus operationStatus;

- (void) addFiles:(id)files;

- (long long) bytesUploadedSoFar;
- (long long) totalBytesToUpload;
- (long long) bytesDownloadedSoFar;
- (long long) totalBytesToDownload;

- (NSString *) FileName;
- (NSString *) getCurrentStatus;
- (void) triggerUpdate;
- (void)deleteLastUploadedFilesZippedOnly:(BOOL)zippedOnly;
@end
