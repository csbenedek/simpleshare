//
//  FeedbackWindowController.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 2/24/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface FeedbackWindowController : NSWindowController
{
    IBOutlet NSTextField* subject;
    IBOutlet NSTextView* textView;
}

-(IBAction)sendMail:(id)sender;
-(IBAction)cancel:(id)sender;
@end
