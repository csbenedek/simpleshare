//
//  UploadOperation.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/8/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "UploadOperation.h"
#import "GDataXMLNode.h"
#import "MainController.h"
#import "HTTPFormDataRequest.h"
#import "HTTPRequest.h"
#import "CkoZip.h"

#import "BoxFile.h"
#import "HTTPRequestHandler.h"
#import "BoxNetHandler.h"
#import "BoxSimpleShareAppDelegate.h"
#import "OAuth2Client.h"
#import "JSON.h"

@interface UploadOperation ()

- (void)triggerUpdate;
- (void)triggerUpdate:(NSTimeInterval)interval;
- (void)deleteLastUploadedFilesZippedOnly:(BOOL)zippedOnly;
- (void)moveFileToTrash:(NSString*)filePath isZipped:(BOOL)zipped;

@end

@implementation UploadOperation

@synthesize itemsToUpload;
@synthesize uploadToFolder;
@synthesize operationStatus;
@synthesize isScreenshot;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
        
        itemsToUpload = [NSMutableArray new];
        uploadFileNames = [NSMutableArray new];
    }
    
    return self;
}

- (NSString *) copyToTmpForCompression
{
	// date
	NSDateFormatter* dateFormatter = [[NSDateFormatter alloc] init];
	[dateFormatter setDateFormat:@"yyyy-MM-dd"];
	NSString* dateString = [dateFormatter stringFromDate:[NSDate date]];
	// time
	[dateFormatter setDateFormat:@"hh.mm.ss a"];
	NSString* timeString = [dateFormatter stringFromDate:[NSDate date]];
	[dateFormatter release];
        
    NSString *dest_folder_name = [NSString stringWithFormat:@"Archive_%@_%@", dateString, timeString];
    [[NSFileManager defaultManager] createDirectoryAtPath:[TMP_PATH stringByAppendingPathComponent:dest_folder_name] withIntermediateDirectories:YES attributes:nil error:nil];
    NSString *tmpFolder = nil;
    
    for (id item in itemsToUpload)
    {
        if ([item isKindOfClass:[NSURL class]]) 
        {
            tmpFolder = [item path];
        }
        else
        {
            tmpFolder = item;
        }
        
        NSError *error = nil;
        NSString *folderName = [NSString stringWithString:[tmpFolder lastPathComponent]];
        NSString *dest = [[TMP_PATH stringByAppendingPathComponent:dest_folder_name] stringByAppendingPathComponent:folderName];
        [[NSFileManager defaultManager] copyItemAtPath:item toPath:dest error:&error];
    }
    
    return [[NSString stringWithFormat:@"%@", [TMP_PATH stringByAppendingPathComponent:dest_folder_name]] retain];
}

