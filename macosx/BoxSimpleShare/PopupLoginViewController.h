//
//  PopupLoginViewController.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 6/30/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PopupLoginViewController : NSViewController
{
    
    IBOutlet NSButton* login;
}


+ (PopupLoginViewController*) shareController;
-(IBAction)performLogin:(id)sender;
@end
