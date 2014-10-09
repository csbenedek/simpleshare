//
//  PreferenceManager.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/17/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "PreferenceManager.h"
#import "MainController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "BoxNetHandler.h"
#import "BoxNetUser.h"
#import "AppConstants.h"

/* Prederence Keys for Marshal & UnMarshal */

#define DELETE_SCREENSHOT_AFTER_UPLOAD @"DELETE_SCREENSHOT_AFTER_UPLOAD"
#define DELETE_ALL_AFTER_UPLOAD @"DELETE_ALL_AFTER_UPLOAD"
#define DISABLE_AUTOMATIC_UPLOAD @"DISABLE_AUTOMATIC_UPLOAD"

#define COPY_URL_TO_CLIPBOARD @"COPY_URL_TO_CLIPBOARD"

#define LAUNCH_APP_ON_SYSTEM_START @"LAUNCH_APP_ON_SYSTEM_START"

#define MUTE_AUDIO @"MUTE_AUDIO"

#define SHORTEN_URL @"SHORTEN_URL"

#define DIRECT_URL @"DIRECT_URL"

#define SCREENCAST_FORMAT @"SCREENCAST_FORMAT"

#define HOT_KEY_SCREENCAST @"HOT_KEY_SCREENCAST"

#define HOT_KEY_UPLOAD @"HOT_KEY_UPLOAD"

#define CREDENTIAL @"CREDENTIAL"
#define IMAGE_UPLOAD_HOST @"IMAGE_UPLOAD_HOST"
#define VIDEO_UPLOAD_HOST @"VIDEO_UPLOAD_HOST"


@implementation PreferenceManager

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

+ (void) savePreference
{
    //save Imgur Preference
    
    MainController *mainController = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    [defaults setBool:mainController.isImgur forKey:@"isImgur"];
    
    [defaults synchronize];
    
    
    if (![[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated]) return;
    
	NSFileManager* fileManager = [NSFileManager defaultManager];
	if (![fileManager fileExistsAtPath:BASE_PATH])
	{
		if (![fileManager createDirectoryAtPath:BASE_PATH withIntermediateDirectories:YES attributes:nil error:NULL])
		{
			DbgLog(@"ERROR: Failed to create a directory: %@", BASE_PATH);
			return;
		}
	}
	
    NSString *path = [BASE_PATH stringByAppendingPathComponent:[NSString stringWithFormat:@"net.box.simpleshare.%@.pref", [[[BoxNetHandler sharedHandler] boxNetUser] userID]]];
    
    NSMutableDictionary *pref = [NSMutableDictionary new];
    MainController *controller = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    [pref setObject:[NSNumber numberWithInt:controller.delete_screenshot_after_upload_check] forKey:DELETE_SCREENSHOT_AFTER_UPLOAD];
    [pref setObject:[NSNumber numberWithInt:controller.delete_all_after_upload_check] forKey:DELETE_ALL_AFTER_UPLOAD];
    [pref setObject:[NSNumber numberWithInt:controller.disable_automatic_upload_check] forKey:DISABLE_AUTOMATIC_UPLOAD];
    
    [pref setObject:[NSNumber numberWithInt:controller.copy_url_to_clipboard_check] forKey:COPY_URL_TO_CLIPBOARD];

    [pref setObject:[NSNumber numberWithInt:controller.shorten_links_check] forKey:SHORTEN_URL];
    
    [pref setObject:[NSNumber numberWithInt:controller.mute_audio_check] forKey:MUTE_AUDIO];
        
    [pref setObject:[NSNumber numberWithInt:controller.screencast_format_index] forKey:SCREENCAST_FORMAT];
    
    [pref setObject:controller.screen_cast_hot_key forKey:HOT_KEY_SCREENCAST];
    
    [pref setObject:controller.upload_hot_key forKey:HOT_KEY_UPLOAD];
    [pref setObject:[NSNumber numberWithInt:controller.uploadhost_index] forKey:IMAGE_UPLOAD_HOST];
    
    [pref setObject:[NSNumber numberWithInt:controller.upload_video_host_index] forKey:VIDEO_UPLOAD_HOST];
    
//    [pref setObject:[[[BoxNetHandler sharedHandler] boxNetUser] getEncryptedCredentials]  forKey:CREDENTIAL];
    
    
    NSMutableData *data = [NSMutableData new];

    NSMutableArray* historyItems = [NSMutableArray array];
    
    int i = 0;
    for (id historyItem in [[[BoxSimpleShareAppDelegate sharedDelegate] filesUploadedInSession] reverseObjectEnumerator]) {
        [historyItems insertObject:historyItem atIndex:0];
        i++;
        if (i == MAX_HISTORY_ITEM_COUNT) {
            break;
        }
    }
    
    NSKeyedArchiver *archiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:data];
    [archiver encodeObject:pref forKey:@"net.box.simpleshare.pref"];
    [archiver encodeObject:historyItems forKey:@"files_uploaded_in_session"];
    [archiver finishEncoding];
    
    safe_release(pref);
    safe_release(archiver);
    
    NSError *error = nil;
    [data writeToFile:path options:NSDataWritingAtomic error:&error];
    
    path = nil;
    
    if (error)
        DbgLog(@"METHOD: %@   ERROR: %@", NSStringFromSelector(_cmd), [error description]);
    
    safe_release(data);
}

