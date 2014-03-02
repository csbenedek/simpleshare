//
//  GrantAccessWindowController.h
//  BoxSimpleShare
//
//  Created by Cuongnq on 1/20/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
@interface GrantAccessWindowController : NSWindowController
{
    
    IBOutlet WebView* webview;
}

-(void)loadUrl:(NSString*) url;
@end
