//
//  MainPreferencesViewController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 20/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "MainPreferencesViewController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "MainController.h"
#import "ITSwitch.h"

@interface MainPreferencesViewController ()

@end

@implementation MainPreferencesViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
        
        
        
        
        
    }
    return self;
}


-(void)awakeFromNib{
    
    [super awakeFromNib];
    
    MainController *controller = (MainController *)[[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    //set states of preferences switches
    self.shortenLinksSwitch.isOn = controller.shorten_links_check;
    self.launchAtStartUpSwitch.isOn = controller.launch_at_startup_check;
    self.deleteScreenshotsSwitch.isOn = controller.delete_screenshot_after_upload_check;
    self.deleteAllFileTypesSwitch.isOn = controller.delete_all_after_upload_check;
    self.copyToPasteboardSwitch.isOn = controller.copy_url_to_clipboard_check;
    
    
    
    
}

#pragma mark - Actions

-(IBAction)shortenLinksControlSwitched:(id)sender{
    
    NSLog(@"Shorten links with google switch");
    
    
    
    MainController *controller = (MainController *)[[BoxSimpleShareAppDelegate   sharedDelegate] mainController];
    
    ITSwitch *control = (ITSwitch *)sender;
    
    controller.shorten_links_check = control.isOn;
    
    //mark preferences as dirty
    
    [controller preferencePageValueChangeNotification:nil];
    
 
    
}


-(IBAction)launchAtLoginControlSwitched:(id)sender{
    
    MainController *controller = (MainController *)[[BoxSimpleShareAppDelegate   sharedDelegate] mainController];
    
    ITSwitch *control = (ITSwitch *)sender;
    
    controller.launch_at_startup_check = control.isOn;
    
    //mark preferences as dirty
    
    [controller preferencePageValueChangeNotification:nil];
    
    
}


-(IBAction)deleteScreenshotsControlSwitched:(id)sender{
    
    MainController *controller = (MainController *)[[BoxSimpleShareAppDelegate   sharedDelegate] mainController];
    
    ITSwitch *control = (ITSwitch *)sender;

    controller.delete_screenshot_after_upload_check = control.isOn;
    
    
    //mark preferences as dirty
    
    [controller preferencePageValueChangeNotification:nil];
    
    
}


-(IBAction)deleteAllFileTypesControlSwitched:(id)sender{
    
    MainController *controller = (MainController *)[[BoxSimpleShareAppDelegate   sharedDelegate] mainController];
    
    ITSwitch *control = (ITSwitch *)sender;
    
    controller.delete_all_after_upload_check = control.isOn;
    
    
    //mark preferences as dirty
    
    [controller preferencePageValueChangeNotification:nil];


    
}


-(IBAction)copyToPasteboardControlSwitched:(id)sender{
    
    MainController *controller = (MainController *)[[BoxSimpleShareAppDelegate   sharedDelegate] mainController];
    
    ITSwitch *control = (ITSwitch *)sender;
    
    controller.copy_url_to_clipboard_check = control.isOn;
    
    
    //mark preferences as dirty
    
    [controller preferencePageValueChangeNotification:nil];
    

}



@end
