//
//  HeaderView.m
//  LiquidFiles
//
//  Created by Cuongnq on 11/21/13.
//  Copyright (c) 2013 Cuongnq. All rights reserved.
//

#import "HeaderView.h"

@implementation HeaderView



-(void)drawRect:(NSRect)dirtyRect
{
   
   // [super drawRect:dirtyRect];
    
    NSRect rect = [self bounds];
    
//    [[NSColor greenColor] setFill];
//    NSRectFill(rect);
    
    NSBezierPath *path = [NSBezierPath bezierPath];
    
    [path moveToPoint:NSMakePoint(NSMinX(rect) + 1, NSMinY(rect))];
    [path lineToPoint:NSMakePoint(NSMaxX(rect) -1, NSMinY(rect))];
    [path closePath];
    
    [[NSColor secondarySelectedControlColor] set];
    [path stroke];
    

}

@end
