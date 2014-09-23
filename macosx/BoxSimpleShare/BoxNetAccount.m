//
//  BoxNetAccount.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 23/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "BoxNetAccount.h"
#import "BoxSimpleShareAppDelegate.h"
#import "OAuth2Client.h"
#import "JSON.h"

@implementation BoxNetAccount


-(void)populateInfo{
    
    NSMutableURLRequest *request = [self makeRequest];
    
    //start connection
    
    self.getInfoConnection = [NSURLConnection connectionWithRequest:request delegate:self];
    
    
}

-(void)loadAvatar{
    
    
    NSMutableURLRequest *request = [self makeAvatarRequest];
    
    
    
    //start connection
    self.getInfoConnection = [NSURLConnection connectionWithRequest:request delegate:self];
    
    
    
}


-(NSMutableURLRequest *)makeAvatarRequest{
    
    //create URL to get current user info
    NSURL *URL = [NSURL URLWithString:self.avatarURL];
    
    
    //create request
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:URL];
    
    [request setHTTPMethod:@"GET"];
    
    //compose authorization string
    
    NSString *authString = [NSString stringWithFormat:@"Bearer %@",[[OAuth2Client sharedInstance] accessToken]];
    
    
    [request addValue:authString forHTTPHeaderField:@"Authorization"];
    
    
    
    return request;
    
    
}


-(NSMutableURLRequest *)makeRequest{
        
    //create URL to get current user info
    NSURL *URL = [NSURL URLWithString:@"https://api.box.com/2.0/users/me"];
    
    
    //create request
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:URL];
    
    [request setHTTPMethod:@"GET"];
    
    //compose authorization string
    
    NSString *authString = [NSString stringWithFormat:@"Bearer %@",[[OAuth2Client sharedInstance] accessToken]];
    
    
    [request addValue:authString forHTTPHeaderField:@"Authorization"];
    
    
    
    return request;
    
                                    
    
}
#pragma mark - NSURLConnectionDelegateMethods
-(void)connectionDidFinishLoading:(NSURLConnection *)connection{
    if (connection == self.getInfoConnection) {
        
        [self loadAvatar];
        
        
    }
    
    
    
}


-(void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error{
    
    NSLog(@"Connection error:%@",error);
    
    
    
}

-(void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response{
    
    if (connection == self.getAvatarConnection) {
        
        NSHTTPURLResponse *response1 =(NSHTTPURLResponse *)response;
        
        NSLog(@"Response %long",[response1 statusCode]);
        
    }
    
    
    
}



-(void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data{
    
    
    if (connection == self.getInfoConnection) {
        //decode data
        NSString *response = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        
        SBJsonParser *parser = [[SBJsonParser alloc] init];
        
        NSMutableDictionary *dict = [parser objectWithData:data];
        
        self.login = [dict valueForKey:@"login"];
        self.avatarURL = [dict valueForKey:@"avatar_url"];
        
        //[self loadAvatar];
        
        [parser release];
        
        // NSLog(@"Data received:%@",response);
    }
    
    if (connection == self.getAvatarConnection) {
        
        //decode data
        
        self.avatar = [[NSImage alloc] initWithData:data];
        
        
    }
    
    
    
    
}



@end
