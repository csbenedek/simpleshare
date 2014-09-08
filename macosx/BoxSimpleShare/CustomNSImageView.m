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
    if (!_imgURL)
    {
        [self setImage:nil];
        return;
    }
    
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


-(void)drawRect:(NSRect)dirtyRect
{
    
    NSRect imageRect =  CGRectInset(self.bounds, 1.0, 1.0);
    
    
    [NSGraphicsContext saveGraphicsState];
    
    float radius =  imageRect.size.height / 2;
    NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:imageRect
                                                         xRadius:radius
                                                         yRadius:radius];
    [path addClip];
    
    NSImage* img  = self.image == nil ?[NSImage imageNamed:@"nouser"] :self.image;
    [img drawInRect:imageRect
           fromRect:NSZeroRect
          operation:NSCompositeSourceOver
           fraction:1.0];
    
    
    [NSGraphicsContext restoreGraphicsState];
    
}

- (void)dealloc
{
    [_imgURL release];
    [self.image release];
    [super dealloc];
}

@end
