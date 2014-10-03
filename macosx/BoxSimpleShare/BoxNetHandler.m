//
//  BoxNetHandler.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "BoxNetHandler.h"
#import "OAuth2Client.h"
#import "GDataXMLNode.h"
#import "JSON.h"
#import "UploadOperation.h"
#import "HTTPRequest.h"
#import "OperationQueue.h"
#import "HTTPRequestHandler.h"
#import "BoxNetUser.h"
#import "BoxFolder.h"
#import "Extended.h"
#import "MainController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "ImgurUploadOperation.h"
#import "YoutubeUploadOperation.h"
#import "Mixpanel.h"
static NSString* BoxNetHandlerDefaultFolderPreferenceKey = @"BoxNetHandlerDefaultFolderPreferenceKey";

static const NSTimeInterval ReloadUserInfoTimeInterval = 360; // every 6 mins

@interface BoxNetHandler (Private)

- (void) createDefaultFolder:(BOOL)shouldChain;
- (void) getUserAccountInfo:(BOOL)shouldChain;
- (void) loadAccountInfo;
- (void)postNotificationOnMainThread:(NSNotification*)notification;

@end


@implementation BoxNetHandler

@synthesize token, folderID, boxNetUser, operationQueue;

static BoxNetHandler *sharedObject = nil;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
        operationQueue = [OperationQueue new];
        [operationQueue setMaxConcurrentOperationCount:MAX_CONCURRENT_OPERATIONS];
        boxNetUser = [BoxNetUser new];
/*
        userInfoReloadTimer = [NSTimer scheduledTimerWithTimeInterval:ReloadUserInfoTimeInterval
                                                               target:self
                                                             selector:@selector(loadAccountInfo)
                                                             userInfo:nil
                                                              repeats:YES];
*/
        userInfoReloadTimer = [NSTimer scheduledTimerWithTimeInterval:ReloadUserInfoTimeInterval
                                                               target:self
                                                             selector:@selector(oauth2LoadAccountInformation)
                                                             userInfo:nil
                                                              repeats:YES];
	}
    
    return self;
}

+ (BoxNetHandler *) sharedHandler
{
    if (!sharedObject)
    {
        sharedObject = [BoxNetHandler new];
        @synchronized(sharedObject)
        {
        }
    }
    
    return sharedObject;
}

#pragma mark
#pragma mark Box.net Comm Methods

- (void)setUser:(BoxNetUser*)user token:(NSString *)newToken {
    if (user != boxNetUser) {
        [boxNetUser release];
        boxNetUser = [user retain];
        [token release];
        token = [newToken retain];
        NSDictionary* storedDefaultFolders = [[NSUserDefaults standardUserDefaults] objectForKey:BoxNetHandlerDefaultFolderPreferenceKey];
        NSDictionary* userFolderDict = [storedDefaultFolders objectForKey:boxNetUser.userID];
        if (userFolderDict) {
            [defaultFolder release];
            defaultFolder = [[BoxFolder alloc] initWithDictionary:userFolderDict];
            [self getUserAccountInfo:YES];//To load user avatar
        }
        [self createDefaultFolder:YES];
        [self loadAccountInfo];
    }                 
}

- (NSString*)defaultFolderPublicName {
    return defaultFolder.publicName;
}

- (void) performLogin:(NSDictionary *)params
{
    assert([params containsKey:USERNAME]);
    assert([params containsKey:PASSWORD]);
    
    NSString* url = [NSString stringWithFormat:LOGIN_ACTION,BASE_URL, API_KEY, URLEncode([params valueForKey:USERNAME]), MD5([NSString stringWithFormat:@"%@%@", [params valueForKey:USERNAME], [params valueForKey:PASSWORD]])];
    
    DbgLog(@"%@", url);
    
    HTTPRequest *request = [HTTPRequest requestWithURL:[NSURL URLWithString:url]];
    [request addMulticastDelegate:self];
    [request setUserInfo:[NSDictionary dictionaryWithObject:LOGIN_ACTION forKey:@"TYPE"]];
    
    [[HTTPRequestHandler sharedHandler] addRequest:request];
}

