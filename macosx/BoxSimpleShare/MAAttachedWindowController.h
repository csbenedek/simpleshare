//
//  MAAttachedWindowCOntroller.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 18/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class MAAttachedWindow;
@class AttachedWindowsController;

@interface MAAttachedWindowController : NSViewController <NSWindowDelegate>


@property (nonatomic, retain) MAAttachedWindow * window;
@property (nonatomic, assign) AttachedWindowsController *rootController;


-(void)showWindow;

-(void)hideWindow;

-(void)configureAttachedWindow:(MAAttachedWindow *)window;

-(NSPoint)getLocationOfStatusItem;



@end
