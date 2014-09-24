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
#import "NSString+URLEncoding.h"
#import "NSString+HTMLTags.h"


@implementation BoxNetAccount


-(void)populateInfo{
    
    NSMutableURLRequest *request = [self makeRequest];
    
    //start connection
    
    self.getInfoConnection = [NSURLConnection connectionWithRequest:request delegate:self];
    
    
}

-(void)loadAvatar{
    
    
    NSMutableURLRequest *request = [self makeAvatarRequest];
    
    
    
    //start connection
    self.getAvatarConnection = [NSURLConnection connectionWithRequest:request delegate:self];
    
    
    
}


-(NSMutableURLRequest *)makeAvatarRequest{
    
    //create URL to get current user info
    NSURL *URL = [NSURL URLWithString:self.avatarURL];
    
    //NSURL *URL = [NSURL URLWithString:@"https%3A%2F%2Fapp.box.com%2Findex.php%3Frm%3Dpic_storage_auth%26pic%3DaFFx2gJdWLEbLcATUXHUW_dEB0ZmGOrCQ-H5w_LSrU0hulnEc3G7xoxrUBVp4QmrpLCS7KYIEyBaDdx2WyHDKcGrf6piOawwxvCaYnio2bKcFPKUSdgqpZdOOD_f3gjDdomZ4ANxZmxKGjAFM54WDc3tOkA1MUWb2XC7wuIRF7oAmu5pLUP1oYCyuJ88BPPtIx79fZ40YRaEMdyLiSG22kI2S6HEL1jbrPbDvcWOle_2-B9oWepnNhkGqEZkubf5wRQAJ7a9b4sCzBYDpiQF4Ee9VfGsrIwmlPpnp7Xl5qY%2C"];
    
    
    //create request
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:URL];
    
    //[request setHTTPMethod:@"GET"];
    
    //compose authorization string
    
    //NSString *authString = [NSString stringWithFormat:@"Bearer %@",[[OAuth2Client sharedInstance] accessToken]];
    
    
    //[request addValue:authString forHTTPHeaderField:@"Authorization"];
    
    
    
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
        
        //we have avatar url, now we load image
        NSLog(@"User info loaded!");
        [self loadAvatar];
        
        
    }
    
    if (connection == self.getAvatarConnection) {
        
        NSLog(@"Avatar loaded");
        
        self.avatar = [[NSImage alloc] initWithData:self.imageData];
        
        PostNotification(@"BoxAccountInfoLoadedNotification");
        
        
    }
    
    
    
}
/*
-(NSURLRequest *)connection:(NSURLConnection *)connection willSendRequest:(NSURLRequest *)request redirectResponse:(NSURLResponse *)response{
    
    NSLog(@"Will send request");
    
    NSURL *URL = request.URL;
    
    NSString *path = [URL absoluteString];
    
    
    if ([path containsString:@"?"]) {
        
        NSArray *parts = [path componentsSeparatedByString:@"?"];
        
        path = [NSString stringWithFormat:@"%@?%@",[parts objectAtIndex:0],[[parts objectAtIndex:1] urlEncodeUsingEncoding:NSUTF8StringEncoding]];
        
        
    }
    

    
    NSURL *newURL = [NSURL URLWithString:path];
    
    NSMutableURLRequest *newRequest = [request mutableCopy];
    
    [newRequest setURL:newURL];
    
    return newRequest;
    
}*/


-(void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error{
    
    NSLog(@"Connection error:%@",error);
    
    
    
}

-(void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response{
    
    
    
    
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
        
        if (!self.imageData) {
            self.imageData = [[NSMutableData alloc] init];
        }
        
        [self.imageData appendData:data];
        
        
    }
    
    
    
    
}



@end
