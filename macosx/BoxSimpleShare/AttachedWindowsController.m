//
//  AttachedWindowsController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 05/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "AttachedWindowsController.h"

#import "TextMessageViewController.h"

#import "BoxSimpleShareAppDelegate.h"

#import "LoginViewController.h"

#import "BoxNetHandler.h"

#import "BoxNetUser.h"

#import "VideoCaptureController.h"

#import "mainView.h"

#import "TextMessageViewController.h"





@implementation AttachedWindowsController

#pragma mark - helpers
-(NSPoint)getLocationOfStatusItem{
    
    BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[[NSApplication sharedApplication] delegate];
    
    NSRect mainScreenRect = [delegate getStatusItemRectInMainScreen];
    
    
    NSPoint point = mainScreenRect.origin;
    
    
    point.x = point.x + mainScreenRect.size.width/2;
    
    return point;
    
}

+(AttachedWindowsController *)sharedController{
    
    BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[[NSApplication sharedApplication] delegate];
    
    AttachedWindowsController *controller = delegate.attachedWindowsController;
    
    
    return controller;
    
    
}



-(void)configureAttachedWindow:(MAAttachedWindow *)attachedWindow{

    
    attachedWindow.borderWidth = 0;
    
    [attachedWindow setBackgroundColor:[NSColor blackColor]];
    
    [attachedWindow setArrowBaseWidth:20.0];
    
    [attachedWindow setArrowHeight:10.0];
    
    
    
    
}

#pragma mark - show/display attached window

-(void)hideAttachedWindow:(MAAttachedWindow *)attachedWindow{
    
    
    [attachedWindow orderOut:self];
    
}


-(void)showAttachedWindow:(MAAttachedWindow *)attachedWindow{
    
    
    [attachedWindow makeKeyAndOrderFront:self];
    
}

-(void)switchAttachedWindow:(MAAttachedWindow *)attachedWindow{
    
    if ([attachedWindow isVisible]) {
        
        [self hideAttachedWindow:attachedWindow];
        
    }
    
    else {
        
        [self showAttachedWindow:attachedWindow];
        
    }
    
    
}





#pragma mark - textMessageWindow

-(void)hideTextMessageWindow{
    
    [self.textMessageWindow orderOut:self];
    
}

#pragma mark - videoCaptureWindow

-(void)displayVideoCaptureWindow{
    
    //hide current window
    
    [self hideAttachedWindow:self.currentAttachedWindow];
    
    if (!self.videoCaptureWindow) {
        
    
        
        //load VideoCaptureController form nib
        
        VideoCaptureController *controller = [[VideoCaptureController alloc] initWithNibName:@"VideoCaptureController" bundle:nil];
        
        //set pairing references
        
        self.videoCaptureController = controller;
        
        self.videoCaptureController.rootController = self;
        
        
        
        
        //view from controller
        
        NSView *aView = controller.view;
    
        //[self.mainWindow.contentView addSubview:aView];
    
        //get status item location
        
        NSPoint point = [self getLocationOfStatusItem];
        
        //instantiate and configure attached window
        
        
        MAAttachedWindow *videoCaptureWindow = [[MAAttachedWindow alloc] initWithView:aView attachedToPoint:point];
        
        
        
        [self configureAttachedWindow:videoCaptureWindow];
        
        [videoCaptureWindow setBackgroundColor:[NSColor whiteColor]];
        
        self.videoCaptureWindow = videoCaptureWindow;
        
        

    }
    
    
    
    self.currentAttachedWindow = self.videoCaptureWindow;
    
    [self showAttachedWindow:self.videoCaptureWindow];
    
    
    
    
    
    
}


#pragma mark - loginWindow

-(void)displayLoginWindow{
    
    
    //hide start Text message
    
    //[self hideTextMessageWindow];
    
    [self.textMessageController hideWindow];
    
    if (!self.loginWindowController) {
        
        LoginViewController *controller = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
        
        self.loginWindowController = controller;
        
        
    }
    
    [self.loginWindowController showWindow];
    
    
    
    
    /*
    if (!self.loginWindow) {
        
        //load LoginViewController from nib file
        
        LoginViewController *controller = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
        
        //get a view from controller, this triggers lazy loading
        NSView *aView = controller.view;
        
        //get a StatusItem location
        
        NSPoint point = [self getLocationOfStatusItem];
        
        //instantiate and configure attached window
        
        MAAttachedWindow *loginWindow = [[MAAttachedWindow alloc] initWithView:aView attachedToPoint:point];
        
        [self configureAttachedWindow:loginWindow];
        
        [loginWindow setBackgroundColor:[NSColor whiteColor]];
    
        
        self.loginWindow = loginWindow;
        
        
        
        
    }
    
    self.currentAttachedWindow = self.loginWindow;
    
    //display attached window
    
    [self showAttachedWindow:self.loginWindow];
    */
    
    
}

#pragma mark - Main window

-(void)displayMainWindow{
    
    
    //load and configure mainWindow from nib file
    
    [self hideAttachedWindow:self.textMessageWindow];
    
    if (!self.mainWindow) {
        mainView *controller = [[mainView alloc] initWithNibName:@"mainView" bundle:nil];
        
        //store reference to mainView controller
        
        self.mainViewController = controller;
        
        //set pairing reference
        
        self.mainViewController.rootController = self;
        
        

        //get a view from controller, this triggers lazy loading
        NSView *aView = controller.view;
        
        //get a StatusItem location
        
        NSPoint point = [self getLocationOfStatusItem];
        
        //instantiate attached window
        MAAttachedWindow *mainWindow = [[MAAttachedWindow alloc] initWithView:aView attachedToPoint:point];
        
        
        [self configureAttachedWindow:mainWindow];
        
        //mainWindow.borderWidth = 0;
        
        [mainWindow setBackgroundColor:[NSColor whiteColor]];
        
        //set delegate
        
        [mainWindow setDelegate:self.mainViewController];
        
        //store reference to window in controller
        
        self.mainViewController.window = mainWindow;
        
        
        self.mainWindow = mainWindow;
    }
    
    
#warning reposition needed!!!!
    
    self.currentAttachedWindow = self.mainWindow;

    //display attached window
    [self switchAttachedWindow:self.mainWindow];
    
    
    
}


