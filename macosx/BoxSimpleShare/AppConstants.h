//
//  AppConstants.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//


#define safe_release(a) if (a) { [a release]; a = nil; }
#define safe_release_sharedObject(a) if (a) { [a dealloc]; a = nil; }

#define AddNotificationObserver(target, method, notification_name, notification_object) [[NSNotificationCenter defaultCenter] addObserver:target selector:method name:notification_name object:notification_object];

#define PostNotification(a) [[NSNotificationCenter defaultCenter] postNotificationName:a object:nil];

#define PostNotificationWithObject(a, b) [[NSNotificationCenter defaultCenter] postNotificationName:a object:b];

#define RemoveNotificationObserver(a) [[NSNotificationCenter defaultCenter] removeObserver:a];


#define FileExists(path) [[NSFileManager defaultManager] fileExistsAtPath:path]

// DEFAULT PATH

#define BASE_PATH [NSHomeDirectory() stringByAppendingString:@"/Library/Application Support/net.box.simpleshare"]
#define TMP_PATH [NSHomeDirectory() stringByAppendingString:@"/Library/Application Support/net.box.simpleshare/tmp"]


#define MAX_CONCURRENT_OPERATIONS 7
#define SHOW_LOGS 1

#define PROGRESS_MENU_ITEM_TEXT @"Show Progress Window"

// GENERAL KEYS

#define USERNAME @"USERNAME"
#define PASSWORD @"PASSWORD"

#define DEFAULT_FOLDER_NAME @"SimpleShare Uploads"

typedef enum
{
    STARTED,
    EXECUTING,
    UPLOADING,
    ZIPPING,
    FAILED,
    CANCELED,
    COMPLETED,
    QUEUED
    
}OperationStatus;

#define BASE_URL @"https://www.box.com/api/1.0/rest?action="

extern NSString * APP_VERSION;

extern NSString * const API_KEY;

extern NSArray * SUPPORTED_FORMATS;

/*
 BOX.NET ACTIONS --- START
 */

extern NSString * const LOGIN_ACTION;

extern NSString * const GET_AVATAR;

extern NSString * const GET_ACCOUNT_INFO;

extern NSString * const CREATE_FOLDER_ACTION;

extern NSString * const UPLOAD_ACTION;

extern NSString * const OVERWRITE_ACTION;

extern NSString * const NEW_COPY_ACTION;

/*
 BOX.NET ACTIONS --- END
 */

#define UPLOAD_FINISHED @"UPLOAD_FINISHED"
#define UPLOAD_FAILED @"UPLOAD_FAILED"

/*
 NOTIFICATION STRINGS --- START
 */

/* PROGRESS */

#define UPDATE_STATUS_ITEM @"UPDATE_STATUS_ITEM"

/*
 NOTIFICATION STRINGS --- END
 */

/*
 MIXPANEL --- START
 */

extern NSString * const MIXPANEL_TOKEN;

extern NSString * const MIXPANEL_ENGAGE_URL;

extern NSString * const MIXPANEL_TRACK_URL;

/*
 MIXPANEL --- END
 */

/*
 OAUTH2 --- START
 */

extern NSString * const OAUTH2_AUTH_CODE_URL;

extern NSString * const OAUTH2_AUTH_TOKEN_URL;

extern NSString * const OAUTH2_CLIENT_ID;

extern NSString * const OAUTH2_CLIENT_SECRET;

/*
 OAUTH2 --- END
 */