- (void) createDefaultFolder:(BOOL)shouldChain
{
    HTTPRequest *request = [HTTPRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:CREATE_FOLDER_ACTION, BASE_URL, API_KEY, token, @"0", URLEncode(DEFAULT_FOLDER_NAME), 1]]];
    [request addMulticastDelegate:self];
    
    NSMutableDictionary *dic = [NSMutableDictionary new];
    [dic setValue:CREATE_FOLDER_ACTION forKey:@"TYPE"];
    
    if (shouldChain) {
        [dic setValue:@"CHAINED" forKey:@"CHAINED"];
    }
    
    [request setUserInfo:dic];
    
    safe_release(dic);
    
    [[HTTPRequestHandler sharedHandler] addRequest:request];
}

// updates user avatar only
- (void) getUserAccountInfo:(BOOL)shouldChain {
    HTTPRequest *request = [HTTPRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:GET_AVATAR, BASE_URL, API_KEY, token, [boxNetUser userID]]]];
    [request addMulticastDelegate:self];
    
    NSMutableDictionary *dic = [NSMutableDictionary new];
    [dic setValue:GET_AVATAR forKey:@"TYPE"];
    
    if (shouldChain) {
        [dic setValue:@"CHAINED" forKey:@"CHAINED"];
    }
    
    [request setUserInfo:dic];
    
    safe_release(dic);
    
    [[HTTPRequestHandler sharedHandler] addRequest:request];
}

// updates account info such as space used, space left etc
- (void) loadAccountInfo {
    HTTPRequest *request = [HTTPRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:GET_ACCOUNT_INFO, BASE_URL, API_KEY, token]]];
    [request addMulticastDelegate:self];
    
    NSMutableDictionary *dic = [NSMutableDictionary dictionaryWithObject:GET_ACCOUNT_INFO forKey:@"TYPE"];
        
    [request setUserInfo:dic];
        
    [[HTTPRequestHandler sharedHandler] addRequest:request];
}

- (void) uploadFiles:(id)files withProperties:(NSDictionary *)properties {
	UploadOperation *opt = [UploadOperation new];
    [opt addFiles:files];
    [opt setUploadToFolder:[defaultFolder folderID]];
     MainController *controller = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    if (properties && [properties containsKey:@"SCREEN_SHOT"]) {
        
        
        [opt setIsScreenshot:YES];
       
        //[[Mixpanel sharedInstance] trackCaptureRegionEvent];

        // Copy URL to clipboard
        if (controller.isImgur)
        {
            opt = [ImgurUploadOperation new];
            [opt addFiles:files];
            [opt setIsScreenshot:YES];

        }
    }
    
    else if (properties && [properties containsKey:@"YOUTUBE"] && controller.isYouTubeLogin == TRUE){
        opt = [YoutubeUploadOperation new];
        [opt addFiles:files];

    }
    
    [operationQueue addOperation:opt];

    safe_release(opt);
}

#pragma mark
#pragma mark Parsers, Response Handlers And Utility Methods

- (NSString *) addTokenAndFolderID:(NSString *)url {
    return [self addTokenAndFolderID:url withFolder:[defaultFolder folderID]];
}

- (NSString *) addTokenAndFolderID:(NSString *)url withFolder:(NSString *)folder {
    assert(url); 
    
    url = [url stringByReplacingOccurrencesOfString:@"<auth token>" withString:token];
    url = [url stringByReplacingOccurrencesOfString:@"<folder id>" withString:folder];
    
    return url;
}

/*
- (void) requestFinished:(id)request {
    // Parse the XML/response in background thread so as to make the UI more responsive
    [NSThread detachNewThreadSelector:@selector(parseResponse:) toTarget:self withObject:[NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:[[request userInfo] valueForKey:@"TYPE"], [request responseData], [request userInfo], nil] forKeys:[NSArray arrayWithObjects:@"ACTION", @"DATA", @"USERINFO", nil]]];
}
*/

- (void) requestFinished:(id)request
{
	NSArray* objs = [NSArray arrayWithObjects:[[request userInfo] valueForKey:@"TYPE"], [request responseString], [request userInfo], nil];
	NSArray* keys = [NSArray arrayWithObjects:@"ACTION", @"DATA", @"USERINFO", nil];

	[NSThread detachNewThreadSelector:@selector(oauth2ParseResponse:)
							 toTarget:self
						   withObject:[NSDictionary dictionaryWithObjects:objs forKeys:keys]];
}

- (void) requestFailed:(id)request {
    NSNotification* notification = [NSNotification notificationWithName:@"LOGIN_RESPONSE" object:[request error]];
    [self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification waitUntilDone:NO];
}

