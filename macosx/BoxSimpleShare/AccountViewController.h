//
//  AccountViewController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 21/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PreferencesController;
@class CustomNSImageView;



@interface AccountViewController : NSViewController
//root controller

@property(nonatomic, assign)PreferencesController * rootController;





@property(nonatomic, retain) IBOutlet NSButton *loginButton;
@property (nonatomic, retain) IBOutlet NSTextField  *loginTextField;
@property (nonatomic, retain) IBOutlet CustomNSImageView *avatarImageView;
@property (nonatomic, retain) IBOutlet NSButton *youTubeLoginButton;
@property (nonatomic, retain) IBOutlet NSButton *imgurLoginButton;





//button's actions methods
-(IBAction)loginButtonAction:(id)sender;

-(IBAction)imgurButtonActon:(id)sender;



//process notifications

-(void)processBoxAccountInfoLoadedNotification:(NSNotification *)notification;

//methods to set button's images
-(void)updateLoginButton;

-(void)setYouTubeLoginButtonImage;
-(void)setYouTubeLogoutButtonImage;

-(void)setUseImgurImage;
-(void)setDontUseImgurImage;






@end