// For compatabilty with older versions
+ (void) loadPreferenceFromOlderFormat
{
    
}

+ (void) loadPreference
{
    if (![[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated]) return;
    
    NSString *path = [BASE_PATH stringByAppendingPathComponent:[NSString stringWithFormat:@"net.box.simpleshare.%@.pref", [[[BoxNetHandler sharedHandler] boxNetUser] userID]]];
    
    NSMutableData *data = [[NSMutableData alloc] initWithContentsOfFile:path];
    
    if (data && [[NSFileManager defaultManager] fileExistsAtPath:path])
    {
        NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];
        NSMutableDictionary *pref = [[unarchiver decodeObjectForKey:@"net.box.simpleshare.pref"] retain];
        
        if (pref)
        {
            MainController *controller = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
            
            controller.delete_screenshot_after_upload_check = [[pref valueForKey:DELETE_SCREENSHOT_AFTER_UPLOAD] intValue];
            controller.delete_all_after_upload_check = [[pref valueForKey:DELETE_ALL_AFTER_UPLOAD] intValue];
            controller.disable_automatic_upload_check = [[pref valueForKey:DISABLE_AUTOMATIC_UPLOAD] intValue];  
            controller.copy_url_to_clipboard_check = [[pref valueForKey:COPY_URL_TO_CLIPBOARD] intValue];
            controller.shorten_links_check = [[pref valueForKey:SHORTEN_URL] intValue];
            controller.mute_audio_check = [[pref valueForKey:MUTE_AUDIO] intValue];
            
            
            controller.upload_hot_key = @"u";
            controller.screen_cast_hot_key = @"5";
            
            controller.screencast_format_index = [[pref valueForKey:SCREENCAST_FORMAT] intValue];
            [controller.screencast_format selectItemAtIndex:controller.screencast_format_index];
            
            //not used anymore
            /*
            if([pref valueForKey:IMAGE_UPLOAD_HOST])
            {
                controller.uploadhost_index = [[pref valueForKey:IMAGE_UPLOAD_HOST] intValue];
                [controller.imageHost selectItemAtIndex:controller.uploadhost_index];
            }
            else
            {
                controller.uploadhost_index = 0;
                [pref setValue:controller.uploadhost_index forKey:IMAGE_UPLOAD_HOST];
                [controller.imageHost selectItemAtIndex:controller.uploadhost_index];
            }
            
            if([pref valueForKey:VIDEO_UPLOAD_HOST])
            {
                controller.upload_video_host_index = [[pref valueForKey:VIDEO_UPLOAD_HOST] intValue];
                [controller.videoHost selectItemAtIndex:controller.uploadhost_index];
            }
            else
            {
                controller.upload_video_host_index = 0;
                [pref setValue:controller.upload_video_host_index forKey:VIDEO_UPLOAD_HOST];
                [controller.videoHost selectItemAtIndex:controller.upload_video_host_index];
            }
            */
            
            controller.screen_cast_hot_key = [NSString stringWithFormat:[pref valueForKey:HOT_KEY_SCREENCAST]];;
            controller.upload_hot_key = [NSString stringWithFormat:[pref valueForKey:HOT_KEY_UPLOAD]];
            
            
            //restore uploaded files
            id obj = [[[unarchiver decodeObjectForKey:@"files_uploaded_in_session"] retain] autorelease];
            
            if (obj) {
                [BoxSimpleShareAppDelegate sharedDelegate].filesUploadedInSession = [NSMutableArray arrayWithArray:obj];
                
            } else {
                [BoxSimpleShareAppDelegate sharedDelegate].filesUploadedInSession = [NSMutableArray array];
            }
            
            controller = nil;
        }
        
        [unarchiver finishDecoding];
        
        
        safe_release(pref);
        safe_release(data);
        safe_release(unarchiver);
    }
    else
    {
        MainController *controller = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
        
        // DEFAULT SETTINGS
        
        controller.delete_screenshot_after_upload_check = 0;
        controller.delete_all_after_upload_check = 0;
        controller.disable_automatic_upload_check = 0;  
        controller.copy_url_to_clipboard_check = 1;
        controller.shorten_links_check = 0;
        controller.mute_audio_check = 0;
        
        controller.screencast_format_index = 0;
        
        [controller.screencast_format selectItemAtIndex:controller.screencast_format_index];
        
        controller.screen_cast_hot_key = @"5";
        controller.upload_hot_key = @"U";
        
        controller = nil;
        
        if (![[BoxSimpleShareAppDelegate sharedDelegate] filesUploadedInSession]) {
            [BoxSimpleShareAppDelegate sharedDelegate].filesUploadedInSession = [NSMutableArray array];
        }
        
        // Save the default preference
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            [PreferenceManager savePreference];
        });
    }
    
    safe_release(data);
    path = nil;
    MainController *controller = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    [controller updateSwitchControl];
}

- (void)dealloc
{
    [super dealloc];
}

@end
