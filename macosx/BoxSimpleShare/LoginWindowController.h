//
//  LoginWindowController.h
//  Agency Machine
//
//  Created by Cuongnq on 2/5/14.
//  Copyright (c) 2014 Chipmunk. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
@interface LoginWindowController : NSWindowController
{

    NSMutableData* receivedData;
    IBOutlet WebView* webview;
    IBOutlet NSProgressIndicator* progress;
}


-(void)reset;
-(void)logout:(NSString*)token;
@end
