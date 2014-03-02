//
//  AppConstants.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "AppConstants.h"

NSString * APP_VERSION = @"v2.3";

// OLD
//NSString * const API_KEY = @"tl21gh5qncigu2e8nneo2fvro5m0h5aa";

// NEW
NSString * const API_KEY = @"eo0cww5szjn2wywnkcje927zdk0opxxp";

// SSHARE
//NSString * const API_KEY = @"bh2on74gxmcu2stykoufo507i9sr6zp3";

NSArray * SUPPORTED_FORMATS = nil;

// Actions

NSString * const LOGIN_ACTION = @"%@authorization&api_key=%@&login=%@&password=%@&method=md5";

NSString * const GET_AVATAR = @"%@get_user_info&api_key=%@&auth_token=%@&user_id=%@"; // BASE_URL, API_KEY, AUTH_TOKEN, USER_ID

NSString * const GET_ACCOUNT_INFO = @"%@get_account_info&api_key=%@&auth_token=%@"; // BASE_URL, API_KEY, AUTH_TOKEN

NSString * const CREATE_FOLDER_ACTION = @"%@create_folder&api_key=%@&auth_token=%@&parent_id=%@&name=%@&share=%i";

NSString * const UPLOAD_ACTION = @"https://upload.box.com/api/1.0/upload/<auth token>/<folder id>"; // Token, Folder id

NSString * const OVERWRITE_ACTION = @"https://upload.box.com/api/1.0/overwrite/<auth token>/<file id>"; // Token, Folder id

NSString * const NEY_COPY_ACTION = @"https://upload.box.com/api/1.0/new_copy/<auth token>/<file id>"; // Token, Folder id

// Mixpanel

//NSString * const MIXPANEL_TOKEN = @"de25386f737fe56de4baee18c1a26eec";
NSString * const MIXPANEL_TOKEN = @"59b8cd4ab8944d85a13d3788280075ad";

NSString * const MIXPANEL_ENGAGE_URL = @"http://api.mixpanel.com/engage/?data=";

NSString * const MIXPANEL_TRACK_URL = @"http://api.mixpanel.com/track/?data=";

// OAuth2

NSString * const OAUTH2_AUTH_CODE_URL = @"https://api.box.com/oauth2/authorize?response_type=code&client_id=%@&redirect_uri=%@";

NSString * const OAUTH2_AUTH_TOKEN_URL = @"https://api.box.com/oauth2/token";

// SSHARE
//NSString * const OAUTH2_CLIENT_ID = @"bh2on74gxmcu2stykoufo507i9sr6zp3";
NSString * const OAUTH2_CLIENT_ID = @"eo0cww5szjn2wywnkcje927zdk0opxxp";

// SSHARE
//NSString * const OAUTH2_CLIENT_SECRET = @"QtCWsYay78SIf7p61FPkEODb5FemdQWH";
NSString * const OAUTH2_CLIENT_SECRET = @"YrtmNDYRVSPuchqX6yv52c4AkIhNydh9";

