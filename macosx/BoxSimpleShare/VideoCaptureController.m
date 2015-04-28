//
//  VideoCaptureController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 16/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "VideoCaptureController.h"
#import "AttachedWindowsController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "MainController.h"
#import "ITSwitch.h"



@interface VideoCaptureController ()

@end

@implementation VideoCaptureController

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
    
    [window setBackgroundColor:[NSColor whiteColor]];
    
    [window setArrowBaseWidth:20.0];
    
    [window setArrowHeight:10.0];
    
    
    
    
}


-(void)showWindow{
    
    
    if (!self.window) {
        
        //view from controller
        
        NSView *aView = self.view;
        
        //get status item location
        
        NSPoint point = [self getLocationOfStatusItem];
        
        //instantiate and configure attached window
        
        
        MAAttachedWindow *videoCaptureWindow = [[MAAttachedWindow alloc] initWithView:aView attachedToPoint:point];
        
        //configure
        
        [self configureAttachedWindow:videoCaptureWindow];
        
        //store reference to window
        self.window = videoCaptureWindow;
        
        
        //set NSWindow delegate
        
        [self.window setDelegate:self];
        
        
        //set microphone switch state
        MainController *mainController = (MainController *)[[BoxSimpleShareAppDelegate sharedDelegate] mainController];
        
        self.microphoneSwitch.isOn = !mainController.mute_audio_check;
        
        
        
    }
    
    
    //make self active controller in AttachedWindowsController
    
    self.rootController.activeController = self;
    
    //show window
    
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    
    [self.window makeKeyAndOrderFront:self];
    
    
    
}




-(IBAction)backToMainWindow:(id)sender{
    
    //hide videoCaptureView
    
    [self hideWindow];
    
    
    //display mainWindow
    [self.rootController displayMainWindow] ;
    
    
    
    
}

-(IBAction)microphoneSwitchAction:(id)sender{
    
    //set mute audio check
    
    MainController *mainController = (MainController *)[[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    mainController.mute_audio_check = !self.microphoneSwitch.isOn;
    
    
    
}

-(IBAction)startVideoCapture:(id)sender{
    
    //hide attached window
    [self hideWindow];
    
    
    //start video capture
    BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[BoxSimpleShareAppDelegate sharedDelegate];
    
    [delegate startVideoCapture:nil];
    
}





@end
