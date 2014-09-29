//
//  mainView.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 12/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "mainView.h"
#import "MainViewCellController.h"
#import "BoxSimpleShareAppDelegate.h"
#import "BoxFile.h"
#import "AttachedWindowsController.h"
#import "MainController.h"
#import "BoxNetFileInfoLoader.h"
#import "ITSwitch.h"



@interface mainView ()

@end

@implementation mainView

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
    
        
        
        
    }
    return self;
}

#pragma mark - show/hide methods
-(void)configureAttachedWindow:(MAAttachedWindow *)window{
    
    window.borderWidth = 0;
    
    [window setBackgroundColor:[NSColor whiteColor]];
    
    [window setArrowBaseWidth:20.0];
    
    [window setArrowHeight:10.0];
    
    
    
}



-(void)showWindow{
    
    if (!self.window) {
        
        //get a view from controller, this triggers lazy loading
        
        NSView *aView = self.view;
        
        //get a StatusItem location
        
        NSPoint point = [self getLocationOfStatusItem];
        
        //instantiate attached window
        MAAttachedWindow *mainWindow = [[MAAttachedWindow alloc] initWithView:aView attachedToPoint:point];
        
        
        [self configureAttachedWindow:mainWindow];
        
        
        //set delegate
        
        [mainWindow setDelegate:self];
        
        //store reference to window in controller
        
        self.window = mainWindow;
        
        
    }
    
    //make self active controller in AttachedWindowsController
    
    self.rootController.activeController = self;
    
    //show window
    
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    
    [self.window makeKeyAndOrderFront:self];

    
    
}

