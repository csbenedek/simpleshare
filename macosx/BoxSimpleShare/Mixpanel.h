//
//  Mixpanel.h
//  BoxSimpleShare
//
//  Created by Sergey Rak on 01/18/13.
//

#import "ASIProgressDelegate.h"

@class ASINetworkQueue;
@class BoxNetUser;

@interface Mixpanel: NSObject<ASIProgressDelegate>
{
	NSString* distinctID;
	NSString* userName;
	NSString* userEmail;
	
	ASINetworkQueue* queue;
	
	BOOL userEngaged;
}

+ (Mixpanel *) sharedInstance;
- (void) deallocSharedInstance;

- (void) identify:(BoxNetUser *)user;

- (void) trackCaptureRegionEvent;
- (void) trackCaptureFullScreenEvent;
- (void) trackVideoCaptureEvent;
- (void) trackUploadFileEvent;
-(void) trackFileDragEvent;
-(void) trackDisableUploads;



@end
