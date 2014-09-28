//
//  AccountViewController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 21/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "AccountViewController.h"
#import "PreferencesController.h"
#import "CustomNSImageView.h"
#import "BoxNetHandler.h"
#import "BoxNetUser.h"
#import "OAuth2Client.h"
#import "BoxNetAccount.h"
#import "MainController.h"
#import "BoxSimpleShareAppDelegate.h"


@interface AccountViewController ()

@end

@implementation AccountViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
        
    }
    return self;
}


-(void)processBoxAccountInfoLoadedNotification:(NSNotification *)notification{
    
    [self.loginTextField setTitleWithMnemonic:self.rootController.account.login];
    
    
    self.avatarImageView.image = self.rootController.account.avatar;
    
    [self.view display];
    
    
    
}

-(IBAction)youtubeLoginButtonAction:(id)sender{
    
    //get main controller
    MainController *mainController = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    
    [mainController doYoutubeLogin:nil];
    
    
    
}



-(IBAction)loginButtonAction:(id)sender{
    
    //already authenticated, logout
    
    if ([[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated]) {
        
        PostNotification(@"DoLogoutNotification");//MainController -(void)processDoLogoutNotification
        
        
        self.loginButton.image = [NSImage imageNamed:@"box-account-login-button"];
        
        [self.loginTextField setTitleWithMnemonic:@""];
        
       // self.avatarImageView.image = [NSImage imageNamed:@"nouser"];
        
        [self.avatarImageView setImage:nil];
        
        [self.avatarImageView display];
        
    }
    
    //not authentivated, login
    
    else{
        
        [[OAuth2Client sharedInstance] authorize];
        
        
        self.loginButton.image = [NSImage imageNamed:@"box-account-logout-button"];
        
    }
    
    
}

//method to control login/logout button view

-(void)updateLoginButton{
    
    if ([[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated]) {
        
        
        self.loginButton.image = [NSImage imageNamed:@"box-account-logout-button"];
        
        
        
        
    }
    
    
    
}





@end
