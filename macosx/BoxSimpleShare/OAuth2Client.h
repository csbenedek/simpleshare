//
//  OAuth2Client.h
//  BoxSimpleShare
//
//  Created by Sergey Rak on 01/27/13.
//

#import "ASIProgressDelegate.h"

@class OAuth2Server;
@class ASINetworkQueue;

@interface OAuth2Client: NSObject<ASIProgressDelegate>
{
@private
	
	OAuth2Server* server;
	ASINetworkQueue* queue;

	NSString* accessToken;
	NSString* refreshToken;
	NSString* tokenType;
	NSTimeInterval tokenExpires;

	NSTimer* refreshTimer;
	
	BOOL isAuthorized;
}

+ (OAuth2Client *) sharedInstance;
- (void) deallocSharedInstance;

- (void) authorize;

- (void) downloadFile:(NSString *)fileID;

- (void) onAuthCodeReceived:(NSString *)code;
- (void) onAuthCodeFailure:(NSString *)code
			andDescription:(NSString *)description;

@property(readonly) NSString* accessToken;

@end