- (void)postNotificationOnMainThread:(NSNotification*)notification {
    [[NSNotificationCenter defaultCenter] postNotification:notification];
}

- (void) parseResponse:(NSDictionary *)params {   
    NSAutoreleasePool *pool = [NSAutoreleasePool new];
    
    NSString *action = [params valueForKey:@"ACTION"];
    
    if ([action isEqualToString:LOGIN_ACTION]) {
        /* Example Response
         
         <response>
         <status>logged</status>
         <auth_token>zn2s4t2125ag0a9iqqsdp4ygksd3sedx</auth_token>
         <user>
         <login>sap.dcc@gmail.com</login>
         <email>sap.dcc@gmail.com</email>
         <access_id>2575395</access_id>
         <user_id>2575395</user_id>
         <space_amount>53687091200</space_amount>
         <space_used>1127310341</space_used>
         <max_upload_size>1073741824</max_upload_size>
         </user>
         </response>
         
         */
        
        NSData *data = [params valueForKey:@"DATA"];
        
        GDataXMLDocument *xmlDoc = [[GDataXMLDocument alloc] initWithData:data options:0 error:nil];
        
        if (xmlDoc) {
            GDataXMLElement *root = [xmlDoc rootElement];
            
            if ([[[[[root elementsForName:@"status"] objectAtIndex:0] stringValue] lowercaseString] isEqualToString:@"invalid_login"])
            {
                // Report Invalid Login 
                
                NSDictionary* userInfo = [NSDictionary dictionaryWithObject:[[[root elementsForName:@"status"] objectAtIndex:0] stringValue]  
                                                                     forKey:@"error"];
                NSError* error = [NSError errorWithDomain:@"net.box.loginerror" code:-1 userInfo:userInfo];
                NSNotification* notification = [NSNotification notificationWithName:@"LOGIN_RESPONSE" object:error];
                [self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification waitUntilDone:NO];
                         
                data = nil;
                root = nil;
                safe_release(xmlDoc);
                [pool drain];
                return;
            }
            
            if (![[[[[root elementsForName:@"status"] objectAtIndex:0] stringValue] lowercaseString] isEqualToString:@"logged"])
            {
                // Login Failure for any other reason other than invalid login credentials
                
                NSDictionary* userInfo = [NSDictionary dictionaryWithObject:[[[root elementsForName:@"status"] objectAtIndex:0] stringValue]  
                                                                     forKey:@"error"];
                NSError* error = [NSError errorWithDomain:@"net.box.loginerror" code:-1 userInfo:userInfo];
                NSNotification* notification = [NSNotification notificationWithName:@"LOGIN_RESPONSE" object:error];
                [self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification waitUntilDone:NO];

                data = nil;
                root = nil;
                safe_release(xmlDoc);
                [pool drain];
                return;
            }
            
            //        Print XML for dump purpose
            //        for (GDataXMLElement *element in [root children])
            //        {
            //            DbgLog(@"%@ -> %@", [element name], [element stringValue]);
            //            
            //        }
            
            token = [[NSString alloc] initWithString:[[[root elementsForName:@"auth_token"] objectAtIndex:0] stringValue]];
            
            root = [[root elementsForName:@"user"] objectAtIndex:0];
            
            [boxNetUser setLogin:[[[root elementsForName:@"login"] objectAtIndex:0] stringValue]];
            [boxNetUser setEmail:[[[root elementsForName:@"email"] objectAtIndex:0] stringValue]];
            [boxNetUser setAccessID:[[[root elementsForName:@"access_id"] objectAtIndex:0] stringValue]];
            [boxNetUser setUserID:[[[root elementsForName:@"user_id"] objectAtIndex:0] stringValue]];
            [boxNetUser setToken:token];
            
            unsigned long long spaceAmount = 0;
            unsigned long long spaceUsed = 0;
            unsigned long long maxUploadSize = 0;
            
            NSNumberFormatter *format = [NSNumberFormatter new];
            
            spaceAmount = [[format numberFromString:[[[root elementsForName:@"space_amount"] objectAtIndex:0] stringValue]] unsignedLongLongValue];
            spaceUsed = [[format numberFromString:[[[root elementsForName:@"space_used"] objectAtIndex:0] stringValue]] unsignedLongLongValue];
            maxUploadSize = [[format numberFromString:[[[root elementsForName:@"max_upload_size"] objectAtIndex:0] stringValue]] unsignedLongLongValue];
            
            boxNetUser.spaceAmount = spaceAmount;
            boxNetUser.spaceUsed = spaceUsed;
            boxNetUser.maxUploadSize = maxUploadSize;
            
            [boxNetUser saveUser];
            safe_release(format);
            root = nil;
            safe_release(xmlDoc);
            
            // Parsing complete, set out notification that login was successful
            // write code here!
			
            NSNotification* notification = [NSNotification notificationWithName:@"UPDATED_LOADING_TEXT"
                                                                         object:@"Fetching/Creating Default Upload Folder"];
            [self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification waitUntilDone:NO];
                        
            [self createDefaultFolder:YES];
        }
        
        data = nil;
    }
    else
        if ([action isEqualToString:CREATE_FOLDER_ACTION])
        {
            /* Example Response
             
             <response>
             <status>s_folder_exists</status>
             <folder>
             <folder_id>84213682</folder_id>
             <folder_name>SimpleShare Uploads</folder_name>
             <folder_type_id>0</folder_type_id>
             <user_id>2575395</user_id>
             <path/>
             <shared>1</shared>
             <public_name>954cfr1qclhcl7nq6qri</public_name>
             <show_comments/>
             <parent_folder_id>0</parent_folder_id>
             <password/>
             </folder>
             </response>
             
             */
            
            NSData *data = [params valueForKey:@"DATA"];
            
            GDataXMLDocument *xmlDoc = [[GDataXMLDocument alloc] initWithData:data options:0 error:nil];
            
            if (xmlDoc)
            {
                GDataXMLElement *root = [xmlDoc rootElement];
                
                NSString* status = [[[[root elementsForName:@"status"] objectAtIndex:0] stringValue] lowercaseString];
                
                if (![status isEqualToString:@"s_folder_exists"] && ![status isEqualToString:@"create_ok"]) {
                    // Login Failure
                    
                    data = nil;
                    root = nil;
                    safe_release(xmlDoc);
                    [pool drain];
                    return;
                }
                
                root = [[root elementsForName:@"folder"] objectAtIndex:0];
                
                safe_release(folderID);
                folderID = [[NSString stringWithString:[[[root elementsForName:@"folder_id"] objectAtIndex:0] stringValue]] retain];
                
                safe_release(defaultFolder);
                defaultFolder = [BoxFolder new];
                
                [defaultFolder setFolderID:[[[root elementsForName:@"folder_id"] objectAtIndex:0] stringValue]];
                [defaultFolder setFolderName:[[[root elementsForName:@"folder_name"] objectAtIndex:0] stringValue]];
                [defaultFolder setFolderTypeID:[[[root elementsForName:@"folder_type_id"] objectAtIndex:0] stringValue]];
                [defaultFolder setIsShared:[[[[root elementsForName:@"shared"] objectAtIndex:0] stringValue] intValue]];
                [defaultFolder setParentFolderID:[[[root elementsForName:@"parent_folder_id"] objectAtIndex:0] stringValue]];
                [defaultFolder setPublicName:[[[root elementsForName:@"public_name"] objectAtIndex:0] stringValue]];
                [defaultFolder setPassword:[[[root elementsForName:@"password"] objectAtIndex:0] stringValue]];
                
                if (boxNetUser.userID) {
                    NSDictionary* folderDict = [defaultFolder dictionaryRepresentation];
                    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
                    NSMutableDictionary* foldersDict = [[[defaults objectForKey:BoxNetHandlerDefaultFolderPreferenceKey] mutableCopy] autorelease];
                    if (!foldersDict) {
                        foldersDict = [NSMutableDictionary dictionary];
                    }
                    [foldersDict setObject:folderDict forKey:boxNetUser.userID];
                    [defaults setObject:foldersDict forKey:BoxNetHandlerDefaultFolderPreferenceKey];
                    [defaults synchronize];
                }
                
                root = nil;
                safe_release(xmlDoc);
                
                // Parsing complete, set out notification that folder was created or already exists
                // write code here!
                
                //[self uploadFiles:[NSArray arrayWithObject:@""] withProperties:nil];
                
                
                PostNotificationWithObject(@"UPDATED_LOADING_TEXT", [NSString stringWithString:@"Fetching User Account Information"]);
                
                if ([[params valueForKey:@"USERINFO"] containsKey:@"CHAINED"])
                {
                    [self getUserAccountInfo:YES];
                }
                
                //[self uploadFiles:nil withProperties:nil];
            }
            
            data = nil;
        }
        else
            if ([action isEqualToString:UPLOAD_ACTION])
            {
                /* Example Response
                 <response>
                 <status>upload_ok</status>
                 <files>
                 <file file_name="ScriptingBridge.h" id="809943004" folder_id="84213682" shared="1" public_name="9coi6451zg8cb4jis66z" />
                 </files>
                 </response>
                 */
                
                
            }
            else
                if ([action isEqualToString:GET_AVATAR])
                {
                    /* Example Response
                    <response>
                        <status>s_get_user_info</status>
                        <user_name>Syed Arsalan Pervez</user_name>
                        <user_email>sap.dcc@gmail.com</user_email>
                        <user_id>2575395</user_id>
                        <avatar_url>https://www.box.com/api/avatar/large/2575395</avatar_url>
                     </response>
                     */
                    
                    NSData *data = [params valueForKey:@"DATA"];
                    
                    GDataXMLDocument *xmlDoc = [[GDataXMLDocument alloc] initWithData:data options:0 error:nil];
                    
                    if (xmlDoc)
                    {
                        GDataXMLElement *root = [xmlDoc rootElement];
                        
                        [boxNetUser setUserAvatar:[[[root elementsForName:@"avatar_url"] objectAtIndex:0] stringValue]];
						[boxNetUser setUserName:[[[root elementsForName:@"user_name"] objectAtIndex:0] stringValue]];
                        
                        root = nil;
                        safe_release(xmlDoc);
                    }
                    
                    data = nil;
					               
                    [boxNetUser setIsAuthenticated:YES];
                    [boxNetUser saveUser];

                    NSNotification* notification = [NSNotification notificationWithName:@"LOGIN_RESPONSE" object:boxNetUser];
                    [self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification waitUntilDone:NO];
                }
                else 
                    if ([action isEqualToString:GET_ACCOUNT_INFO])
                    {
                        /* Example response
                         <?xml version='1.0' encoding='UTF-8' ?>
                         <response>
                         <status>get_account_info_ok</status>
                         <user>
                         <login>email@example.com</login>
                         <email>email@example.com</email>
                         <access_id>398387</access_id>
                         <user_id>398387</user_id>
                         <space_amount>1073741824</space_amount>
                         <space_used>0</space_used>
                         <max_upload_size>2147483648</max_upload_size>
                         </user>
                         </response>
                         */
                        NSData *data = [params valueForKey:@"DATA"];
                        
                        GDataXMLDocument *xmlDoc = [[GDataXMLDocument alloc] initWithData:data options:0 error:nil];
                        
                        if (xmlDoc)
                        {
                            GDataXMLElement *root = [xmlDoc rootElement];
                            
                            NSArray* elements = [root elementsForName:@"user"];
                            if ([elements count] > 0) {
                                root = [elements objectAtIndex:0];
                                
                                NSNumberFormatter *format = [NSNumberFormatter new];
                                
                                NSArray* elements = [root elementsForName:@"space_amount"];
                                if ([elements count] > 0) {
                                    [boxNetUser setSpaceAmount:[[format numberFromString:[[elements objectAtIndex:0] stringValue]] unsignedLongLongValue]];
                                }
                                
                                elements = [root elementsForName:@"space_used"];
                                if ([elements count] > 0) {
                                    [boxNetUser setSpaceUsed:[[format numberFromString:[[elements objectAtIndex:0] stringValue]] unsignedLongLongValue]];
                                }
                                
                                [format release];
                                
                            }
                            root = nil;
                            safe_release(xmlDoc);
                        }
                        
                        data = nil;
                        
                        [boxNetUser saveUser];
						
                        NSNotification* notification = [NSNotification notificationWithName:@"UPDATE_MAINVIEW_CONTENT" object:boxNetUser];
                        [self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification waitUntilDone:NO];
                    }
    
    action = nil;
    
    [pool drain];
}

