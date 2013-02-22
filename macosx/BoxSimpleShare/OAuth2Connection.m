//
//  OAuth2Client.m
//  BoxSimpleShare
//
//  Created by Sergey Rak on 01/28/13.
//

#import "OAuth2Connection.h"
#import "OAuth2Server.h"

@implementation OAuth2Connection

- (id) initWithHandle:(NSFileHandle *)remoteHandle
		  andDelegate:(id)delegateObject
{
	if ((self = [super init]))
	{
		handle = [remoteHandle retain];
		delegate = [delegateObject retain];
		
		msg = NULL;
		
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(onDataAvailable:)
													 name:NSFileHandleReadCompletionNotification
												   object:nil];
	
		[handle readInBackgroundAndNotify];
	}
	
	return self;
}

- (void) dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[handle release];
	[delegate release];
	
	if (NULL != msg)
	{
		CFRelease( msg );
		msg = NULL;
	}
	
	[super dealloc];
}

- (void) onDataAvailable:(NSNotification *)notification
{
	NSData* data = [[notification userInfo] objectForKey:NSFileHandleNotificationDataItem];

	if ([data length] == 0)
	{
		[delegate onConnectionClosed:self];
	}
	else
	{
		[handle readInBackgroundAndNotify];
		
		if (NULL == msg)
		{
			msg = CFHTTPMessageCreateEmpty(kCFAllocatorDefault, TRUE);
		}
		
		BOOL success = CFHTTPMessageAppendBytes(msg, [data bytes], [data length]);
		if (success)
		{
			if (CFHTTPMessageIsHeaderComplete(msg))
			{
				SSLog(@"HTTP REQUEST: [%s]", (const char*)[data bytes]);
				
				CFURLRef url = CFHTTPMessageCopyRequestURL( msg );
				
				[delegate onRequestURL:(NSURL *)url];
				
				CFRelease( url );
				CFRelease( msg );
				msg = NULL;
			}
		}
		else
		{
			[delegate onConnectionClosed:self];
		}
	}
}

@end
