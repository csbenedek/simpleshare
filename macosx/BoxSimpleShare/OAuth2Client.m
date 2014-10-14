//
//  OAuth2Client.m
//  BoxSimpleShare
//
//  Created by Sergey Rak on 01/27/13.
//

#import "OAuth2Client.h"
#import "OAuth2Server.h"
#import "BoxNetHandler.h"
#import "ASIFormDataRequest.h"
#import "ASINetworkQueue.h"
#import "SBJsonParser.h"
#import "GrantAccessWindowController.h"
@implementation OAuth2Client

static OAuth2Client* oauth2SharedInstance = nil;
GrantAccessWindowController* accessWindowController;
@synthesize accessToken;

- (id) init
{
	if ((self = [super init]))
	{
		queue = [ASINetworkQueue new];
		[queue go];
		
		[queue setDelegate:self];
		[queue setRequestDidFinishSelector:@selector(requestDidFinish:)];
		[queue setRequestDidFailSelector:@selector(requestDidFail:)];
		[queue setRequestWillRedirectSelector:@selector(request:willRedirectToURL:)];
	
		server = [[OAuth2Server alloc] initWithDelegate:self];
		
		tokenExpires = 0;
		isAuthorized = NO;
		
		refreshTimer = nil;
		
		[self loadRefreshToken];
	}
	
	return self;
}

- (void) dealloc
{
	if (refreshTimer)
	{
		[refreshTimer invalidate];
	}
	
	[server release];
	
	if ([queue operationCount] > 0)
	{
		for (id request in [queue operations])
		{
			[request reset];
		}
		
		[queue cancelAllOperations];
	}
	
	[queue reset];
	[queue release];

	[accessToken release];
	[refreshToken release];
	[tokenType release];
	
	[super dealloc];
}

+ (OAuth2Client *) sharedInstance
{
	@synchronized(self)
	{
		if (oauth2SharedInstance == nil)
		{
			oauth2SharedInstance = [[OAuth2Client alloc] init];
		}
		
		return oauth2SharedInstance;
	}
}

- (void) deallocSharedInstance
{
	[self dealloc];
	oauth2SharedInstance = nil;
}

- (void) authorize
{
	if (!refreshToken)
	{
		NSString* redirectURL = [NSURL URLWithString: [NSString stringWithFormat:@"http://localhost:%d", (int)[server listenPort]]];
		//NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:OAUTH2_AUTH_CODE_URL, OAUTH2_CLIENT_ID, redirectURL]];
        
		//[[NSWorkspace sharedWorkspace] openURL:url];
        
        if(accessWindowController == nil) {
            
            accessWindowController = [[GrantAccessWindowController alloc] initWithWindowNibName:@"GrantAccessWindowController"];
            [accessWindowController loadWindow];
        }
        
        NSString* url = [NSString stringWithFormat:OAUTH2_AUTH_CODE_URL, OAUTH2_CLIENT_ID, redirectURL];
        [accessWindowController loadUrl:url];
        [accessWindowController showWindow:nil];
        
	}
	else
	{
		[self refreshAccessToken];
	}
}

- (BOOL) isAuthorized
{
	return refreshToken != nil;
}

- (void) reset
{
	if (refreshToken)
	{
		[refreshToken release];
		refreshToken = nil;
	}
	
	[self saveRefreshToken];
}

- (void) downloadFile:(NSString *)fileID
{
	NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"https://api.box.com/2.0/files/%@/content", fileID]];

	ASIHTTPRequest* request = [ASIHTTPRequest requestWithURL:url];
	
	[request setRequestMethod:@"GET"];
	[request addRequestHeader:@"Authorization" value:[NSString stringWithFormat:@"Bearer %@", accessToken]];
	
    NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:@"DOWNLOAD_URL" forKey:@"TYPE"];
    [request setUserInfo:dic];
	
	[queue addOperation:request];
}

- (void) onAuthCodeReceived:(NSString *)code
{
	DbgLog(@"oauth2 code is [%@]", code);

	NSURL* url = [NSURL URLWithString:OAUTH2_AUTH_TOKEN_URL];
	
	ASIFormDataRequest* request = [ASIFormDataRequest requestWithURL:url];
	[request addPostValue:@"authorization_code" forKey:@"grant_type"];
	[request addPostValue:code forKey:@"code"];
	[request addPostValue:OAUTH2_CLIENT_ID forKey:@"client_id"];
	[request addPostValue:OAUTH2_CLIENT_SECRET forKey:@"client_secret"];
    
    //this creates query for login with received credentials

    NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:LOGIN_ACTION forKey:@"TYPE"];
    [request setUserInfo:dic];
    
	[queue addOperation:request];
}