#pragma mark - process notifications

-(void)processNewHistoryElementNotification:(NSNotification *)notification{
    
    NSLog(@"processNewHistoryElementNotification called!");
    
    [self.mainViewController updateHistoryElements];
    
}



-(void)processShowStartMessageNotification:(NSNotification *)notification{
    
    if (!self.textMessageController) {
        
        
        //load text message view controller from nib file
        TextMessageViewController *controller = [[TextMessageViewController alloc] initWithNibName:@"TextMessageViewController" bundle:nil];
        
        //store reference to controller
        
        self.textMessageController = controller;
        
        
    }
    
    [self.textMessageController showStartMessage];
    /*
    //this is just for triggering lazy loading
    NSView *aView1 = controller.view;
    
    //this view is really used
    NSView *aView2 = controller.startView;

    //get a location of statusItem
    
    
    NSPoint point = [self getLocationOfStatusItem];
    
    MAAttachedWindow *attachedWindow = [[MAAttachedWindow alloc] initWithView:aView2 attachedToPoint:point];
    
    //configure attached window
    [self configureAttachedWindow:attachedWindow];
    
    self.textMessageWindow = attachedWindow;
    
    //set delefate
    
    [self.textMessageWindow setDelegate:self.textMessageController];
    
    self.textMessageController.window = attachedWindow;
    
    
    
    //display attached window
    [self showAttachedWindow:self.textMessageWindow];
    */
    
    
    
    
}


//not used for now..

-(void)processShowTextMessageNotification:(NSNotification *)notification{
    
    NSDictionary *userInfo = notification.userInfo;
    
    NSInteger width = [[userInfo valueForKey:@"width"] intValue];
    
    NSString *string = [userInfo valueForKey:@"TextToDisplay"];
    
    //[self displayMessageWithText:string andWidth:width];
    
    
}



-(void)processShowSuccessfulLoginMessageNotification:(NSNotification *)notification{
    
    
    //hide login window
    [self hideAttachedWindow:self.loginWindow];
    
    
    //NSLog(@"%@",text);
    //load text message view controller from nib file
    TextMessageViewController *controller = [[TextMessageViewController alloc] initWithNibName:@"TextMessageViewController" bundle:nil];
    
    
    //this is just for triggering lazy loading
    NSView *aView1 = controller.view;
    
    //this view is really used
    NSView *aView2 = controller.successfulLoginView;
    
    
    
    //get a location of statusItem
    
    
    NSPoint point = [self getLocationOfStatusItem];
    
    //instantiate attached window
    
    MAAttachedWindow *attachedWindow = [[MAAttachedWindow alloc] initWithView:aView2 attachedToPoint:point];
    
    
    [self configureAttachedWindow:attachedWindow];
    
    self.textMessageWindow = attachedWindow;
    
    
    //display attached window
    self.currentAttachedWindow = self.textMessageWindow;
    
    
    [self showAttachedWindow:self.textMessageWindow];
    
    
    
    
}




-(void)processStatusItemClickedNotification{
    
    //NSLog(@"Status Item Clicked");
    
    //window of active controller is visible, hide it
    
    if ([self.activeController.window isVisible]) {
        
        //[self hideAttachedWindow:self.currentAttachedWindow];
        
        [self.activeController hideWindow];
        
        
        
        return;
    }
    
    
    
    //if not authenticated, display login window
    
    if (![[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated])
    {
        
        [self displayLoginWindow];
        
        
        
    }
    
    //else display main window
    else{
        
        
        [self displayMainWindow];
        
        
    }
    
    
    
    
}

#pragma mark -

/*
-(void)displayMessageWithText:(NSString *)text andWidth:(NSInteger)width{
    
    
    NSLog(@"%@",text);
    //load text message view controller from nib file
    TextMessageViewController *controller = [[TextMessageViewController alloc] initWithNibName:@"TextMessageViewController" bundle:nil];
    
    
    //get a view from controller, this triggers lazy loading
    NSView *aView = controller.view;
    
    //change width
    
    NSRect rect = [aView frame];
    
    rect.size.width = width;
    
    [aView setFrame:rect];
    
    //set text
    [controller.textField setTitleWithMnemonic:text];
    
    
    //get a location of statusItem
    
    
    NSPoint point = [self getLocationOfStatusItem];
    
    
    //instantiate and configure attached window
    
    MAAttachedWindow *textMessageAttachedWindow = [[MAAttachedWindow alloc] initWithView:aView attachedToPoint:point];
    
    textMessageAttachedWindow.borderWidth = 0;
    
    [textMessageAttachedWindow setBackgroundColor:[NSColor blackColor]];
    
    [textMessageAttachedWindow setArrowBaseWidth:20.0];
    
    [textMessageAttachedWindow setArrowHeight:10.0];
    
    
    
    self.textMessageWindow = textMessageAttachedWindow;

    //display attached window
    [self.textMessageWindow makeKeyAndOrderFront:self];
    
    
    
    
}
*/








-(void)processShowLoginWindowNotification:(NSNotification *)notification{
    
    [self displayLoginWindow];
    
}




@end
