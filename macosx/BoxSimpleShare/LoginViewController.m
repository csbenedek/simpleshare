//
//  LoginViewController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 07/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "LoginViewController.h"

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




//-(NSInteger)tableView

-(IBAction)loginButtonClicked:(id) sender{
    
    
    
    //post message after login
    
    //[[NSNotificationCenter defaultCenter] postNotificationName:@"ShowSuccessfulLoginMessageNotification" object:self userInfo:nil];
    
    [[OAuth2Client sharedInstance] authorize];
    
    //hide attached window
    //[[NSNotificationCenter defaultCenter] postNotificationName:@"HideAttachedWindowNotification" object:self userInfo:nil];
    
}


@end
