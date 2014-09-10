//
//  GDriverAuthenticateManager.h
//  Agency Machine
//
//  Created by Cuongnq on 3/28/14.
//  Copyright (c) 2014 Chipmunk. All rights reserved.
//

#import <Foundation/Foundation.h>

@class GTMOAuth2Authentication;
@interface YoutubeAuthenticateManager : NSObject
{
    
}

@property (strong) GTMOAuth2Authentication* authenticate;
+(YoutubeAuthenticateManager*) shareManager;
-(void)updateAuthenticateToken :(NSDictionary*) dict;

@end
