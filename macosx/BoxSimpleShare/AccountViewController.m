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
        //add KVO observing
        
        MainController *mainController = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
        
        [mainController addObserver:self forKeyPath:@"isYouTubeLogin" options:NSKeyValueObservingOptionNew context:nil];
        
        [mainController addObserver:self forKeyPath:@"isImgur" options:NSKeyValueObservingOptionNew context:nil];
        
        
        
    }
    return self;
}

-(void)loadView{
    [super loadView];
    
    MainController *mainController = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    //set youtubeLoginButton image
    
    BOOL isYouTubeLogin = mainController.isYouTubeLogin;
    
    
    if (isYouTubeLogin) {
        [self setYouTubeLogoutButtonImage];
    }
    
    else{
        
        
        [self setYouTubeLoginButtonImage];
        
    }
    
    
    
    BOOL isImgur = mainController.isImgur;
    
    if (isImgur) {
        [self setDontUseImgurImage];
    }
    else{
        
        
        [self setUseImgurImage];
    }
    
    
}


-(void)processBoxAccountInfoLoadedNotification:(NSNotification *)notification{
    
    [self.loginTextField setTitleWithMnemonic:self.rootController.account.login];
    
    
    self.avatarImageView.image = self.rootController.account.avatar;
    
    [self.view display];
    
    
    
}


-(IBAction)imgurButtonActon:(id)sender{
    
    //get main controller
    MainController *mainController = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    mainController.isImgur = !mainController.isImgur;
     
}


-(IBAction)youtubeLoginButtonAction:(id)sender{
    
    //get main controller
    MainController *mainController = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    if (!mainController.isYouTubeLogin) {
        
        [mainController doYoutubeLogin:nil];
    }
    else{
        
        [mainController doYoutubeLogout:nil];
        
    }
    
    
    
    
    
}



-(IBAction)loginButtonAction:(id)sender{
    
    //already authenticated, logout
    
    if ([[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated]) {
        
        PostNotification(@"DoLogoutNotification");//MainController -(void)processDoLogoutNotification
        
        //get main controller
        MainController *mainController = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];

        self.loginButton.image = [NSImage imageNamed:@"box-account-login-button"];
        
        [self.loginTextField setTitleWithMnemonic:@""];
        
       // self.avatarImageView.image = [NSImage imageNamed:@"nouser"];
        
        [self.avatarImageView setImage:nil];
        
        [self.avatarImageView display];
        
        
        //logout from Youtube
        [mainController doYoutubeLogout:nil];
        
        
        //reset Imgur preference
        mainController.isImgur = FALSE;
        
        
    }
    
    //not authenticated, login
    
    else{
        
        [[OAuth2Client sharedInstance] authorize];
        
        
        self.loginButton.image = [NSImage imageNamed:@"box-account-logout-button"];
        
    }
    
    
}

//method to control login/logout button view

#pragma mark - buttons images control methods
-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context{
    
    
    
    if ([keyPath isEqualTo: @"isYouTubeLogin"]) {
        
        //BOOL isYouTubeLogin = [[change valueForKey:@"NSKeyValueChangeNewKey"] boolValue];
        
        BOOL isYouTubeLogin = [[change valueForKey:@"new"] boolValue];
        
        if (isYouTubeLogin) {
            [self setYouTubeLogoutButtonImage];
        }
        
        else{
            
            
            [self setYouTubeLoginButtonImage];
            
        }
        
    }
    
    if ([keyPath isEqualToString:@"isImgur"]) {
        
        BOOL isImgur = [[change valueForKey:@"new"] boolValue];
        
        
        if (isImgur) {
            [self setDontUseImgurImage];
        }
        
        else{
            
            [self setUseImgurImage];
            
        }
        
        
    }
    
    
}



-(void)setUseImgurImage{
    
    
    self.imgurLoginButton.image = [NSImage imageNamed:@"use-imgur-button"];
    
}

-(void)setDontUseImgurImage{
    
    self.imgurLoginButton.image = [NSImage imageNamed:@"dont-use-imgur-button"];    
    
}


-(void)setYouTubeLoginButtonImage{
    
    self.youTubeLoginButton.image = [NSImage imageNamed:@"login-with-youtube-button"];
    
    [self.youTubeLoginButton display];
}

-(void)setYouTubeLogoutButtonImage{
    
    
    self.youTubeLoginButton.image = [NSImage imageNamed:@"logout-from-youtube-button"];
    
    [self.youTubeLoginButton display];
    
    
}




-(void)updateLoginButton{
    
    if ([[[BoxNetHandler sharedHandler] boxNetUser] isAuthenticated]) {
        
        
        self.loginButton.image = [NSImage imageNamed:@"box-account-logout-button"];
        
        
        
        
    }
    
    
    
}





@end