- (void) start
{
	BOOL hasFolder = NO;
	
	NSFileManager* fileManager = [NSFileManager defaultManager];
	for (id item in itemsToUpload)
	{
		BOOL isDir = NO;
		if ([fileManager fileExistsAtPath:item isDirectory:&isDir])
		{
			if (isDir)
			{
				hasFolder = YES;
				break;
			}
		}
	}
	
    assert([itemsToUpload count] > 0);

    operationStatus = STARTED;
    [self triggerUpdate];
    
    BOOL success = NO;
    NSString *fileName = nil;
    NSString *filePath = nil;
    
    if ([itemsToUpload count] > 1 || hasFolder)
    {
//        [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Notification" withDescription:@"Creating archive for uploading"];
        
        // DO ZIP THEN UPLOAD
        operationStatus = ZIPPING;
        [self triggerUpdate];
        
        safe_release(filePath);
        filePath = [self copyToTmpForCompression];
        fileName = [NSString stringWithFormat:@"%@.%@", [filePath lastPathComponent], @"zip"];
		
        NSMutableString *strOutput = [NSMutableString new]; // give size of file
        CkoZip *zip = [[CkoZip alloc] init];
        
        // UNLOCK CHILKAT
        {
            success = [zip UnlockComponent: @"ZIPT34MB34N_0F616ABFp16E"];
            if (success != YES) {
                [strOutput appendString: zip.LastErrorText];
                [strOutput appendString: @"\n"];
            }
        }
        
        // CREATE BLANK ARCHIVE FOR WRITING DATA IN TO
        if (success)
        {
            success = [zip NewZip:[filePath stringByAppendingPathComponent:fileName]]; // Archive Path
            if (success != YES) {
                [strOutput appendString: zip.LastErrorText];
                [strOutput appendString: @"\n"];
            }
        }
        
        // RECURSIVELY SELECT ALL FILES AND FOLDERS
        if (success)
        {
            success = [zip AppendFiles: [filePath stringByAppendingPathComponent:@"*"]  recurse:YES]; //TMP FOLDER NAME
            if (success != YES) {
                [strOutput appendString: zip.LastErrorText];
                [strOutput appendString: @"\n"];
            }
        }
        
        // WRITE COMPRESSED DATA TO FILES
        if (success)
        {
            success = [zip WriteZipAndClose];
            if (success != YES) {
                [strOutput appendString: zip.LastErrorText];
                [strOutput appendString: @"\n"];
            }
        }
        
        safe_release(zip);
        isZipped = YES;
        
        if (!success) 
        {
            DbgLog(@"ZIP %@", strOutput);
//            [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Notification" withDescription:[NSString stringWithFormat:@"Failed to create archive uploading failed with error '%@'", strOutput]];
            
            safe_release(filePath);
            fileName = nil; 
            safe_release(strOutput);
            
            operationStatus = FAILED;
            
            return; 
        }
        else
            safe_release(strOutput);
    }
    else
    {
        success = NO;
    }
    
    // ADD REQUEST FOR UPLOADING
    {        
        // PREPARE FOR UPLOADING
        operationStatus = EXECUTING;
        [self triggerUpdate];
        
        id fileURL = nil;
        
        if (success)
        {
            // File url should be the archive path
            fileURL = [filePath stringByAppendingPathComponent:fileName];
        }
        else
        {
            if ([[itemsToUpload objectAtIndex:0] isKindOfClass:[NSURL class]]) {
                fileURL = [[itemsToUpload objectAtIndex:0] absoluteString];
            } else {
                fileURL = [itemsToUpload objectAtIndex:0];
            }
        }
        
        uploadFileName = [fileURL retain];
        
        [uploadFileNames removeAllObjects]; // used to delete files after uploading
        [uploadFileNames addObject:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:isZipped], @"zipped", fileURL, @"path", nil]];
        if ([itemsToUpload count] > 1) {
            for (id item in itemsToUpload) {
                NSString* path = nil;
                if ([item isKindOfClass:[NSURL class]])  {
                    path = [item path];
                } else {
                    path = item;
                }
                [uploadFileNames addObject:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], @"zipped", path, @"path", nil]];
            }
        }
        
