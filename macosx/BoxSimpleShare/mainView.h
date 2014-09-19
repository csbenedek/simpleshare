//
//  mainView.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 12/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MAAttachedWindowController.h"

@class BoxSimpleShareAppDelegate;
@class AttachedWindowsController;
@class MAAttachedWindow;



@interface mainView : MAAttachedWindowController <NSTableViewDelegate, NSTableViewDataSource>

@property(nonatomic, retain) IBOutlet NSTextField *fileName;

@property(nonatomic, retain) IBOutlet NSTextField *shareDate;

@property(nonatomic, retain) IBOutlet NSImageView *image;


@property(nonatomic, retain) IBOutlet NSTableView *tableView;


@property(nonatomic, assign)NSMutableArray *historyElements;


@property (nonatomic, retain)NSMutableArray *cells;

@property(nonatomic, retain)MAAttachedWindow *window;



//weak refernce to AttachedWindowsController

@property(nonatomic, assign)AttachedWindowsController *rootController;




-(IBAction)quitApp:(id)sender;




-(void)updateHistoryElements;

//actions
-(IBAction)showCaptureVideoView:(id)sender;
-(IBAction)takeScreenShot:(id)sender;







@end
