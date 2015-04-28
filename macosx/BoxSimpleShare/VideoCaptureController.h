//
//  VideoCaptureController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 16/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MAAttachedWindowController.h"



@class ITSwitch;
@class AttachedWindowsController;



@interface VideoCaptureController : MAAttachedWindowController

@property (nonatomic, assign) AttachedWindowsController *rootController;
@property (nonatomic, retain) IBOutlet ITSwitch *microphoneSwitch;






-(IBAction)backToMainWindow:(id)sender;

-(IBAction)startVideoCapture:(id)sender;


-(IBAction)microphoneSwitchAction:(id)sender;

@end
