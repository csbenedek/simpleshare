//
//  CustomNSView.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/9/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "CustomNSView.h"


@implementation CustomNSView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
        
        enabled = YES;
        
    }
    
    return self;
}

-(void)drawRect:(NSRect)dirtyRect
{
    [[NSColor whiteColor] setFill];
    NSRectFill(dirtyRect);
}
- (void)dealloc
{
    [super dealloc];
}

- (NSView *) hitTest:(NSPoint)aPoint
{
    if (enabled)
        return [super hitTest:aPoint];
    
    return nil;
}

@end
