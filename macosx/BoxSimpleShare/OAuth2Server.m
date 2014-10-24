//
//  OAuth2Server.m
//  BoxSimpleShare
//
//  Created by Sergey Rak on 01/27/13.
//

#import "OAuth2Server.h"
#import "OAuth2Connection.h"
#import "OAuth2Client.h"

#include <netinet/in.h>

@implementation OAuth2Server

- (id) initWithDelegate:(id)delegateObject
{
	if ((self = [super init]))
	{
        
        // 0 for automatic port assignment
		socket = [[NSSocketPort alloc] initWithTCPPort:0];
		handle = [[NSFileHandle alloc] initWithFileDescriptor:[socket socket] closeOnDealloc:YES];
		
		delegate = [delegateObject retain];
	
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(onNewConnection:)
													 name:NSFileHandleConnectionAcceptedNotification
												   object:nil];
		
		[handle acceptConnectionInBackgroundAndNotify];
		
		connections = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void) dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	//[delegate release];
	
	[connections release];
	
	[socket release];
	[handle release];
	
	[super dealloc];
}

- (NSInteger) listenPort
{
	struct sockaddr_in* addr = (struct sockaddr_in *)[[socket address] bytes];
	return htons(addr->sin_port);
}

- (void) onNewConnection:(NSNotification *)notification
{
	NSDictionary* userInfo = [notification userInfo];
	NSFileHandle* remoteHandle = [userInfo objectForKey:NSFileHandleNotificationFileHandleItem];
	
	NSNumber* errorNo = [userInfo objectForKey:@"NSFileHandleError"];
	if (errorNo)
	{
		DbgLog(@"Accept error: %@", errorNo);
		return;
	}
	
    //continue to listen for connections
    
	[handle acceptConnectionInBackgroundAndNotify];
	
	if (remoteHandle)
	{
		OAuth2Connection* connection = [[OAuth2Connection alloc] initWithHandle:remoteHandle
																	andDelegate:self];
		
		if (connection)
		{
			[connections addObject:connection];
			[connection release];
		}
	}
}

- (void) onRequestURL:(NSURL *)url
{
	NSString* errCode = nil;
	NSString* errDesc = nil;
	
	NSArray* queryParams = [[url query] componentsSeparatedByString:@"&"];
	for (NSString* param in queryParams)
	{
		NSArray* paramParts = [param componentsSeparatedByString:@"="];
		if ([paramParts count] > 0)
		{
			NSString* key = [[paramParts objectAtIndex:0] lowercaseString];
			
			if ([key isEqualToString:@"code"])
			{
				[delegate onAuthCodeReceived:[paramParts objectAtIndex:1]];
				return;
			}
			else if ([key isEqualToString:@"error"])
			{
				errCode = [paramParts objectAtIndex:1];
			}
			else if ([key isEqualToString:@"error_description"])
			{
				errDesc = [paramParts objectAtIndex:1];
			}
		}
	}
	
	if (errCode || errDesc)
	{
		[delegate onAuthCodeFailure:errCode andDescription:errDesc];
	}
}

- (void) onConnectionClosed:(OAuth2Connection *)connection
{
	[connections removeObject:connection];
}

@end

