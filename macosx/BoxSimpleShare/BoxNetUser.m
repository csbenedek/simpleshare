//
//  BoxNetUser.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "BoxNetUser.h"

#import "Extended.h"
#import "Utilities.h"

static NSString* SavedUserPreferencesKey = @"SavedUserPreferencesKey";

@implementation BoxNetUser

@synthesize login;
@synthesize email; 
@synthesize accessID;
@synthesize userID;
@synthesize userName;
@synthesize userAvatar;
@synthesize spaceAmount;
@synthesize spaceUsed;
@synthesize maxUploadSize;
@synthesize isAuthenticated;
@synthesize password;
@synthesize token;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
        isAuthenticated = NO;
    }
    
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    // write code
    return nil;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    
}

- (void)dealloc
{
    safe_release(login);
    safe_release(email);
    safe_release(accessID);
    safe_release(userID);
	safe_release(userName);
    safe_release(userAvatar);
    safe_release(password);
    safe_release(token);
    
    spaceAmount = 0;
    spaceUsed = 0;
    maxUploadSize  = 0;
    
    [super dealloc];
}

#pragma mark -

+ (NSString*)userKeychainItemNameForName:(NSString*)name {
    return [NSString stringWithFormat:@"BoxSimpleShare : %@", name];
}

- (void)saveUser {
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    NSMutableDictionary* userInfo = [NSMutableDictionary dictionary];
    
    [userInfo safeSetObject:login forKey:@"login"];
    [userInfo safeSetObject:email forKey:@"email"];
    [userInfo safeSetObject:accessID forKey:@"accessID"];
    [userInfo safeSetObject:userID forKey:@"userID"];
    [userInfo safeSetObject:userName forKey:@"userName"];
    [userInfo safeSetObject:userAvatar forKey:@"userAvatar"];
//    [userInfo safeSetObject:token forKey:@"token"];
        
    [userInfo safeSetObject:[NSNumber numberWithLongLong:spaceAmount] forKey:@"spaceAmount"];
    [userInfo safeSetObject:[NSNumber numberWithLongLong:spaceUsed] forKey:@"spaceUsed"];
    [userInfo safeSetObject:[NSNumber numberWithLongLong:maxUploadSize] forKey:@"maxUploadSize"];
    
    [defaults setObject:userInfo forKey:SavedUserPreferencesKey];
    [defaults synchronize];
    
    [Utilities saveAuthToken:token forKeychainItemName:[BoxNetUser userKeychainItemNameForName:email]];
}

+ (BoxNetUser*)savedUser {
    NSDictionary* dictionary = [[NSUserDefaults standardUserDefaults] objectForKey:SavedUserPreferencesKey];
    if (dictionary) {
        BoxNetUser* user = [[BoxNetUser alloc] init];
        
        user.login = [dictionary objectForKey:@"login"];
        user.email = [dictionary objectForKey:@"email"];
        
        NSString* token = [Utilities authTokenForKeychainItemName:[self userKeychainItemNameForName:user.email]];
        
        if (!token) {
            [user release];
            return nil;
        }
        
        user.token = token;
        
        user.accessID = [dictionary objectForKey:@"accessID"];
        user.userID = [dictionary objectForKey:@"userID"];
        user.userName = [dictionary objectForKey:@"userName"];
        user.userAvatar = [dictionary objectForKey:@"userAvatar"];
        
        user.spaceAmount = [[dictionary objectForKey:@"spaceAmount"] unsignedLongLongValue];
        user.spaceUsed = [[dictionary objectForKey:@"spaceUsed"] unsignedLongLongValue];
        user.maxUploadSize = [[dictionary objectForKey:@"maxUploadSize"] unsignedLongLongValue];
        user.isAuthenticated = YES;
        
        return [user autorelease];
    } else {
        return nil;
    }       
}

+ (void)removeSavedUser {
    BoxNetUser* user = [self savedUser];
    if (user) {
        [Utilities removeAuthTokenForKeychainItemName:[self userKeychainItemNameForName:user.email]];
        [[NSUserDefaults standardUserDefaults] setObject:nil forKey:SavedUserPreferencesKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
}

@end
