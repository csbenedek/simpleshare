//
//  VideoCaptureController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 16/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "VideoCaptureController.h"
#import "AttachedWindowsController.h"
#import "BoxSimpleShareAppDelegate.h"


@interface VideoCaptureController ()

@end

@implementation VideoCaptureController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    
    if (self) {
        // Initialization code here.
    }
    return self;
}

-(IBAction)backToMainWindow:(id)sender{
    
    //hide videoCaptureView
    [self.rootController hideAttachedWindow:self.rootController.currentAttachedWindow];
    
    //display mainWindow
    [self.rootController displayMainWindow] ;
    
    
    
    
}

-(IBAction)startVideoCapture:(id)sender{
    
    BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[BoxSimpleShareAppDelegate sharedDelegate];
    
    [delegate startVideoCapture:nil];
    
}





@end
