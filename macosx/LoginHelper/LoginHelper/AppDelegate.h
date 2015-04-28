//
//  AppDelegate.h
//  LoginHelper
//
//  Created by Cuongnq on 10/11/12.
//  Copyright (c) 2012 Cuongnq. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class WebView;
@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet WebView *webView;

@end
