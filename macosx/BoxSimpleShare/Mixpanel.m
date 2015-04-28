//
//  Mixpanel.m
//  BoxSimpleShare
//
//  Created by Sergey Rak on 01/18/13.
//

#import "Mixpanel.h"
#import "AppConstants.h"
#import "BoxNetUser.h"
#import "ASIHTTPRequest.h"
#import "ASINetworkQueue.h"
#import "NSData+Base64.h"
#import "JSON.h"
#import "MainController.h"
#import "BoxSimpleShareAppDelegate.h"


@implementation Mixpanel

static Mixpanel* mixpanelSharedInstance = nil;

- (id) init
{
	if ((self = [super init]))
	{
		queue = [ASINetworkQueue new];
		[queue go];
		
		[queue setDelegate:self];
		[queue setRequestDidFinishSelector:@selector(requestDidFinish:)];
		[queue setRequestDidFailSelector:@selector(requestDidFail:)];
		
		userEngaged = NO;
	}
	
	return self;
}

- (void) dealloc
{
	[distinctID release];
	[userName   release];
	[userEmail  release];
	
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
	queue = nil;
	
	[super dealloc];
}

+ (Mixpanel *) sharedInstance
{
	@synchronized(self)
	{
		if (mixpanelSharedInstance == nil)
		{
			mixpanelSharedInstance = [[Mixpanel alloc] init];
		}
		
		return mixpanelSharedInstance;
	}
}

- (void) deallocSharedInstance
{
	[self dealloc];
	mixpanelSharedInstance = nil;
}

- (void) identify:(BoxNetUser *)boxUser
{
	if (!userEngaged)
	{		
		distinctID = [[boxUser userID] retain];
		userName   = [[boxUser userName] retain];
		userEmail  = [[boxUser email] retain];

		[self engage];
	}
}

- (void) requestDidFinish:(ASIHTTPRequest *)request
{
	if ([[request responseString] isEqualToString:@"1"])
	{
		if ([[request userInfo] objectForKey:@"engage"])
		{
			userEngaged = YES;
		}
	}
	else
	{
		DbgLog(@"ERROR: Mixpanel request wasn't logged: %@", [[request url] absoluteString]);
	}
}

- (void) requestDidFail:(ASIHTTPRequest *)request
{
	DbgLog(@"ERROR: Mixpanel request failed: %@", [request error]);
}


-(void) trackDisableUploads{
    
    [self track:@"OnDisableUploads"];
    
    
}

-(void) trackFileDragEvent{
    
    
    [self track:@"OnFileDragged"];
    
}

- (void) trackCaptureRegionEvent
{
	[self track:@"OnCaptureRegion"];
}

- (void) trackCaptureFullScreenEvent
{
	[self track:@"OnCaptureFullScreen"];
}

- (void) trackVideoCaptureEvent
{
	[self track:@"OnVideoCapture"];
}

- (void) trackUploadFileEvent
{
	[self track:@"OnUploadFile"];	
}

- (void) track:(NSString *)eventName
{
	if (!distinctID || !userName || !userEmail)
	{
		DbgLog(@" ********* MIXPANEL TRACK FAILED !!!!!!!!!! *************");
		return;
	}

	if (!userEngaged)
	{
		[self engage];
	}
	
	NSNumber* time = [NSNumber numberWithInt:(int)[[NSDate date] timeIntervalSince1970]];
	
	NSMutableDictionary* properties = [NSMutableDictionary dictionary];
	[properties setObject:MIXPANEL_TOKEN forKey:@"token"];
	[properties setObject:distinctID forKey:@"distinct_id"];
	[properties setObject:time forKey:@"time"];
	
	NSMutableDictionary* event = [NSMutableDictionary dictionary];
	[event setObject:eventName forKey:@"event"];
	[event setObject:properties forKey:@"properties"];
	
	SBJsonWriter* jsonWriter = [[SBJsonWriter alloc] init];
	NSString* plainJSON = [jsonWriter stringWithObject:event];
	NSData* data = [plainJSON dataUsingEncoding:NSUTF8StringEncoding];
	NSString* base64JSON = [data base64EncodedString];
	
	NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"%@%@", MIXPANEL_TRACK_URL, base64JSON]];
	ASIHTTPRequest* request = [ASIHTTPRequest requestWithURL:url];
	[queue addOperation:request];
	
	[jsonWriter release];
}


- (void) engage
{
	if (!distinctID || !userName || !userEmail)
	{
		return;
	}
    
    MainController *controller = (MainController *)[[BoxSimpleShareAppDelegate   sharedDelegate] mainController];
	
	NSMutableDictionary* action = [NSMutableDictionary dictionary];
	[action setObject:userName forKey:@"$first_name"];
	[action setObject:userEmail forKey:@"$email"];
    //set user preferences
    [action setObject:[NSNumber numberWithBool:controller.delete_screenshot_after_upload_check] forKey:@"deleteScreenShotAfterUpload"];
    
    [action setObject:[NSNumber numberWithBool:controller.delete_all_after_upload_check] forKey:@"deleteAllAfterUpload"];
    [action setObject:[NSNumber numberWithBool:controller.launch_at_startup_check] forKey:@"launchAtLogin"];
    [action setObject:[NSNumber numberWithBool:controller.shorten_links_check] forKey:@"shortenLinks"];
    [action setObject:[NSNumber numberWithBool:controller.copy_url_to_clipboard_check] forKey:@"copyURLToClipboard"];
    [action setObject:[NSNumber numberWithBool:controller.mute_audio_check] forKey:@"muteAudio"];
    
    if (controller.isImgur) {
        
        [action setObject:@"Imgur" forKey:@"Image Host"];
    }
    
    else{
        
        [action setObject:@"Box" forKey:@"Image Host"];
    }
    
    if (controller.isYouTubeLogin) {
        
        [action setObject:@"YouTube" forKey:@"Video Host"];
    }
    else{
        
        [action setObject:@"Box" forKey:@"Video Host"];
        
    }
    

	NSMutableDictionary* properties = [NSMutableDictionary dictionary];
	[properties setObject:action forKey:@"$set"];
	[properties setObject:MIXPANEL_TOKEN forKey:@"$token"];
	[properties setObject:distinctID forKey:@"$distinct_id"];

	SBJsonWriter* jsonWriter = [[SBJsonWriter alloc] init];
	NSString* plainJSON = [jsonWriter stringWithObject:properties];
	NSData* data = [plainJSON dataUsingEncoding:NSUTF8StringEncoding];
	NSString* base64JSON = [data base64EncodedString];
	
	NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"%@%@", MIXPANEL_ENGAGE_URL, base64JSON]];
	ASIHTTPRequest* request = [ASIHTTPRequest requestWithURL:url];

	[request setUserInfo: [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:@"engage"]];
	
	[queue addOperation:request];
}

@end