- (void) oauth2GetUserInformation
{
	PostNotificationWithObject(@"UPDATED_LOADING_TEXT", [NSString stringWithString:@"Fetching User Account Information"]);
	
	NSURL* url = [NSURL URLWithString:@"https://api.box.com/2.0/users/me"];
	
	HTTPRequest* request = [HTTPRequest requestWithURL:url];
	[request addRequestHeader:@"Authorization"
						value:[NSString stringWithFormat:@"Bearer %@", [[OAuth2Client sharedInstance] accessToken]]];

	[request addMulticastDelegate:self];

    NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:GET_ACCOUNT_INFO forKey:@"TYPE"];
    [request setUserInfo:dic];
	
    [[HTTPRequestHandler sharedHandler] addRequest:request];
}

- (void) oauth2LoadAccountInformation
{
	NSURL* url = [NSURL URLWithString:@"https://api.box.com/2.0/users/me"];
	
	HTTPRequest* request = [HTTPRequest requestWithURL:url];
	[request addRequestHeader:@"Authorization"
						value:[NSString stringWithFormat:@"Bearer %@", [[OAuth2Client sharedInstance] accessToken]]];
	
	[request addMulticastDelegate:self];
	
    NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:@"LOAD_ACCOUNT_INFO" forKey:@"TYPE"];
    [request setUserInfo:dic];
	
    [[HTTPRequestHandler sharedHandler] addRequest:request];
}

