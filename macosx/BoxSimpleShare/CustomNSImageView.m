//
//  CustomNSImageView.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/9/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "CustomNSImageView.h"
#import "HTTPRequest.h"

@implementation CustomNSImageView

@synthesize imageURL = _imgURL;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void) setImageURL:(NSString *)url
{
    _imgURL = [[NSString stringWithString:url] retain];
    [self reloadImage];
}

- (void) reloadImage
{
    if (!_imgURL) return;
    
    HTTPRequest *request = [HTTPRequest requestWithURL:[NSURL URLWithString:_imgURL]];
    [request addMulticastDelegate:self];
    [request startAsynchronous];
    request = nil;
}

- (void) requestFinished:(id) request
{
    NSImage *img = [[NSImage alloc] initWithData:[request responseData]];
    
    if (img)
    {
        [self setImage:img];
    }
    
    [img release];
}

- (void) requestFailed:(id) request
{
    
}

- (void)dealloc
{
    [_imgURL release];
    
    [super dealloc];
}

@end
