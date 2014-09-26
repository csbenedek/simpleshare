//
//  MainPreferencesViewController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 20/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ITSwitch;

@interface MainPreferencesViewController : NSViewController

@property(nonatomic, retain) IBOutlet ITSwitch *launchAtStartUpSwitch;
@property(nonatomic, retain) IBOutlet ITSwitch *shortenLinksSwitch;
@property(nonatomic, retain) IBOutlet ITSwitch *deleteScreenshotsSwitch;
@property(nonatomic, retain) IBOutlet ITSwitch *deleteAllFileTypesSwitch;
@property(nonatomic, retain) IBOutlet ITSwitch *copyToPasteboardSwitch;



-(void)updateSwitchesState;


-(IBAction)launchAtLoginControlSwitched:(id)sender;
-(IBAction)shortenLinksControlSwitched:(id)sender;
-(IBAction)deleteScreenshotsControlSwitched:(id)sender;
-(IBAction)deleteAllFileTypesControlSwitched:(id)sender;
-(IBAction)copyToPasteboardControlSwitched:(id)sender;






@end
