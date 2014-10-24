//
//  LoginViewController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 07/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "LoginViewController.h"
#import "MAAttachedWindow.h"
#import "AttachedWindowsController.h"



#import "OAuth2Client.h"


@interface LoginViewController ()

@end

@implementation LoginViewController

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
    
    
    
    //instantiate and configure attached window
    
    if (!self.window) {
        
        //get a view from controller, this triggers lazy loading
        NSView *aView = self.view;
        
        //get a StatusItem location
        
        NSPoint point = [self getLocationOfStatusItem];

        
        MAAttachedWindow *window = [[MAAttachedWindow alloc] initWithView:aView attachedToPoint:point];
        
        self.window = window;
        
        
        [self configureAttachedWindow:self.window];
        
        [self.window setDelegate:self];
        
        
        
    }
    
    //make self active controller in AttachedWindowsController
    
    
    self.rootController.activeController = self;
    
    //show window
    
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    
    [self.window makeKeyAndOrderFront:self];
    
    
    
}


-(void)disableLoginButton{
    
    //[self.loginButton setEnabled:FALSE];
    
    //[self.window update];
    
    
}

-(IBAction)quit:(id)sender{
    
    [[NSApplication sharedApplication] terminate:nil];

    
}

//-(NSInteger)tableView

-(IBAction)loginButtonClicked:(id) sender{
    
    
    
    //post message after login
    
    //[[NSNotificationCenter defaultCenter] postNotificationName:@"ShowSuccessfulLoginMessageNotification" object:self userInfo:nil];
    
    [[OAuth2Client sharedInstance] authorize];
    
    //hide attached window
    //[[NSNotificationCenter defaultCenter] postNotificationName:@"HideAttachedWindowNotification" object:self userInfo:nil];
    
}


@end
