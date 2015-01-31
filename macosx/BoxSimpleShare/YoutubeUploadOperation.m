//
//  YoutubeUploadOperation.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 3/23/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <GTL/GTLUtilities.h>
#import <GTL/GTMHTTPUploadFetcher.h>

#import "GTLYoutube.h"
#import "GTL/GTLUtilities.h"
#import "GTL/GTMHTTPUploadFetcher.h"
#import "GTL/GTMHTTPFetcherLogging.h"
#import "GTL/GTMOAuth2WindowController.h"
#import "GTLYouTube.h"
#import <GTL/GTMOAuth2WindowController.h>
#import "YoutubeUploadOperation.h"
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
#import "YoutubeUploadWindowController.h"
#import "YoutubeAuthenticateManager.h"


@implementation YoutubeUploadOperation


- (void) start
{
    
    // PREPARE FOR UPLOADING
    operationStatus = EXECUTING;
    [self triggerUpdate];
    
    
    NSString *filePath = [itemsToUpload objectAtIndex: 0]  ;
    uploadFileName = [[filePath lastPathComponent] copy];
    
    [uploadFileNames removeAllObjects]; // used to delete files after uploading
    [uploadFileNames addObject:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], @"zipped", filePath, @"path", nil]];
   
    // To mark the start
    PostNotificationWithObject(QUEUE_PROGRESS, self);
    [self triggerUpdate];
    
    
    // PREPARE FOR UPLOADING
    operationStatus = QUEUED;
//    [self triggerUpdate];
    YoutubeUploadWindowController* youtubeController = [YoutubeUploadWindowController sharedWindowController];
    //[youtubeController showWindow:nil];
    NSWindow* window = [[BoxSimpleShareAppDelegate sharedDelegate] statusBarWindow];
    [youtubeController beginSheetModalForWindow:window completionHandler:^(NSUInteger result) {
        
        if (result == 0) {
            self.operationStatus = CANCELED;
        }
        else
        {
            _title = youtubeController._uploadTitleField.stringValue;
            _description = youtubeController._uploadDescriptionField.stringValue;
            _tag = youtubeController._uploadTagsField.stringValue;
            _privateStatus = [youtubeController._uploadPrivacyPopup titleOfSelectedItem];
            [self uploadVideoFile];
        }
    }];
    
    while (![self isFinished])
    {
        sleep(5);
    }

}
- (GTLServiceYouTube *)youTubeService {
    static GTLServiceYouTube *service;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        service = [[GTLServiceYouTube alloc] init];
        
        // Have the service object set tickets to fetch consecutive pages
        // of the feed so we do not need to manually fetch them.
        service.shouldFetchNextPages = YES;
        
        // Have the service object set tickets to retry temporary error conditions
        // automatically.
        service.retryEnabled = YES;
        service.authorizer = [YoutubeAuthenticateManager shareManager].authenticate;
    });
    return service;
}

-(void)startUpload
{
    operationStatus = UPLOADING;
    PostNotificationWithObject(REQUEST_PROGRESS, self);
    [self triggerUpdate];
    
}

- (NSWindow *)ESSYouTubeNeedsWindowToAttachTo:(ESSYouTube *)youtube
{
	return     [[BoxSimpleShareAppDelegate sharedDelegate] statusBarWindow];

}

-(void)onError:(NSString*)description {
    
    NSLog(@"error:%@", description);
}



- (void)uploadCompleted:(NSURL *)url
{
    BoxFile* file = [BoxFile new];
    
    [file setUploadStatus:@"upload_ok"];
    
    [file setFileName:[self FileName]];
    [file setShortURL:[url absoluteString]];
    [file setIsShared:YES];
    [file setIsYouTube:TRUE];
    
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
    
    
    BOOL deleteFiles = controller.delete_all_after_upload_check;
    
    if (deleteFiles) {
        [self deleteLastUploadedFilesZippedOnly: NO];
    }
    [[[BoxSimpleShareAppDelegate sharedDelegate] filesUploadedInSession] addObject:file];
    [[BoxSimpleShareAppDelegate sharedDelegate] updateHistoryMenu]; // update history menu
    
    
    //post notification to update table in NewDesign
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"NewHistoryElementNotification" object:nil userInfo:nil];
    
    
    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Complete" withDescription:[NSString stringWithFormat:@"Finish uploading file '%@'", [self FileName]]];
}


-(void)didSendBytes
{
      PostNotificationWithObject(UPDATE_STATUS_ITEM, self);

}

