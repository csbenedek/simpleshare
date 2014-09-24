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

-(IBAction)loginButtonAction:(id)sender;


@property(nonatomic, retain) IBOutlet NSButton *loginButton;
@property (nonatomic, retain) IBOutlet NSTextField  *loginTextField;
@property (nonatomic, retain) IBOutlet CustomNSImageView *avatarImageView;



-(void)updateLoginButton;


//process notifications

-(void)processBoxAccountInfoLoadedNotification:(NSNotification *)notification;





@end