- (void) oauth2CreateDefaultFolder
{
	NSNotification* notification = [NSNotification notificationWithName:@"UPDATED_LOADING_TEXT"
																 object:@"Fetching/Creating Default Upload Folder"];
	
	[self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification waitUntilDone:NO];
	
	NSURL* url = [NSURL URLWithString:@"https://api.box.com/2.0/folders"];

	HTTPRequest* request = [HTTPRequest requestWithURL:url];
	[request addRequestHeader:@"Authorization"
						value:[NSString stringWithFormat:@"Bearer %@", [[OAuth2Client sharedInstance] accessToken]]];

	NSMutableDictionary* parent = [NSMutableDictionary dictionary];
	[parent setObject:@"0" forKey:@"id"];
	
	NSMutableDictionary* properties = [NSMutableDictionary dictionary];
	[properties setObject:DEFAULT_FOLDER_NAME forKey:@"name"];
	[properties setObject:parent forKey:@"parent"];
	
	SBJsonWriter* jsonWriter = [[SBJsonWriter alloc] init];
	NSString* plainJSON = [jsonWriter stringWithObject:properties];
	
	[request appendPostData:[plainJSON dataUsingEncoding:NSUTF8StringEncoding]];
	[request addMulticastDelegate:self];
	
    NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:CREATE_FOLDER_ACTION forKey:@"TYPE"];
    [request setUserInfo:dic];
	
    [[HTTPRequestHandler sharedHandler] addRequest:request];
	
	[jsonWriter release];
}