-(void)awakeFromNib{
    
     MainController *controller = (MainController *)[[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    NSLog(@"Main VIew awake from nib!");
    
    //init cells array
    
    self.cells = [[NSMutableArray alloc] init];
    
    
    
    //initial population of history elements array
    [self updateHistoryElements];
    
    //register nib for MainViewCell identifier
    NSNib *nib = [[NSNib alloc] initWithNibNamed:@"MainViewCellController" bundle:nil];
    
    [self.tableView registerNib:nib forIdentifier:@"MainViewCell"];
    
    //set extended delegate to self
    
    self.tableView.extendedDelegate = self;
    
    //set enabled load switch state
    
    self.enableUploadSwitch.isOn = controller.disable_automatic_upload_check;
    
    
}


#pragma mark - Table methods

-(BOOL)tableView:(NSTableView *)tableView shouldSelectRow:(NSInteger)row{
    
    //disable selection
    
    return FALSE;
    
}


-(void)tableView:(NSTableView *)tableView didClickedRow:(NSInteger)row{
    
    //NSLog(@"Did clicked row:%li",row);
    
    [[self.cells objectAtIndex:row] openSharedLink];
    
    
    
}

-(void)updateHistoryElements{
    
    //clear cells array
    
    [self.cells removeAllObjects];
    
    //get array of uploaded items
    
    BoxSimpleShareAppDelegate *appDelegate = (BoxSimpleShareAppDelegate *)[[NSApplication sharedApplication] delegate];
    
    //go through list of saved files in reverse order
    
    for (BoxFile *file in [appDelegate.filesUploadedInSession reverseObjectEnumerator]) {
        
        //make cell for each file
        
        //controller to load nib and set properties
        MainViewCellController *controller = [[MainViewCellController alloc] initWithNibName:@"MainViewCellController" bundle:nil];
        
        //just triggering lazy loading
        NSView *tmpView = controller.view;
        
        
        //configure border and corners
        /*
        [controller.imageView setWantsLayer:TRUE];
        
        controller.imageView.layer.borderWidth = 1.0;
        controller.imageView.layer.borderColor = [[NSColor grayColor] CGColor];
        controller.imageView.layer.cornerRadius = 4.0;
        controller.imageView.layer.masksToBounds = TRUE;
         */
        //set filename
        
        [controller.fileName setTitleWithMnemonic:file.fileName];
        
        //stamp date if not presented
        
        
        if (!file.uploadDate) {
            
            NSLog(@"Stamping date");
            
            file.uploadDate = [NSDate date];
            
            
        }
        
        //set date in controller
        
        NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
        formatter.doesRelativeDateFormatting = YES;
        formatter.locale = [NSLocale currentLocale];
        formatter.dateStyle = NSDateFormatterShortStyle;
        formatter.timeStyle = NSDateFormatterShortStyle;
        NSString *timeString = [formatter stringFromDate:file.uploadDate];
        
        [controller.uploadDateTextField setTitleWithMnemonic:timeString];
        
        [formatter release];
        
        //set reference to BoxFile
        controller.boxFile = file;
        
        //if box file doesn't contain thumbnail image,load it
        
        if (!controller.boxFile.thumbnailImage) {
            
            [controller.fileInfoLoader loadThumbnailImageForID:file.fileID];
            
            //show spinner and start animation
            [controller.spinner setHidden:FALSE];
            [controller.spinner startAnimation:self];
            
            
        }
        
        else{
            
            controller.imageView.image = controller.boxFile.thumbnailImage;
            
        }
        
        
        //store controller
        
        [self.cells addObject:controller];
       
    }
  
    //reload table view
    [self.tableView reloadData];
   
    
}



-(NSView *) tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row{
    
    //return cell from cells array
    
    MainViewCellController *cellController = [self.cells objectAtIndex:row];
    
    return cellController.view;
    
    
}




-(NSInteger)numberOfRowsInTableView:(NSTableView *)tableView{
    
    
    
    return [self.cells count];
    
   
    
}



#pragma mark - NSWindowDelegate methods

-(void)windowDidResignKey:(NSNotification *)notification{
    
    
    NSLog(@"Main window did resign key");
    
    [self.window orderOut:self];
    
    
}

#pragma mark - process notifications methods

-(void)processNewHistoryElementNotification:(NSNotification *)notification{
    
    
    [self updateHistoryElements];
    
    
    
}



#pragma mark - interface Actions
-(IBAction)toggleEnableUploads:(id)sender{
    
    MainController *mainController = (MainController *)[[BoxSimpleShareAppDelegate sharedDelegate] mainController];
    
    
    [mainController toggleUploadsEnabled];
    
    
}



#pragma mark - Helpers




-(IBAction)quitApp:(id)sender{
    
    BoxSimpleShareAppDelegate *delegate =(BoxSimpleShareAppDelegate *)[[NSApplication sharedApplication] delegate];
    
    
    [delegate quitApplication];
    
    
}

-(void)showCaptureVideoView:(id)sender{
    
    //make attachedWindowsCOntroller display capture video window
    [self.rootController displayVideoCaptureWindow];
    
    
    
}




-(IBAction)showPreferencesWindow:(id)sender{
    
    /*
    BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[BoxSimpleShareAppDelegate sharedDelegate];
    
    //hide window
    
    [self hideWindow];
    
    //make partial screenshot
    
    delegate.window.isVisible = true;
    */
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"ShowPreferencesWindowNotification" object:nil];
    
    
   // NSLog(@"Show preferences window");
    
    
    
    
}


-(IBAction)uploadFile:(id)sender{
    
    BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[BoxSimpleShareAppDelegate sharedDelegate];
    
    //hide window
    
    [self hideWindow];
    
    //make partial screenshot
    
    [delegate selectAndUploadFile:nil];
    
    
    
}

-(IBAction)sendFeedback:(id)sender{
    
    BoxSimpleShareAppDelegate *delegate = [BoxSimpleShareAppDelegate sharedDelegate];
    
    
    [delegate sendFeedback:nil];
    
    
}


-(IBAction)takeScreenShot:(id)sender{
    
    
    BoxSimpleShareAppDelegate *delegate = (BoxSimpleShareAppDelegate *)[BoxSimpleShareAppDelegate sharedDelegate];
    
    //hide window
    
    [self hideWindow];
    
    //make partial screenshot
    
    [delegate startImageCapture:nil];
    
    
    //NSLog(@"Take screenshot!");
      
    
}






@end