//        [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Notification" withDescription:[NSString stringWithFormat:@"File '%@' added to upload queue", [self FileName]]];
    
		/* API v.1.0 upload request
		
        NSString *url = [NSString stringWithString:UPLOAD_ACTION];
        url = [[BoxNetHandler sharedHandler] addTokenAndFolderID:url];
        
        request = [[HTTPFormDataRequest requestWithURL:[NSURL URLWithString:url]] retain];
        
        DbgLog(@"Operation URL %@", url);
        
        [request setQueue:[[HTTPRequestHandler uploadHandler] queue]];
        [request setRequestMethod:@"POST"];
        [request addMulticastDelegate:self];
        [request addMulticastDelegate:[BoxNetHandler sharedHandler]];
        [request setFile:fileURL forKey:@"file"];
        [request setPostValue:[NSNumber numberWithInt:1] forKey:@"share"];
        [request setUserInfo:[NSDictionary dictionaryWithObject:UPLOAD_ACTION forKey:@"TYPE"]];
		 
		*/
		
		NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"https://api.box.com/2.0/folders/%@/items?limit=1000&fields=name,etag",
										   [[BoxNetHandler sharedHandler] folderID]]];
		
        request = [[HTTPFormDataRequest requestWithURL:url] retain];
		
		[request addRequestHeader:@"Authorization"
							value:[NSString stringWithFormat:@"Bearer %@", [[OAuth2Client sharedInstance] accessToken]]];
		
        [request setQueue:[[HTTPRequestHandler uploadHandler] queue]];
        [request setRequestMethod:@"GET"];
        [request addMulticastDelegate:self];
        [request addMulticastDelegate:[BoxNetHandler sharedHandler]];
        [request setUserInfo:[NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:@"GET_FOLDER_ITEMS", fileURL, nil]
														 forKeys:[NSArray arrayWithObjects:@"TYPE", @"URL", nil]]];
		
		// --------------------------
		
        // To mark the start
        PostNotificationWithObject(QUEUE_PROGRESS, self);
        [self triggerUpdate];
        
        [[HTTPRequestHandler uploadHandler] addRequest:request];
        
        fileURL = nil;
    }
    
    safe_release(filePath);
    fileName = nil;

    // PREPARE FOR UPLOADING
    operationStatus = QUEUED;
    [self triggerUpdate];
    
    while (![self isFinished]) 
    {
        sleep(5);
    }
}

- (void) triggerUpdate
{
    [[[[BoxSimpleShareAppDelegate sharedDelegate] mainController] requestsTable] reloadData];
}

- (void) triggerUpdate:(NSTimeInterval)interval
{
     [[[[BoxSimpleShareAppDelegate sharedDelegate] mainController] requestsTable] performSelector:@selector(reloadData) withObject:nil afterDelay:interval];
}

- (void) addFiles:(id)files
{
    assert([files isKindOfClass:[NSArray class]] || [files isKindOfClass:[NSMutableArray class]] || [files isKindOfClass:[NSString class]] );
    
    if ([files isKindOfClass:[NSArray class]] || [files isKindOfClass:[NSMutableArray class]])
    {
        [itemsToUpload addObjectsFromArray:files];
    }
    else
        if ([files isKindOfClass:[NSString class]])
        {
            [itemsToUpload addObject:files];
        }
}

- (NSString *) FileName
{
    if (isZipped || [itemsToUpload count] > 1) 
        return [NSString stringWithFormat:@"%@ (Multi-Files)", [uploadFileName lastPathComponent]];
    
    return [uploadFileName lastPathComponent];
}

- (NSString *) getCurrentStatus
{
    /*
    STARTED,
    EXECUTING,
    UPLOADING,
    ZIPPING,
    FAILED,
    CANCELED,
    COMPLETED
     */
    
    switch (operationStatus)
    {
        case CANCELED:
            return @"Canceled";
            
        case FAILED:
            return @"Failed";
            
        case STARTED:
        case EXECUTING:
            return @"Processing";
            
        case UPLOADING:
            return @"Uploading";
            
        case ZIPPING:
            return @"Compressing";
            
        case COMPLETED:
            return @"Completed";
            
        case QUEUED:
            return @"Queued";
            
        default:
            return @"Queued";
    }
    
    return nil;
}

- (void) cancel
{
    [super cancel];
    [request reset];
    operationStatus = CANCELED;
}

- (BOOL) isExecuting
{
    return (operationStatus == EXECUTING || operationStatus == UPLOADING || operationStatus == ZIPPING || operationStatus == QUEUED);
}

- (BOOL) isCancelled
{
    return (operationStatus == CANCELED);
}

- (BOOL) isFinished
{
    return (operationStatus == COMPLETED || operationStatus == CANCELED || operationStatus == FAILED);
}

- (BOOL) isConcurrent
{
    return YES;
}

#pragma mark
#pragma mark Progress Reporter

- (long long) bytesUploadedSoFar
{
    return [request totalBytesSent];
}

- (long long) totalBytesToUpload
{
    return [request postLength];
}

- (long long) bytesDownloadedSoFar
{
    return [request totalBytesRead];
}

- (long long) totalBytesToDownload
{
    return [request contentLength] + [request partialDownloadSize];
}