- (void) oauth2GetFolderInformation:(NSString *)folderId
{
	NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"https://api.box.com/2.0/folders/%@", folderId]];
	
	HTTPRequest* request = [HTTPRequest requestWithURL:url];
	[request addRequestHeader:@"Authorization"
						value:[NSString stringWithFormat:@"Bearer %@", [[OAuth2Client sharedInstance] accessToken]]];

	[request addMulticastDelegate:self];
	
    NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:CREATE_FOLDER_ACTION forKey:@"TYPE"];
    [request setUserInfo:dic];
	
    [[HTTPRequestHandler sharedHandler] addRequest:request];
}

- (void) oauth2CreateSharedLink:(NSString *)folderId
{
	NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"https://api.box.com/2.0/folders/%@", folderId]];
	
	HTTPRequest* request = [HTTPRequest requestWithURL:url];
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
	
    NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:CREATE_FOLDER_ACTION forKey:@"TYPE"];
    [request setUserInfo:dic];
	
    [[HTTPRequestHandler sharedHandler] addRequest:request];
	
	[jsonWriter release];
}

- (void) oauth2ParseResponse:(NSDictionary *)params
{
    NSAutoreleasePool* pool = [NSAutoreleasePool new];
    
    NSString* action = [params valueForKey:@"ACTION"];

    
	if ([action isEqualToString:GET_ACCOUNT_INFO] || [action isEqualToString:@"LOAD_ACCOUNT_INFO"])
	{
		SBJsonParser* parser = [[SBJsonParser alloc] init];
		NSDictionary* json = [parser objectWithString:[params objectForKey:@"DATA"]];
		
		[boxNetUser setLogin:[json objectForKey:@"login"]];
		[boxNetUser setEmail:[json objectForKey:@"login"]];
		[boxNetUser setUserID:[json objectForKey:@"id"]];
		[boxNetUser setSpaceAmount:[[json objectForKey:@"space_amount"] unsignedLongLongValue]];
		[boxNetUser setSpaceUsed:[[json objectForKey:@"space_used"] unsignedLongLongValue]];
		 
		// Q: Where is maxUploadSize???
		
		[boxNetUser saveUser];
		
		NSNotification* notification1 = [NSNotification notificationWithName:@"UPDATE_MAINVIEW_CONTENT" object:boxNetUser];
		[self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification1 waitUntilDone:NO];
		
		[boxNetUser setUserAvatar:[json objectForKey:@"avatar_url"]];
		[boxNetUser setUserName:[json objectForKey:@"name"]];
		
		[boxNetUser setIsAuthenticated:YES];
		[boxNetUser saveUser];

		[parser release];
	
		if ([action isEqualToString:GET_ACCOUNT_INFO])
		{
			[self oauth2CreateDefaultFolder];
		}
	}
	else if ([action isEqualToString:CREATE_FOLDER_ACTION])
	{
		SBJsonParser* parser = [[SBJsonParser alloc] init];
		NSDictionary* json = [parser objectWithString:[params objectForKey:@"DATA"]];

		if ([[json objectForKey:@"type"] isEqualToString:@"error"])
		{
			if ([[json objectForKey:@"code"] isEqualToString:@"item_name_in_use"])
			{
				DbgLog(@"Default folder already exists, retrieving information about it...");
				
				NSArray* conflicts = [[json objectForKey:@"context_info"] objectForKey:@"conflicts"];
				[self oauth2GetFolderInformation:[[conflicts objectAtIndex:0] objectForKey:@"id"]];
			}
			else
			{
				DbgLog(@"ERROR: folder operation failed: @%", [json objectForKey:@"message"]);
			}
		}
		else
		{
			safe_release(folderID);
			folderID = [[json objectForKey:@"id"] retain];
			
			safe_release(defaultFolder);
			defaultFolder = [BoxFolder new];
			
			// Q: What is the difference between FolderName and PublicName?
			// Q: What is the purpose of password?
			
			[defaultFolder setFolderID:[json objectForKey:@"id"]];
			[defaultFolder setFolderName:[json objectForKey:@"name"]];
			[defaultFolder setFolderTypeID:[json objectForKey:@"type"]];
			[defaultFolder setParentFolderID:[[json objectForKey:@"parent"] objectForKey:@"id"]];

			if ([[json objectForKey:@"shared_link"] isKindOfClass:[NSNull class]])
			{
				DbgLog(@"Default folder doesn't have shared links, creating shared link ...");
                
				[self oauth2CreateSharedLink:[defaultFolder folderID]];
			}
			else
			{
				[defaultFolder setIsShared:[[[json objectForKey:@"shared_link"] objectForKey:@"access"] isEqualToString:@"open"]];
				[defaultFolder setPublicName:[[json objectForKey:@"shared_link"] objectForKey:@"url"]];

				if (boxNetUser.userID)
				{
					NSDictionary* folderDict = [defaultFolder dictionaryRepresentation];
					NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
					NSMutableDictionary* foldersDict = [[[defaults objectForKey:BoxNetHandlerDefaultFolderPreferenceKey] mutableCopy] autorelease];
					if (!foldersDict)
					{
						foldersDict = [NSMutableDictionary dictionary];
					}
					
					[foldersDict setObject:folderDict forKey:boxNetUser.userID];
					[defaults setObject:foldersDict forKey:BoxNetHandlerDefaultFolderPreferenceKey];
					[defaults synchronize];
				}
				
				NSNotification* notification2 = [NSNotification notificationWithName:@"LOGIN_RESPONSE" object:boxNetUser];
				[self performSelectorOnMainThread:@selector(postNotificationOnMainThread:) withObject:notification2 waitUntilDone:NO];
			}
		}
		
		[parser release];
	}
	
	[pool drain];
}

#pragma mark
#pragma mark Memory Management

- (void) deallocSharedHandler
{
    safe_release_sharedObject(sharedObject);
}

- (void) dealloc
{
    safe_release(token);
    safe_release(folderID);
    safe_release(boxNetUser);
    safe_release(defaultFolder);
    
    [userInfoReloadTimer invalidate];
    
    if ([operationQueue operationCount] > 0)
        [operationQueue cancelAllOperations];
    safe_release(operationQueue);
    
    [super dealloc];
}

@end
