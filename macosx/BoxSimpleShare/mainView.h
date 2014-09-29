//
//  mainView.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 12/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MAAttachedWindowController.h"
#import "SHTableView.h"

@class BoxSimpleShareAppDelegate;
@class AttachedWindowsController;
@class MAAttachedWindow;
@class ITSwitch;



@interface mainView : MAAttachedWindowController <NSTableViewDelegate, NSTableViewDataSource, ExtendedTableViewDelegate>

@property(nonatomic, retain) IBOutlet NSTextField *fileName;

@property(nonatomic, retain) IBOutlet NSTextField *shareDate;

@property(nonatomic, retain) IBOutlet NSImageView *image;

@property(nonatomic, retain) IBOutlet ITSwitch *enableUploadSwitch;





@property(nonatomic, retain) IBOutlet SHTableView *tableView;


@property(nonatomic, assign)NSMutableArray *historyElements;


@property (nonatomic, retain)NSMutableArray *cells;

@property(nonatomic, retain)MAAttachedWindow *window;



//weak refernce to AttachedWindowsController

@property(nonatomic, assign)AttachedWindowsController *rootController;








-(void)updateHistoryElements;

//-(void)addHistoryElement:



//actions
-(IBAction)showCaptureVideoView:(id)sender;
-(IBAction)takeScreenShot:(id)sender;
-(IBAction)uploadFile:(id)sender;

-(IBAction)showPreferencesWindow:(id)sender;
-(IBAction)quitApp:(id)sender;
-(IBAction)toggleEnableUploads:(id)sender;
-(IBAction)sendFeedback:(id)sender;




@end