#pragma mark
#pragma mark Progress Delegate And Operation Status

- (void) requestStarted:(id)request
{
//    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Notification" withDescription:[NSString stringWithFormat:@"Uploading file '%@'", [self FileName]]];
    
    operationStatus = UPLOADING;
    PostNotificationWithObject(REQUEST_PROGRESS, self);
    [self triggerUpdate];
}

- (void) oauth2CreateSharedLink:(NSString *)fileID
{
	NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"https://api.box.com/2.0/files/%@", fileID]];
	
	request = [[HTTPRequest requestWithURL:url] retain];
	[request addRequestHeader:@"Authorization"
						value:[NSString stringWithFormat:@"Bearer %@", [[OAuth2Client sharedInstance] accessToken]]];
	
	NSMutableDictionary* access = [NSMutableDictionary dictionary];
	[access setObject:@"open" forKey:@"access"];
	
	NSMutableDictionary* properties = [NSMutableDictionary dictionary];
	[properties setObject:access forKey:@"shared_link"];
	
	SBJsonWriter* jsonWriter = [[SBJsonWriter alloc] init];
	NSString* plainJSON = [jsonWriter stringWithObject:properties];
	
	[request appendPostData:[plainJSON dataUsingEncoding:NSUTF8StringEncoding]];
	[request setRequestMethod:@"PUT"];
	[request addMulticastDelegate:self];
	[request setQueue:[[HTTPRequestHandler uploadHandler] queue]];
	
	[request setUserInfo:[NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:@"CREATE_SHARED_LINK", nil]
													 forKeys:[NSArray arrayWithObjects:@"TYPE", nil]]];

	
	[[HTTPRequestHandler uploadHandler] addRequest:request];
	
	[jsonWriter release];
}

