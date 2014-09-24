//
//  AccountViewController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 21/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class BoxNetAccount;
@class PreferencesController;
@class CustomNSImageView;



@interface AccountViewController : NSViewController
//root controller

@property(nonatomic, assign)PreferencesController * rootController;





@property (nonatomic, retain) BoxNetAccount *account;

@property (nonatomic, retain) IBOutlet NSTextField  *loginTextField;
@property (nonatomic, retain) IBOutlet CustomNSImageView *avatarImageView;








-(IBAction)getAccountInfo:(id)sender;



@end