- (void)uploadVideoFile {
    // Collect the metadata for the upload from the user interface.
    
    // Status.
    GTLYouTubeVideoStatus *status = [GTLYouTubeVideoStatus object];
    status.privacyStatus = _privateStatus;
    
    // Snippet.
    GTLYouTubeVideoSnippet *snippet = [GTLYouTubeVideoSnippet object];
    snippet.title = _title;
    NSString *desc = _description;
    if ([desc length] > 0) {
        snippet.descriptionProperty = desc;
    }
    NSString *tagsStr = _tag;
    if ([tagsStr length] > 0) {
        snippet.tags = [tagsStr componentsSeparatedByString:@","];
    }
    snippet.categoryId = _categoty;
//    if ([_uploadCategoryPopup isEnabled]) {
//        NSMenuItem *selectedCategory = [_uploadCategoryPopup selectedItem];
//        snippet.categoryId = [selectedCategory representedObject];
//    }
    
    GTLYouTubeVideo *video = [GTLYouTubeVideo object];
    video.status = status;
    video.snippet = snippet;
    
    [self uploadVideoWithVideoObject:video
             resumeUploadLocationURL:nil];
}
- (void)uploadVideoWithVideoObject:(GTLYouTubeVideo *)video
           resumeUploadLocationURL:(NSURL *)locationURL {
    // Get a file handle for the upload data.
    NSString *path =    [itemsToUpload objectAtIndex: 0];//[_uploadPathField stringValue];
    NSString *filename = [path lastPathComponent];
    NSFileHandle *fileHandle = [NSFileHandle fileHandleForReadingAtPath:path];
    if (fileHandle) {
        NSString *mimeType = [self MIMETypeForFilename:filename
                                       defaultMIMEType:@"video/mp4"];
        GTLUploadParameters *uploadParameters =
        [GTLUploadParameters uploadParametersWithFileHandle:fileHandle
                                                   MIMEType:mimeType];
        uploadParameters.uploadLocationURL = locationURL;
        
        GTLQueryYouTube *query = [GTLQueryYouTube queryForVideosInsertWithObject:video
                                                                            part:@"snippet,status"
                                                                uploadParameters:uploadParameters];
        
        GTLServiceYouTube *service = self.youTubeService;
        
        [self startUpload];
        [[NSOperationQueue mainQueue] addOperationWithBlock:^{
            
        
        _uploadFileTicket = [service executeQuery:query
                                completionHandler:^(GTLServiceTicket *ticket,
                                                    GTLYouTubeVideo *uploadedVideo,
                                                    NSError *error) {
                                    // Callback
                                    _uploadFileTicket = nil;
                                    if (error == nil) {
                                        NSURL* _url = [NSURL URLWithString:[NSString stringWithFormat:@"http://www.youtube.com/watch?v=%@",uploadedVideo.identifier]];
                                        
                                        [self uploadCompleted:_url];
                                        
                                    } else {
//                                        [self displayAlert:@"Upload Failed"
//                                                    format:@"%@", error];
                                        [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Failed" withDescription:[NSString stringWithFormat:@"Failed to upload file '%@'", [self FileName]]];
                                        
                                        operationStatus = FAILED;
                                        PostNotificationWithObject(REQUEST_PROGRESS, self);
                                        [self triggerUpdate:2];

                                    }
                                    

                                }];
        
        _uploadFileTicket.uploadProgressBlock = ^(GTLServiceTicket *ticket,
                                                  unsigned long long numberOfBytesRead,
                                                  unsigned long long dataLength) {
            totalBytesUploadedSofar = numberOfBytesRead;
            totalBytesUpload = dataLength;
            
            [self didSendBytes];
        };
        }];
        

    } else {
        // Could not read file data.
        //[self displayAlert:@"File Not Found" format:@"Path: %@", path];
        [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Failed" withDescription:[NSString stringWithFormat:@"Failed to upload file '%@'", [self FileName]]];
        operationStatus = FAILED;
        PostNotificationWithObject(REQUEST_PROGRESS, self);
        [self triggerUpdate:2];

    }
   
}
- (NSString *)MIMETypeForFilename:(NSString *)filename
                  defaultMIMEType:(NSString *)defaultType {
    NSString *result = defaultType;
    NSString *extension = [filename pathExtension];
    CFStringRef uti = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension,
                                                            (__bridge CFStringRef)extension, NULL);
    if (uti) {
        CFStringRef cfMIMEType = UTTypeCopyPreferredTagWithClass(uti, kUTTagClassMIMEType);
        if (cfMIMEType) {
            result = CFBridgingRelease(cfMIMEType);
        }
        CFRelease(uti);
    }
    return result;
}

- (long long) bytesUploadedSoFar
{
    return totalBytesUploadedSofar;
}

- (long long) totalBytesToUpload
{
    return totalBytesUpload;
}
@end