- (void) requestFinished:(id)request1
{
	/*
    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Complete" withDescription:[NSString stringWithFormat:@"Finish uploading file '%@'", [self FileName]]];
    
    operationStatus = COMPLETED;
    [self triggerUpdate:5];
	*/
    
//    DbgLog(@"DATA FROM SERVER AFTER UPLOADING FILE: %@", [request1 responseString]);

	BoxFile *file = nil;
	
	SBJsonParser* parser = [[SBJsonParser alloc] init];
	NSDictionary* json = [parser objectWithString:[request1 responseString]];
	
	if ([[json objectForKey:@"type"] isEqualToString:@"error"])
	{
		[self requestFailed:request1];
		[parser release];
		return;
	}
	
	if ([[[request1 userInfo] objectForKey:@"TYPE"] isEqualToString:@"GET_FOLDER_ITEMS"])
	{
		NSString* fileID = nil;
		NSString* fileETag = nil;
		NSString* fileURL = [[request1 userInfo] objectForKey:@"URL"];
		NSString* fileName = [fileURL lastPathComponent];
		
		for (NSMutableDictionary* item in [json objectForKey:@"entries"])
		{
			if ([[item objectForKey:@"name"] isEqualToString:fileName])
			{
				if ([[item objectForKey:@"type"] isEqualToString:@"file"])
				{
					fileID = [item objectForKey:@"id"];
					fileETag = [item objectForKey:@"etag"];
				}
				else
				{
					DbgLog(@"Item but not file [%@] already exists", fileName);
					[self requestFailed:request1];
					[parser release];
					return;
				}
				
				break;
			}
		}
		
		NSURL* url = nil;
		
		if (fileID)
		{
			DbgLog(@" File [%@] already exists. Uploading a new version...", fileName);
			
			url = [NSURL URLWithString:[NSString stringWithFormat:@"https://api.box.com/2.0/files/%@/content", fileID]];
			
			request = [[HTTPFormDataRequest requestWithURL:url] retain];
			[request addRequestHeader:@"If-Match" value:fileETag];
		}
		else
		{
			DbgLog(@" No file [%@] has been uploaded yet. Uploading...", fileName);
			
			url = [NSURL URLWithString:@"https://api.box.com/2.0/files/content"];
			request = [[HTTPFormDataRequest requestWithURL:url] retain];
		}
		
		[request addRequestHeader:@"Authorization"
							value:[NSString stringWithFormat:@"Bearer %@", [[OAuth2Client sharedInstance] accessToken]]];
		
		[request setQueue:[[HTTPRequestHandler uploadHandler] queue]];
		[request setRequestMethod:@"POST"];
		[request addMulticastDelegate:self];
		[request addMulticastDelegate:[BoxNetHandler sharedHandler]];
		[request setFile:fileURL forKey:@"filename"];
		[request setPostValue:[[BoxNetHandler sharedHandler] folderID] forKey:@"parent_id"];
		
		[request setUserInfo:[NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:UPLOAD_ACTION, nil]
														 forKeys:[NSArray arrayWithObjects:@"TYPE", nil]]];
		
		[[HTTPRequestHandler uploadHandler] addRequest:request];
		
		[parser release];
		return;
	}
	else if ([[[request1 userInfo] objectForKey:@"TYPE"] isEqualToString:UPLOAD_ACTION])
	{
		DbgLog(@" Creating shared link for uploaded file ...");
		[self oauth2CreateSharedLink:[[[json objectForKey:@"entries"] objectAtIndex:0] objectForKey:@"id"]];
			
		[parser release];
		return;
	}
	else if ([[[request1 userInfo] objectForKey:@"TYPE"] isEqualToString:@"CREATE_SHARED_LINK"])
	{
		DbgLog(@" File has been uploaded");
		
		file = [BoxFile new];
		
		[file setUploadStatus:@"upload_ok"];
		
		[file setFileName:[json objectForKey:@"name"]];
		[file setFileID:[json objectForKey:@"id"]];
		[file setPublicName:[[json objectForKey:@"shared_link"] objectForKey:@"url"]];
		[file setFolderID:[[json objectForKey:@"parent"] objectForKey:@"id"]];
		[file setIsShared:YES];
		
		[BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Complete" withDescription:[NSString stringWithFormat:@"Finish uploading file '%@'", [self FileName]]];
		
		operationStatus = COMPLETED;
		[self triggerUpdate:5];
		
		[parser release];
	}
	else
	{
		DbgLog(@"ERROR: skipping unknown request processing");
		return;
	}
	
    {
        // Add URL to clipboard
        
        // Parse
        /*
         <response>
            <status>upload_ok</status>
            <files>
                <file file_name="Archive_2011-07-17_10.41.09 PM.zip" id="824227784" folder_id="84213682" shared="1" public_name="qi3e2rx6yl9k7x99jd4l" />
            </files>
         </response>
         */
        
		/* API v1.0 implementation
        GDataXMLDocument *xmlDoc = [[GDataXMLDocument alloc] initWithData:[request1 responseData] options:0 error:nil];
        
        if (xmlDoc)
        {
            GDataXMLElement *root = [xmlDoc rootElement];
            
            BoxFile *file = [BoxFile new];
            
            [file setUploadStatus:[[[root elementsForName:@"status"] objectAtIndex:0] stringValue]];
            
            if (![[file uploadStatus] isEqualToString:@"upload_ok"])
            {
                [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Failed" withDescription:[NSString stringWithFormat:@"Failed to upload file '%@'", [self FileName]]];
                
                goto exit;
            }
    
            root = [[[[root elementsForName:@"files"] objectAtIndex:0] elementsForName:@"file"] objectAtIndex:0];
            
            [file setFileName:[[root attributeForName:@"file_name"] stringValue]];
            [file setFileID:[[root attributeForName:@"id"] stringValue]];
            [file setPublicName:[[root attributeForName:@"public_name"] stringValue]];
            [file setFolderID:[[root attributeForName:@"folder_id"] stringValue]];
            [file setIsShared:[[[root attributeForName:@"shared"] stringValue] intValue]];
			*/
            
            MainController *controller = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
                        
            // Copy URL to clipboard
            if (controller.copy_url_to_clipboard_check)
            {
				NSString* url = nil;
				
                if ([file shortURL])
                {
					url = [file shortURL];
					
//                    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"URL copied to clipboard" withDescription:[NSString stringWithFormat:@"%@", [file shortURL]]];
                }
                else
                {
//					url = [NSString stringWithFormat:@"http://www.box.com/shared/%@", [file publicName]];
					
					url = [file publicName];
					
//                    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"URL copied to clipboard" withDescription:[NSString stringWithFormat:@"%@", url]];
                }

				if (controller.shorten_links_check)
				{
					url = URLShortner( url );
				}
				
				CopyURLToClipBoardForType(url, NSStringPboardType, nil);
            }
            
            
            // remove file
            
            BOOL deleteFiles = (isScreenshot && controller.delete_screenshot_after_upload_check) || (!isScreenshot && controller.delete_all_after_upload_check);
            
            if (deleteFiles || isZipped) {
                [self deleteLastUploadedFilesZippedOnly:!deleteFiles];
            }
            
            controller = nil;
            
            [[[BoxSimpleShareAppDelegate sharedDelegate] filesUploadedInSession] addObject:file];
            [[BoxSimpleShareAppDelegate sharedDelegate] updateHistoryMenu]; // update history menu
            
        exit:
            
//            root = nil;
//            safe_release(xmlDoc);
            safe_release(file);
        //}
    }
}

