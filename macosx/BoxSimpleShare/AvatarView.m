//
//  AvatarView.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 8/24/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "AvatarView.h"

@implementation AvatarView



-(void)drawRect:(NSRect)dirtyRect
{
    
    NSRect imageRect =  CGRectInset(self.bounds, 1.0, 1.0);
    
    
    [NSGraphicsContext saveGraphicsState];
    
    float radius =  imageRect.size.height / 2;
    NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:imageRect
                                                         xRadius:radius
                                                         yRadius:radius];
    [path addClip];
    
        NSImage* img =  self.avatar == nil ?[NSImage imageNamed:@"nouser"] :self.avatar;
        [img drawInRect:imageRect
                 fromRect:NSZeroRect
                operation:NSCompositeSourceOver
                 fraction:1.0];
    
    
    [NSGraphicsContext restoreGraphicsState];
    
}

@end
