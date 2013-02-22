//
//  StatusItemView.h
//  BoxSimpleShare
//
//  Created by Syed Arsalan Pervez on 7/15/11.
//  Copyright 2011 SAPLogix. All rights reserved.
//


@interface StatusItemView : NSView <NSMenuDelegate>
{
    CALayer* rootLayer; // weak, retained by hierarchy
    CALayer* imageLayer; // weak, retained by hierarchy
    NSTextField *statusText;
    
    
    NSStatusItem *statusItem;
    id layerDelegate;
    BOOL mouseDown;
    
    int currentProgressImageIndex;
    NSMutableArray* imageQueue;
}

@property (assign, nonatomic) NSStatusItem *statusItem;

- (id) initWithStatusItem:(NSStatusItem *)item;

@end