- (void) onAuthCodeFailure:(NSString *)code
			andDescription:(NSString *)description;
{
	//DbgLog(@"ERROR: Failed to receive oauth2 code: code=%@, description=%@", code, description);
	// TODO: handle error and show the corresponding description.
	// the possible values are: invalid_request, unsupportedaccess_denied, server_error
	
	NSDictionary* userInfo = [NSDictionary dictionaryWithObject:description
														 forKey:@"error"];
	
	NSError* err = [NSError errorWithDomain:@"net.box.loginerror" code:-1 userInfo:userInfo];
	NSNotification* notification = [NSNotification notificationWithName:@"LOGIN_RESPONSE" object:err];
	[[NSNotificationCenter defaultCenter] postNotification:notification];
}

- (void) requestDidFinish:(ASIHTTPRequest *)request
{
	NSString* reqType = [[request userInfo] valueForKey:@"TYPE"];
	
	if ([reqType isEqualToString:LOGIN_ACTION])
	{
		DbgLog(@"OAuth2 token response: %@", [request responseString]);
		
		SBJsonParser* parser = [[SBJsonParser alloc] init];
		NSDictionary* json = [parser objectWithString:[request responseString]];
		
		NSString* key = nil;
		NSEnumerator* enumerator = [json keyEnumerator];
		
		NSMutableDictionary* dict = [NSMutableDictionary dictionaryWithCapacity:0];
		
		while ((key = [enumerator nextObject]))
		{
			if ([[key lowercaseString] isEqualToString:@"error"])
			{
				NSString* error = [json objectForKey:key];
				
				DbgLog(@"ERROR: Failed to receive oauth2 access token: error=%@", error);
				// TODO: handle error and show the corresponding description.
				// the possible values are: invalid_request, unauthorized_client,
				// invalid_grant, invalid_client, redirect_uri_mismatch, internal_server_error

				if (accessToken && refreshToken)
				{
					NSDictionary* userInfo = [NSDictionary dictionaryWithObject:[json objectForKey:@"error_description"]
																		 forKey:@"error"];
					
					NSError* err = [NSError errorWithDomain:@"net.box.loginerror" code:-1 userInfo:userInfo];
					NSNotification* notification = [NSNotification notificationWithName:@"LOGIN_RESPONSE" object:err];
					[[NSNotificationCenter defaultCenter] postNotification:notification];
				}
				else
				{
					PostNotification(@"HIDE_LOADING");
				}
				
				if (refreshToken)
				{
					[refreshToken release];
					refreshToken = nil;
				}
				
				[self saveRefreshToken];
				
				return;
			}
			else if ([[key lowercaseString] isEqualToString:@"access_token"])
			{
				[dict setObject:[json objectForKey:key] forKey:@"access_token"];
			}
			else if ([[key lowercaseString] isEqualToString:@"refresh_token"])
			{
				[dict setObject:[json objectForKey:key] forKey:@"refresh_token"];
			}
			else if ([[key lowercaseString] isEqualToString:@"token_type"])
			{
				[dict setObject:[json objectForKey:key] forKey:@"token_type"];
			}
			else if ([[key lowercaseString] isEqualToString:@"expires_in"])
			{
				[dict setObject:[json objectForKey:key] forKey:@"expires_in"];
			}
		}
		
		if ([dict count] == 4)
		{
			[accessToken release];
			accessToken = [[dict objectForKey:@"access_token"] retain];

			[refreshToken release];
			refreshToken = [[dict objectForKey:@"refresh_token"] retain];

			[tokenType release];
			tokenType = [[dict objectForKey:@"token_type"] retain];
			
			tokenExpires = [[dict objectForKey:@"expires_in"] doubleValue];
			
			DbgLog(@"OAuth2 access token has been received successfully");

			[self saveRefreshToken];
			
			[self startRefreshTimer:(tokenExpires / 2)];
			
			if (!isAuthorized)
			{
				isAuthorized = YES;
//				[self getUserInformation];
				
				[[BoxNetHandler sharedHandler] oauth2GetUserInformation];
			}
            
            if (accessWindowController.window.isVisible) {
                [accessWindowController.window close];
            }
		}
		else
		{
			DbgLog(@"ERROR: Received malformed oauth2 response for access token");
			[self startRefreshTimer:10.0];
		}
		
		[parser release];
	}
	else if ([reqType isEqualToString:GET_ACCOUNT_INFO])
	{
		DbgLog(@"GET_ACCOUNT_INFO: %@", [request responseString]);
	}
}

