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


-(id)init{
    
    self = [super init];
    
    if (self) {
        
        //LoginViewController *controller = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
        
        //self.loginWindowController = controller;
        
        
        return self;
        
        
    }
    
    return nil;
    
}

/*
-(id)initWithSettings:(NSDictionary *)settingsDict{
    
    self = [super init];
    
    
    if (self) {
        
        self.isFirstLaunch = [[settingsDict valueForKey:@"isFirstLaunch"] boolValue];
        
       
        return self;
    }
    
    return nil;
}*/




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
    
    //[self.textMessageWindow orderOut:self];
    
}

#pragma mark - videoCaptureWindow

-(void)displayVideoCaptureWindow{
    
    if (!self.videoCaptureController) {
        
        //load VideoCaptureController form nib
        
        VideoCaptureController *controller = [[VideoCaptureController alloc] initWithNibName:@"VideoCaptureController" bundle:nil];
        
        
        self.videoCaptureController = controller;
        
        
    }
    
    
    [self.videoCaptureController showWindow];
    
 
    /*
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
    
    
    
    //self.currentAttachedWindow = self.videoCaptureWindow;
    
    [self showAttachedWindow:self.videoCaptureWindow];
    */
    
    
    
    
    
}


#pragma mark - loginWindow

-(void)displayLoginWindow{
    
    
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
    
    //hide SuccessfulLoginMessage
    
    [self.textMessageController hideWindow];
    
    
    //mainWindowController
    
    if (!self.mainViewController) {
        
        mainView *controller = [[mainView alloc] initWithNibName:@"mainView" bundle:nil];
        
        //store reference to mainView controller
        
        self.mainViewController = controller;

        
        
    }
    
    [self.mainViewController showWindow];
    
    
    
    
    
    
    //[self hideAttachedWindow:self.textMessageWindow];
    /*
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
    
    */
    
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
    
    //tell textMessageController to show start message
    
    [self.textMessageController showStartMessage];
    
    //this needed for processStatusItemClickedNotification
    
    self.isStartUpMessageVisible = TRUE;
    
    
    
    
}


-(void)processSuccessfulLoginNotification:(NSNotification *)notification{
    
    
    if (self.isFirstLaunch) {
        
         [self.textMessageController showSuccessfulLoginMessage];
        
        //this needed for processStatusItemClickedNotification
        self.isSuccessfulLoginMessageVisible = TRUE;
        
    }
    else{
        
        
        //display main window if it is not first launch
        
        [self displayMainWindow];
        
        
    }
    

    
}




-(void)processStatusItemClickedNotification{
    
    //NSLog(@"Status Item Clicked");
    
    //if startup message visible -> show login window
    //this is first launch and user not authorized
    if (self.isStartUpMessageVisible) {
        
        NSLog(@"Is StartupMessageVisible!");
        
        self.isStartUpMessageVisible = FALSE;
        
        [self.textMessageController hideWindow];
        
        [self displayLoginWindow];
        
        return;
    }
    
    if (self.isSuccessfulLoginMessageVisible) {
        
        NSLog(@"Is successful login visible!");
        
        self.isSuccessfulLoginMessageVisible = FALSE;
        
        
        [self.textMessageController hideWindow];
        
        [self displayMainWindow];
        
        return;
    }
    
    
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
    
    //else window of active controller
    else{
        
        if (self.isFirstLaunch) {
            
            [self displayMainWindow];
            
            self.isFirstLaunch = FALSE;
            
            
        }
        else{
            
            [self.activeController showWindow];
            
        }
        
        //[self displayMainWindow];
        
        
        
        
        
    }
    
    
    
    
}




-(void)processShowLoginWindowNotification:(NSNotification *)notification{
    
    [self displayLoginWindow];
    
}




@end
