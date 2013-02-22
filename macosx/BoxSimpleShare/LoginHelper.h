//
//  LoginHelper.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/10/11.
//

#import <WebKit/WebKit.h>

#import "BoxUser.h"
#import "BoxGetTicketOperation.h"
#import "BoxGetAuthTokenOperation.h"

//#import "BSBaseLoginHelper.h"

@protocol LoginHelperDelegate <NSObject>

@required

- (void)loginCompletedWithUser:(BoxUser*)user;
- (void)loginFailedWithError:(NSError*)error;

@end


@interface LoginHelper : NSObject <BoxOperationDelegate> {
    
	BoxGetTicketOperation*      _ticketOperation;
	BoxGetAuthTokenOperation*   _authTokenOperation;
    NSWindow*                   _parentLoginWindow; // weak
    
    id<LoginHelperDelegate>     _delegate; // weak
    
}

- (void)setDelegate:(id<LoginHelperDelegate>)delegate;

- (void)startLoginProcess;
- (void)requestAuthToken;
- (void)startLoginProcessWithParentWindow:(NSWindow*)window;

@end
