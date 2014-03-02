//
//  AppDelegate.m
//  LoginHelper
//
//  Created by Cuongnq on 10/11/12.
//  Copyright (c) 2012 Cuongnq. All rights reserved.
//

#import "AppDelegate.h"
@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//    NSURLRequest* request = [[NSURLRequest alloc] initWithURL:[NSURL URLWithString:@"http://google.com"]];
//    [[_webView mainFrame]loadRequest:request];
    NSString* appPath = @"/Applications/SimpleShare.app";
    NSBundle *targetBundle = [NSBundle bundleWithPath:appPath];
    NSURL *applicationURL = [targetBundle executableURL];
    [[NSWorkspace sharedWorkspace] launchApplicationAtURL:applicationURL
                                                  options:0
                                            configuration:nil
                                                    error:nil];
    [NSApp terminate : nil];

}

@end
