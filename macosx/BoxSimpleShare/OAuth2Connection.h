//
//  OAuth2Connection.h
//  BoxSimpleShare
//
//  Created by Sergey Rak on 01/28/13.
//

@interface OAuth2Connection: NSObject
{
	NSFileHandle* handle;
	id delegate;
	
	CFHTTPMessageRef msg;
}

- (id) initWithHandle:(NSFileHandle *)handle
		  andDelegate:(id)delegate;

@end
