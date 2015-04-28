//
//  MAAttachedWindowCOntroller.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 18/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "MAAttachedWindowController.h"
#import "MAAttachedWindow.h"
#import "BoxSimpleShareAppDelegate.h"
#import "AttachedWindowsController.h"





@interface MAAttachedWindowController ()

@end

@implementation MAAttachedWindowController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
       
        //set Root controller
        
        BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[[NSApplication sharedApplication] delegate];
        
        self.rootController = delegate.attachedWindowsController;
        
        
        
        
    }
    return self;
}


-(void)showWindow{
    
    [self.window makeKeyAndOrderFront:self];
    
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    
    
    
}

-(void)hideWindow{
    
    [self.window orderOut:self];
    
    
}


-(NSPoint)getLocationOfStatusItem{
    
    BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[[NSApplication sharedApplication] delegate];
    
    NSRect mainScreenRect = [delegate getStatusItemRectInMainScreen];
    
    
    NSPoint point = mainScreenRect.origin;
    
    
    point.x = point.x + mainScreenRect.size.width/2;
    
    return point;
    
    
    
}


-(void)configureAttachedWindow:(MAAttachedWindow *)window{
    
    
    
    
    
}

#pragma mark - NSWindowDelegate methods
-(void)windowDidResignKey:(NSNotification *)notification{
    
    //hide window if focus lost
    
    [self hideWindow];
    
}


@end