- (void) requestDidFail:(ASIHTTPRequest *)request
{
	DbgLog(@"ERROR: OAuth2 request failed: %@", [request error]);
	
	if (accessToken && refreshToken && tokenExpires > 0)
	{
		[self startRefreshTimer:10.0];
	}
}

- (void)request:(ASIHTTPRequest *)request willRedirectToURL:(NSURL *)newURL
{
	if ([[[request userInfo] valueForKey:@"TYPE"] isEqualToString:@"DOWNLOAD_URL"])
	{
		[[NSWorkspace sharedWorkspace] openURL:newURL];
	}
}

- (void) getUserInformation
{
	NSURL* url = [NSURL URLWithString:@"https://api.box.com/2.0/users/me"];
	
	ASIHTTPRequest* request = [ASIHTTPRequest requestWithURL:url];
	[request addRequestHeader:@"Authorization" value:[NSString stringWithFormat:@"Bearer %@", accessToken]];
	
    NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:GET_ACCOUNT_INFO forKey:@"TYPE"];
    [request setUserInfo:dic];
	
	[queue addOperation:request];
}

- (void) startRefreshTimer:(NSTimeInterval)interval
{
	if (refreshTimer)
	{
		[refreshTimer invalidate];
		refreshTimer = nil;
	}
	
	refreshTimer = [NSTimer scheduledTimerWithTimeInterval:interval
													target:self
												  selector:@selector(onRefreshTimerTick:)
												  userInfo:nil
												   repeats:NO];
}

- (void) onRefreshTimerTick:(NSTimer *)timer
{
	refreshTimer = nil;
	
	if (accessToken && refreshToken)
	{
		[self refreshAccessToken];
	}
}

- (void) refreshAccessToken
{
	DbgLog(@"Refreshing OAuth2 access token...");

	NSURL* url = [NSURL URLWithString:OAUTH2_AUTH_TOKEN_URL];
	
	ASIFormDataRequest* request = [ASIFormDataRequest requestWithURL:url];
	
	[request addRequestHeader:@"Authorization" value:[NSString stringWithFormat:@"Bearer %@", accessToken]];
	
	[request addPostValue:@"refresh_token" forKey:@"grant_type"];
	[request addPostValue:refreshToken forKey:@"refresh_token"];
	[request addPostValue:OAUTH2_CLIENT_ID forKey:@"client_id"];
	[request addPostValue:OAUTH2_CLIENT_SECRET forKey:@"client_secret"];
	
	NSMutableDictionary* dic = [NSMutableDictionary dictionaryWithObject:LOGIN_ACTION forKey:@"TYPE"];
	[request setUserInfo:dic];
	
	[queue addOperation:request];
}

- (void) loadRefreshToken
{
	NSString *path = [BASE_PATH stringByAppendingPathComponent:[NSString stringWithFormat:@"net.box.simpleshare.rt"]];
    
    NSMutableData *data = [[NSMutableData alloc] initWithContentsOfFile:path];
    
    if (data && [[NSFileManager defaultManager] fileExistsAtPath:path])
    {
        NSKeyedUnarchiver *unarchiver = [[NSKeyedUnarchiver alloc] initForReadingWithData:data];
        NSMutableDictionary *details = [[unarchiver decodeObjectForKey:@"net.box.simpleshare.rt"] retain];
        
        if (details)
        {
			refreshToken = [details valueForKey:@"RT"];
        }
        
        [unarchiver finishDecoding];
		
        safe_release( details );
    }
    
    safe_release( data );
    path = nil;
}

- (void) saveRefreshToken
{
	NSFileManager* fileManager = [NSFileManager defaultManager];
	if (![fileManager fileExistsAtPath:BASE_PATH])
	{
		if (![fileManager createDirectoryAtPath:BASE_PATH withIntermediateDirectories:YES attributes:nil error:NULL])
		{
			DbgLog(@"ERROR: Failed to create a directory: %@", BASE_PATH);
			return;
		}
	}
	
    NSString *path = [BASE_PATH stringByAppendingPathComponent:[NSString stringWithFormat:@"net.box.simpleshare.rt"]];
    
    NSMutableDictionary* details = [NSMutableDictionary new];
	
	if (refreshToken)
	{
		[details setObject:refreshToken forKey:@"RT"];
	}
    
    NSMutableData *data = [NSMutableData new];
	
    NSKeyedArchiver *archiver = [[NSKeyedArchiver alloc] initForWritingWithMutableData:data];
    [archiver encodeObject:details forKey:@"net.box.simpleshare.rt"];
    [archiver finishEncoding];
    
    safe_release(details);
    safe_release(archiver);
    
    NSError *error = nil;
    [data writeToFile:path options:NSDataWritingAtomic error:&error];
    
    safe_release( data );
    path = nil;
}

@end
