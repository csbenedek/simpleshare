//
//  LoginViewController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 07/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MAAttachedWindowController.h"
@class MAAttachedWindow;

@interface LoginViewController : MAAttachedWindowController

@property (nonatomic, retain) MAAttachedWindow *window;




-(IBAction)loginButtonClicked:(id) sender;

@end