- (void) requestFailed:(id)request
{
    [BoxSimpleShareAppDelegate showNotificationWithTitle:@"Upload Failed" withDescription:[NSString stringWithFormat:@"Failed to upload file '%@'", [self FileName]]];
    
    operationStatus = FAILED;
    PostNotificationWithObject(REQUEST_PROGRESS, self);
    [self triggerUpdate:2];
}

- (void) request:(id)requestRef didReceiveBytes:(long long)bytes
{
    //PostNotificationWithObject(REQUEST_PROGRESS, self);
	//DbgLog(@"Request Received Bytes: %lld", bytes);
}

- (void) request:(id)requestRef didSendBytes:(long long)bytes
{
    //PostNotificationWithObject(REQUEST_PROGRESS, self);
	//DbgLog(@"Request Send Bytes: %lld", bytes);
}

- (void) request:(id)requestRef incrementDownloadSizeBy:(long long)newLength
{
	//DbgLog(@"incrementDownloadSizeBy %lld", newLength); 
}

- (void) request:(id)requestRef incrementUploadSizeBy:(long long)newLength
{
	//DbgLog(@"incrementUploadSizeBy %lld", newLength);
}

#pragma mark 
#pragma mark Memory Management

- (void) dealloc
{   
    safe_release(itemsToUpload);
    safe_release(uploadToFolder);
    safe_release(request);    
    if (isZipped) {
            [self deleteLastUploadedFilesZippedOnly:YES];
    }
    
    safe_release(uploadFileName);
    safe_release(uploadFileNames);
    
    [super dealloc];
}

#pragma mark -

- (void)deleteLastUploadedFilesZippedOnly:(BOOL)zippedOnly {
    for (NSDictionary* dict in uploadFileNames) {
        BOOL zipped = [[dict objectForKey:@"zipped"] boolValue];
        if (!zippedOnly || zipped) {
            [self moveFileToTrash:[dict objectForKey:@"path"] isZipped:zipped];
        }
    }
}

- (void)moveFileToTrash:(NSString*)filePath isZipped:(BOOL)zipped {
    NSInteger tag;
    NSString* fileToDelete = [filePath lastPathComponent];
    NSString* parentFolder = [filePath stringByDeletingLastPathComponent];
    if (zipped) { // we should delete temp directory
        fileToDelete = [parentFolder lastPathComponent];
        parentFolder = [parentFolder stringByDeletingLastPathComponent];
    }
    [[NSWorkspace sharedWorkspace] performFileOperation:NSWorkspaceRecycleOperation
                                                 source:parentFolder 
                                            destination:@""
                                                  files:[NSArray arrayWithObject:fileToDelete]
                                                    tag:&tag];    
}

@end
