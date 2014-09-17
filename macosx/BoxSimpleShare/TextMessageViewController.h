//
//  TextMessageViewController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 05/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//


#import <Cocoa/Cocoa.h>

@class MAAttachedWindow;

@interface TextMessageViewController : NSViewController <NSWindowDelegate>


@property(nonatomic,retain) IBOutlet NSTextField *textField;

@property (nonatomic, retain) IBOutlet NSView *startView;


@property (nonatomic, retain) IBOutlet NSView *successfulLoginView;


@property (nonatomic, retain) MAAttachedWindow *window;






@end
