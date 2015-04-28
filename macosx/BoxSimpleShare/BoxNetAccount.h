//
//  BoxNetAccount.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 23/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BoxNetAccount : NSObject <NSURLConnectionDataDelegate, NSURLConnectionDelegate>


@property (nonatomic, retain) NSString * login;
@property (nonatomic, retain) NSString *avatarURL;
@property (nonatomic, retain) NSImage *avatar;

@property (nonatomic, retain) NSMutableData * imageData;





@property(nonatomic, retain) NSURLConnection *getInfoConnection;
@property(nonatomic, retain) NSURLConnection *getAvatarConnection;




-(void)populateInfo;

-(NSMutableURLRequest *)makeRequest;
-(NSMutableURLRequest *)makeAvatarRequest;
-(void)loadAvatar;





@end
