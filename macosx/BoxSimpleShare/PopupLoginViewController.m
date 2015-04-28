//
//  PopupLoginViewController.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 6/30/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "PopupLoginViewController.h"
#import "MainController.h"
#import "BoxSimpleShareAppDelegate.h"

@interface PopupLoginViewController ()

@end

@implementation PopupLoginViewController

static PopupLoginViewController* _loginController = nil;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    return self;
}


+ (PopupLoginViewController*) shareController
{
    if (_loginController == nil) {
        _loginController = [[PopupLoginViewController alloc] initWithNibName:@"PopupLoginViewController" bundle:nil];
    }
    return _loginController;
}

-(IBAction)performLogin:(id)sender
{
    MainController *controller = [[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    [controller doLogin:nil];
}
@end
