//
//  TextMessageViewController.h
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 05/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface TextMessageViewController : NSViewController

@property(nonatomic,retain) IBOutlet NSTextField *textField;

@property (nonatomic, retain) IBOutlet NSView *startView;


@property (nonatomic, retain) IBOutlet NSView *successfulLoginView;






@end
