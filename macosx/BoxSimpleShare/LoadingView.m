//
//  LoadingView.m
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/9/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//

#import "LoadingView.h"


@implementation LoadingView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
        
        enabled = YES;
        
        indicator = [[NSProgressIndicator alloc] initWithFrame:NSMakeRect(frame.size.width/2-75, frame.size.height/2+5, 150, 30)];
        
        [indicator startAnimation:self];
        
        text = [[NSTextField alloc] initWithFrame:NSMakeRect(frame.size.width/2-150, frame.size.height/2-95, 300, 100)];
        
        [text setBackgroundColor:[NSColor clearColor]];
        [text setTextColor:[NSColor whiteColor]];
        [text setAlignment:NSCenterTextAlignment];
        [text setStringValue:@""];
        [text setEditable:NO];
        [text setEnabled:NO];
        [text setBordered:NO];
        [text setBezeled:NO];
        
        [text setFont:[NSFont fontWithName:@"Helvetica" size:14]];
        
        [self addSubview:indicator];
        [self addSubview:text];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateText:) name:@"UPDATED_LOADING_TEXT" object:nil];
    }
    
    return self;
    
}

- (void) updateText:(NSNotification *)notification
{
    if (notification && [notification object])
        [self setText:[notification object]];
}

- (void) setText:(NSString *)string
{
    [text setStringValue:string];
}

- (void) drawRect:(NSRect)dirtyRect
{
    // Fill in background Color
    CGContextRef context = (CGContextRef) [[NSGraphicsContext currentContext] graphicsPort];
    CGContextSetRGBFillColor(context, 0,0,0,0.7);
    CGContextFillRect(context, NSRectToCGRect(dirtyRect));
}

- (NSView *) hitTest:(NSPoint)aPoint
{
    if (enabled)
        return [super hitTest:aPoint];
    
    return nil;
}

- (void)dealloc
{
    safe_release(indicator);
    safe_release(text);
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    [super dealloc];
}

@end
