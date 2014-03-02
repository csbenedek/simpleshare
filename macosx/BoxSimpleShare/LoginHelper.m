//
//  LoginHelper.h
//  BoxSimpleShare
//
//  Created by Alexander Kievsky on 11/10/11.
//

#import "LoginHelper.h"
#import "BoxUser.h"
//#import "BSUtils.h"
//#import "BSBoxServiceProvider.h"
//#import "BSMainController.h"
//#import "BSConstants.h"

@interface LoginHelper (Private)

- (void)startRequestAuthToken;

@end

@implementation LoginHelper

- (id)init {
    if (self = [super init]) {
        
        _ticketOperation = [[BoxGetTicketOperation alloc] initWithDelegate:self];
		_authTokenOperation = [[BoxGetAuthTokenOperation alloc] initWithTicket:nil delegate:self];
    }
    return self;
}

- (void)startLoginProcessWithParentWindow:(NSWindow*)window {
    _parentLoginWindow = window;
    NSBeginAlertSheet(InterfaceString(@"StartLoginTitle"), 
                      InterfaceString(@"OK"), 
                      InterfaceString(@"Cancel"), 
                      nil, 
                      _parentLoginWindow, 
                      self, 
                      @selector(sheetDidEndShouldDelete:returnCode:contextInfo:), 
                      NULL, 
                      NULL, 
                      InterfaceString(@"StartLoginMessage")); 
}

- (void)sheetDidEndShouldDelete:(NSWindow*)sheet
                     returnCode:(NSInteger)returnCode
                    contextInfo:(void*)contextInfo {
    if (returnCode == NSAlertDefaultReturn) {
        [self performSelectorInBackground:@selector(requestTicketFromService) withObject:nil];
    }
}

- (void)requestTicketFromService {
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    [self startLoginProcess];
    [self performSelectorOnMainThread:@selector(showLoginConfirmation) withObject:nil waitUntilDone:NO];
    [pool release];
}


- (void)showLoginConfirmation {
    NSBeginAlertSheet(InterfaceString(@"WasLoginSuccessTitle"), 
                      InterfaceString(@"YES"), 
                      InterfaceString(@"NO"), 
                      nil, 
                      _parentLoginWindow, 
                      self, 
                      @selector(loginConfirmationSheetDidEndShouldDelete:returnCode:contextInfo:), 
                      NULL, 
                      NULL, 
                      InterfaceString(@"WasLoginSuccessMessage"));
}

- (void)loginConfirmationSheetDidEndShouldDelete:(NSWindow*)sheet
                                      returnCode:(NSInteger)returnCode
                                     contextInfo:(void*)contextInfo {
    if (returnCode == NSAlertDefaultReturn) {
        [self performSelectorInBackground:@selector(requestAuthToken) withObject:nil];
    }
    
}

- (void)requestAuthToken {
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    [self startRequestAuthToken];
    [pool release];
}


- (void)startLoginProcess {
    // if we don't have the ticket, request it
	// otherwise, go straight to the next step
	if (!_ticketOperation.ticket || _ticketOperation.ticket == @"") {
		[_ticketOperation start];
	} else {
		[self operation:_ticketOperation didCompleteForPath:nil response:BoxOperationResponseSuccessful];
	}
}

- (void)setDelegate:(id<LoginHelperDelegate>)delegate {
    _delegate = delegate;
}

- (void)dealloc {
    [_ticketOperation setDelegate:nil];
    [_ticketOperation release];
    [_authTokenOperation setDelegate:nil];
    [_authTokenOperation release];
    [super dealloc];
}


#pragma mark -

- (void)fireError {
    [_delegate loginFailedWithError:nil];
}

- (void)fireCompleted {
    BoxUser* user = _authTokenOperation.user;
    [_delegate loginCompletedWithUser:user];
}

- (void)openBrowserForAuth {
    [[NSWorkspace sharedWorkspace] openURL:[_ticketOperation authenticationURL]];
}

- (void)operation:(BoxOperation*)op didCompleteForPath:(NSString*)path response:(BoxOperationResponse)response {
	if (response != BoxOperationResponseSuccessful) {
        [self performSelectorOnMainThread:@selector(fireError) withObject:nil waitUntilDone:NO];
		return;
	}
	
	// what we do depends on which operation completed
	if (op == _ticketOperation) {
        [self performSelectorOnMainThread:@selector(openBrowserForAuth) withObject:nil waitUntilDone:NO];
	} else if (op == _authTokenOperation) {
		// login complete!
        [self performSelectorOnMainThread:@selector(fireCompleted) withObject:nil waitUntilDone:NO];
	}
}

- (void)startRequestAuthToken {
    _authTokenOperation.ticket = _ticketOperation.ticket;
    [_authTokenOperation start];
}


@end
