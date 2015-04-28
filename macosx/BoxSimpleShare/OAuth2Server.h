//
//  OAuth2Server.h
//  BoxSimpleShare
//
//  Created by Sergey Rak on 01/27/13.
//

@class OAuth2Connection;

@interface OAuth2Server: NSObject
{
	NSSocketPort* socket;
	NSFileHandle* handle;
	
	NSMutableArray* connections;
	
	id delegate;
}

- (id) initWithDelegate:(id)delegate;

- (NSInteger) listenPort;

- (void) onRequestURL:(NSURL *)url;
- (void) onConnectionClosed:(OAuth2Connection *)connection;

@end

