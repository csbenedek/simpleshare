//
//  BoxNetFileInfoLoader.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 26/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "BoxNetFileInfoLoader.h"
#import "OAuth2Client.h"
#import "AppConstants.h"


@implementation BoxNetFileInfoLoader



-(id)init{
    
    self = [super init];
    
    if (self) {
        
        self.thumbnailImageData = [[NSMutableData alloc] init];
        
        return self;
        
    }
    
    return nil;
    
}


-(void)loadThumbnailImageForID:(NSString *) ID{
    
    //compose string URL to get 128x128 thumbnail
    NSString *URLString = [NSString stringWithFormat:@"https://api.box.com/2.0/files/%@/thumbnail.png?min_height=128&min_width=128",ID];
    
    NSURL *URL = [NSURL URLWithString:URLString];
    
    //create request
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:URL];
    
    [request setHTTPMethod:@"GET"];
    
    //compose authorization string
    
    NSString *authString = [NSString stringWithFormat:@"Bearer %@",[[OAuth2Client sharedInstance] accessToken]];
    
    
    [request addValue:authString forHTTPHeaderField:@"Authorization"];
    
    [request addValue:ID forHTTPHeaderField:@"ID"];

    self.loadThumbnailURLConnection = [NSURLConnection connectionWithRequest:request delegate:self];
    
    
    
}

-(void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response{
    
    
    if (connection == self.loadThumbnailURLConnection) {
        
        
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
        
        NSInteger statusCode = httpResponse.statusCode;
        
        
        
        switch (statusCode) {
            case 202:{
                //get ID of file
                NSDictionary *originalHeaders = connection.originalRequest.allHTTPHeaderFields;
                
                NSString *ID = [originalHeaders valueForKey:@"ID"];
                
                NSLog(@"OriginalID: %@",ID);
                
                NSDictionary *headers = httpResponse.allHeaderFields;
                
                //NSLog(@"Status:%li",httpResponse.statusCode);
                
                //NSLog(@"Location:%@",[headers valueForKey:@"Location"]);
                
                NSInteger retryAfterInterval = [[headers valueForKey:@"Retry-After"] intValue];
                
                //NSLog(@"Retry-after:%@",[headers valueForKey:@"Retry-After"]);

                //retry after
                NSDictionary *userInfo = [NSDictionary dictionaryWithObject:ID forKey:@"ID"];
                
                
                
                //NSTimer *timer = [NSTimer timerWithTimeInterval:retryAfterInterval target:self selector:@selector(retryTimerMethod:) userInfo:userInfo repeats:false];
                
                [NSTimer scheduledTimerWithTimeInterval:retryAfterInterval target:self selector:@selector(retryTimerMethod:) userInfo:userInfo repeats:FALSE];
            
            break;
            }
                
                
                
            default:
                break;
        }
        
        
    }
    
    
    
    
}

-(void)retryTimerMethod:(NSTimer *)timer{
    
    
    NSLog(@"Retry connection");
    
    //get ID from timer's userInfo
    NSString *ID = [timer.userInfo objectForKey:@"ID"];
    
    //retry loading of thumbnail
    [self loadThumbnailImageForID:ID];
    
    
    
}



-(void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data{
    
    if (connection == self.loadThumbnailURLConnection) {
        
        [self.thumbnailImageData appendData:data];
        
    }
    
}

-(void)connectionDidFinishLoading:(NSURLConnection *)connection{
    
    if (connection == self.loadThumbnailURLConnection) {
        
        
        NSImage *image = [[NSImage alloc] initWithData:self.thumbnailImageData];
        
        
        self.thumbnailImage = image;
        
        
        //post notification with self as object
        
        [[NSNotificationCenter defaultCenter] postNotificationName:@"ThumbnailLoadedNotification" object:self];
        
        [image release];
        
        
        
        return;
        
    }
    
    
    
}

-(void)dealloc{
    
    safe_release(_thumbnailImage);
    safe_release(_thumbnailImageData);
    safe_release(_loadThumbnailURLConnection);
    
    
    [super dealloc];
    
    
}




@end
