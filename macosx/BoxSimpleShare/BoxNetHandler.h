//
//  BoxNetHandler.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//


#import "ASIHTTPRequestDelegate.h"
#import "ASIProgressDelegate.h"

@class OperationQueue, BoxNetUser, BoxFolder;

@interface BoxNetHandler : NSObject <ASIHTTPRequestDelegate, ASIProgressDelegate> {
@private
    
    NSString *token;
    NSString *folderID;
    
    BoxNetUser *boxNetUser;
    BoxFolder *defaultFolder;
    
    OperationQueue *operationQueue;
    
    NSTimer* userInfoReloadTimer;
}

@property (readonly, nonatomic) NSString *token;
@property (readonly, nonatomic) NSString *folderID;

@property (readonly, nonatomic) OperationQueue *operationQueue;
@property (readonly, nonatomic) BoxNetUser *boxNetUser;

+ (BoxNetHandler *) sharedHandler;
- (void) performLogin:(NSDictionary *)params;

- (NSString *) addTokenAndFolderID:(NSString *)url;
- (NSString *) addTokenAndFolderID:(NSString *)url withFolder:(NSString *)folder;

- (NSString*)defaultFolderPublicName;

- (void) uploadFiles:(id)files withProperties:(NSDictionary *)properties;

// Appeared as a result of changing login method from direct login to using browser
- (void)setUser:(BoxNetUser*)user token:(NSString *)newToken;

- (void) oauth2GetUserInformation;

- (void) deallocSharedHandler;

@end
