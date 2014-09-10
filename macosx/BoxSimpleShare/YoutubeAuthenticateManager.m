//
//  GDriverAuthenticateManager.m
//  Agency Machine
//
//  Created by Cuongnq on 3/28/14.
//  Copyright (c) 2014 Chipmunk. All rights reserved.
//

#import "YoutubeAuthenticateManager.h"
#import <GTL/GTMOAuth2WindowController.h>


@implementation YoutubeAuthenticateManager


+(YoutubeAuthenticateManager*) shareManager
{
    static YoutubeAuthenticateManager* _instance = nil;
    if (_instance == nil) {
        _instance = [[YoutubeAuthenticateManager alloc] init];
    }
    
    return _instance;
}

-(id)init
{
    if (self = [super init])
    {
        
        GTMOAuth2Authentication *auth;
        
        auth = [[GTMOAuth2Authentication alloc] init];
        auth.clientID = @"807165422668-bhms83kuijpsarl5jips11tkpf3jpq3b.apps.googleusercontent.com";
        auth.clientSecret = @"hzF4inZYbVKl5pfWBfX_6UwK";
        auth.accessToken =@"ya29.1.AADtN_UbZBbJhaajK-OaVcT9FAlE9wlg9LsjD2iV9moTZ7CJ2roWzCtcAiXKWrE9zs7iTg";
        auth.refreshToken = @"1/KyBmjsdRJBIj0rqSpLdgX0gisxr8aUNCB_S6pBG6N8A";
        auth.expiresIn = [NSNumber numberWithInteger:3600];
        auth.tokenType = @"Bearer";
        auth.expirationDate = [NSDate dateWithTimeIntervalSinceNow:-100];
        auth.tokenURL = [NSURL URLWithString:@"https://accounts.google.com/o/oauth2/token"];
        
        self.authenticate = auth;
        return self;
    }
    
    return nil;
    
    
}

-(void)updateAuthenticateToken :(NSDictionary*) dict
{
    self.authenticate.accessToken = [dict objectForKey:@"access_token"];
    self.authenticate.refreshToken = [dict objectForKey:@"refresh_token"];
    if ([dict objectForKey:@"expireDate"]) {
        self.authenticate.expirationDate = [dict objectForKey:@"expireDate"];
    }
}


@end
