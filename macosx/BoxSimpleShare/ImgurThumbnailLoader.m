//
//  ImgurThumbnailLoader.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 04/10/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "ImgurThumbnailLoader.h"

@implementation ImgurThumbnailLoader

-(id)init{
    
    self = [super init];
    
    if (self) {
        
        self.data = [[NSMutableData alloc] init];
        
        return self;
        
        
    }
    return nil;
    
}


-(void)loadThumbnailForImageWithID:(NSString *)ID{
    
    NSString *URLString = [NSString stringWithFormat:@"http://i.imgur.com/%@t.jpg",ID];
    
    //http://i.imgur.com/12345m.jpg
    
    //create request
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:URLString]];
    
    
    [request setHTTPMethod:@"GET"];
    
    //needed for retry
    [request addValue:ID forHTTPHeaderField:@"ID"];
    
    
    [NSURLConnection connectionWithRequest:request delegate:self];
    
}


-(void)retryTimerMethod:(NSTimer *)timer{
    
    NSString *ID = (NSString *)timer.userInfo;
    
    [self loadThumbnailForImageWithID:ID];
    
    
}

#pragma mark - NSURLConnection delegate methods


-(void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data{
    
    
    [self.data appendData:data];
    
    
    
}


-(void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response{
    
    
    
    
    
    NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
    
    NSInteger statusCode = [httpResponse statusCode];
    
    NSURLRequest *request = connection.originalRequest;
    
    NSDictionary *originalHeaders = [request allHTTPHeaderFields];
    
    
    
    if (statusCode == 404) {
        NSLog(@"Thumbnail not ready yet, retrying in 5 second");
        
        [connection cancel];
        
        [NSTimer scheduledTimerWithTimeInterval:5 target:self selector:@selector(retryTimerMethod:) userInfo:[originalHeaders valueForKey:@"ID"] repeats:FALSE];
        
        
        
    }
    
    
    
}


-(void)connectionDidFinishLoading:(NSURLConnection *)connection{
    
    
    self.thumbnail = [[NSImage alloc] initWithData:self.data];
    
    //post youtube thumbnail ready notification
    
    PostNotificationWithObject(@"ImgurThumbnailLoadedNotification", self);
    
    
}

@end
