//
//  BoxNetUser.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/6/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface BoxNetUser : NSObject <NSCoding> {
@private
    
    NSString *login;
    NSString *email;
    NSString *userID;
	NSString *userName;
    NSString *accessID;
    NSString *userAvatar;
    NSString *password;
    NSString *token;
    
    unsigned long long spaceAmount;
    unsigned long long spaceUsed;
    
    unsigned long long maxUploadSize;
    
    BOOL isAuthenticated;
}

@property (retain, nonatomic) NSString *login;
@property (retain, nonatomic) NSString *email;
@property (retain, nonatomic) NSString *userID;
@property (retain, nonatomic) NSString *userName;
@property (retain, nonatomic) NSString *accessID;
@property (retain, nonatomic) NSString *userAvatar;
@property (retain, nonatomic) NSString *password;
@property (retain, nonatomic) NSString *token;

@property (nonatomic) unsigned long long spaceAmount;
@property (nonatomic) unsigned long long spaceUsed;
@property (nonatomic) unsigned long long maxUploadSize;

@property (nonatomic) BOOL isAuthenticated;

- (void)saveUser;
+ (BoxNetUser*)savedUser;
+ (void)removeSavedUser;

@end
