//
//  AccountViewController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 21/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "AccountViewController.h"
#import "BoxNetAccount.h"
#import "PreferencesController.h"
#import "CustomNSImageView.h"

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

-(IBAction)getAccountInfo:(id)sender{
    
    [self.account populateInfo];
    
    
}







@end
