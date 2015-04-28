//
//  GrantAccessWindowController.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 1/20/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "GrantAccessWindowController.h"

@interface GrantAccessWindowController ()

@end


 NSString* script = @"var element = document.getElementsByClassName('mvl');if (element != null && typeof element[0] !== 'undefined') {element[0].style.display = 'none';}";
@implementation GrantAccessWindowController

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

-(void) loadUrl:(NSString *)url {
    
    [webview setMainFrameURL:url];
}

-(void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame {
    
    [webview stringByEvaluatingJavaScriptFromString:script];
}
@end
