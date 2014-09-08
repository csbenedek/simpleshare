//
//  DragDropWindowController.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 5/5/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "DragDropWindowController.h"
#import "BoxNetHandler.h"
@interface DragDropWindowController ()

@end

@implementation DragDropWindowController

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

-(void) awakeFromNib
{
    [self.window setBackgroundColor:[NSColor clearColor]];
}
+(DragDropWindowController*) shareController {
    
    static DragDropWindowController* _instance = nil;
    if (_instance == nil) {
        _instance = [[DragDropWindowController alloc] initWithWindowNibName:@"DragDropWindowController"];
    }
    
    return _instance;
}

-(void)uploadFiles:(NSArray*) files {
    
    if (files == nil ) {
        [self endSheetWithReturnCode:0];
    }
    else
    {
        [[BoxNetHandler sharedHandler] uploadFiles:files withProperties:nil];
        [self endSheetWithReturnCode:1];
    }
}
@end
