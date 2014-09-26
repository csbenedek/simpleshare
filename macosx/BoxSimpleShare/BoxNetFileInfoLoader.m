//
//  BoxNetFileInfoLoader.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 26/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "BoxNetFileInfoLoader.h"
#import "OAuth2Client.h"


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
    
    //compose string URL to get 32x32 thumbnail
    NSString *URLString = [NSString stringWithFormat:@"https://api.box.com/2.0/files/%@/thumbnail.png?max_height=32&max_width=32",ID];
    
    NSURL *URL = [NSURL URLWithString:URLString];
    
    //create request
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:URL];
    
    [request setHTTPMethod:@"GET"];
    
    //compose authorization string
    
    NSString *authString = [NSString stringWithFormat:@"Bearer %@",[[OAuth2Client sharedInstance] accessToken]];
    
    
    [request addValue:authString forHTTPHeaderField:@"Authorization"];

    self.loadThumbnailURLConnection = [NSURLConnection connectionWithRequest:request delegate:self];
    
    
    
}

-(void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data{
    
    if (connection == self.loadThumbnailURLConnection) {
        
        [self.thumbnailImageData appendData:data];
        
    }
    
}

-(void)connectionDidFinishLoading:(NSURLConnection *)connection{
    
    if (connection == self.loadThumbnailURLConnection) {
        
        
        self.thumbnailImage = [[NSImage alloc] initWithData:self.thumbnailImageData];
        
        //post notification with self as object
        
        [[NSNotificationCenter defaultCenter] postNotificationName:@"ThumbnailLoadedNotification" object:self];
        
        
        
        return;
        
    }
    
    
    
}




@end
