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

    BOOL _isHighlighted;
    SEL _action;
    __unsafe_unretained id _target;
    
    
    BOOL isJustLaunched;

}

@property (assign, nonatomic) NSStatusItem *statusItem;
@property (nonatomic, unsafe_unretained) id target;
@property (nonatomic, unsafe_unretained) id delegate;
@property (nonatomic, setter = setHighlighted:) BOOL isHighlighted;
@property (nonatomic, readonly) NSRect globalRect;
@property (nonatomic) SEL action;

- (id) initWithStatusItem:(NSStatusItem *)item;

-(void)postStatusItemClickedNotification;

@end
