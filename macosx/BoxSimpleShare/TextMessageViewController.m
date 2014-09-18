//
//  TextMessageViewController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 05/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "TextMessageViewController.h"
#import "MAAttachedWindow.h"
#import "MAAttachedWindowController.h"


@interface TextMessageViewController ()

@end

@implementation TextMessageViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    return self;
}


-(void)configureAttachedWindow:(MAAttachedWindow *)window{
    
    window.borderWidth = 0;
    
    [window setBackgroundColor:[NSColor blackColor]];
    
    [window setArrowBaseWidth:20.0];
    
    [window setArrowHeight:10.0];
    
    
}


-(void)showStartMessage{
    
    //this is just for triggering lazy loading
    NSView *aView1 = self.view;
    
    //this view is really used
    NSView *aView2 = self.startView;
    
    //get a location of statusItem
    
    
    NSPoint point = [self getLocationOfStatusItem];
    
    MAAttachedWindow *attachedWindow = [[MAAttachedWindow alloc] initWithView:aView2 attachedToPoint:point];
    
    //configure attached window
    [self configureAttachedWindow:attachedWindow];
    
    self.window = attachedWindow;
    
    //set delegate
    
    [self.window setDelegate:self];
    
    
    //display attached window
    
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    
    [self.window makeKeyAndOrderFront:self];
    
    
    
}

#pragma mark - NSWindowDelegate methods


-(void)windowDidResignKey:(NSNotification *)notification{
    
    
    [self.window orderOut:self];
    
    
    
}

@end
