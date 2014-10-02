

//
//  LoginWindowController.m
//  Agency Machine
//
//  Created by Cuongnq on 2/5/14.
//  Copyright (c) 2014 Chipmunk. All rights reserved.
//


#import <WebKit/WebKit.h>
#import "LoginWindowController.h"
#import "GDataXMLNode.h"
#import "JSON.h"
#import "AppConstants.h"

static NSString* clientId = @"807165422668-bhms83kuijpsarl5jips11tkpf3jpq3b.apps.googleusercontent.com";//
static NSString* secret   =  @"hzF4inZYbVKl5pfWBfX_6UwK";
static NSString* callback =   @"https://localhost:1111/oauth2callback";
static NSString* scope    = @"https://www.googleapis.com/auth/youtube+https://www.googleapis.com/auth/userinfo.email";

static NSString *visibleactions = @"http://schemas.google.com/AddActivity";

@interface LoginWindowController ()

@end

@implementation LoginWindowController

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
}

-(void)awakeFromNib
{
    [self.window setLevel:NSModalPanelWindowLevel];
  
    NSString *url = [NSString stringWithFormat:@"https://accounts.google.com/o/oauth2/auth?response_type=code&client_id=%@&redirect_uri=%@&scope=%@&max_auth_age=0&access_type=offline&approval_prompt=force",clientId,callback,scope];
    
    [webview setMainFrameURL:url];
}

#pragma  webview delegate 





-(void)displayProgress:(BOOL)display
{
    if (display) {
        [progress setHidden:NO];
        [progress startAnimation:self];
    }
    else{
        [progress setHidden:YES];
        [progress stopAnimation:self];
    }
}
- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
    [self displayProgress:YES];
}

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
    [self displayProgress:NO];
}



-(void)webView:(WebView *)webView didReceiveServerRedirectForProvisionalLoadForFrame:(WebFrame *)frame
{
    NSURL * url =[[[frame provisionalDataSource] request] URL];
    
    NSLog(@"%@", url);
     if ([[url absoluteString] hasPrefix:callback]) {
  
         // Extract oauth_verifier from URL query
         NSString* verifier = nil;
         NSArray* urlParams = [[url query] componentsSeparatedByString:@"&"];
         for (NSString* param in urlParams) {
             NSArray* keyValue = [param componentsSeparatedByString:@"="];
             NSString* key = [keyValue objectAtIndex:0];
             if ([key isEqualToString:@"code"]) {
                 verifier = [keyValue objectAtIndex:1];
                 NSLog(@"verifier %@",verifier);
                 break;
             }
         }
  
         if (verifier) {
  
             NSString *data = [NSString stringWithFormat:@"code=%@&client_id=%@&client_secret=%@&redirect_uri=%@&grant_type=authorization_code", verifier,clientId,secret,callback];
             NSString *url = [NSString stringWithFormat:@"https://accounts.google.com/o/oauth2/token"];
             NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:url]];
             [request setHTTPMethod:@"POST"];
             [request setHTTPBody:[data dataUsingEncoding:NSUTF8StringEncoding]];
  
  
             NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:request delegate:self];
        receivedData = [[NSMutableData alloc] init];
            
             
         }
     }
    
    
}
-(void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
    [self displayProgress:NO];
}


-(NSDictionary*) validateAccessToken :(NSString*)xmlString
{
    NSError* error = nil;
    GDataXMLElement* element = [[GDataXMLElement alloc] initWithXMLString:xmlString error:&error];
    if (!error) {
        
        NSString* json = element.stringValue;
        NSDictionary* dict = [json JSONValue];
        if ([[dict allKeys] containsObject:@"apikey"]) {
            return dict; //[dict objectForKey:@"apikey"];
        }
    }
    return nil;
}


- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data

{
    [receivedData appendData:data];
    NSLog(@"verifier %@",receivedData);
}
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error{
    NSAlert *alert = [[NSAlert alloc]  init];
    [alert setMessageText:[NSString stringWithFormat:@"%@", error]];
                      
    [alert runModal];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
    NSString *response = [[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding];
    NSDictionary* dictionary = [response JSONValue];
    if (dictionary != nil && [dictionary objectForKey:@"access_token"]) {
       
        NSError* err = nil;
        NSString* infoUrl = [NSString stringWithFormat:@"https://www.googleapis.com/oauth2/v1/userinfo?access_token=%@", [dictionary objectForKey:@"access_token"]];
        NSURLRequest* request = [NSURLRequest requestWithURL:[NSURL URLWithString:infoUrl]];
        
        NSMutableDictionary* userInfo = [NSMutableDictionary dictionaryWithDictionary:dictionary];
        NSData* emailData = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:&err];
        
        if (err == nil) {
            
            NSString *infoResponse= [[NSString alloc] initWithData:emailData encoding:NSUTF8StringEncoding];
            NSDictionary* jsonDict =[infoResponse JSONValue];
            NSString* userEmail = [jsonDict objectForKey:@"email"];
            if (userEmail) {
                
                [userInfo setObject:userEmail forKey:@"email"];
                [userInfo setObject:[jsonDict objectForKey:@"picture"] forKey:@"picture"];
                [userInfo setObject:[NSDate date] forKey:@"expireDate"];
                PostNotificationWithObject(YOUTUBE_LOGIN, userInfo);
                [self.window close];
            }
        }
        
    }
    DbgLog (@"%@",response);
    
    
}



#pragma animation

-(void)showWindow:(id)sender {
    
    [self doAnimateShow];
        
    
}
-(void)doAnimateShow
{
    [self.window setAlphaValue:0.0];
    
    [self.window orderFront:self];
    NSRect _saveRect = [self.window  frame];
    NSRect _zeroRect = _saveRect;
    
    float transition = 1.0;

//    int paletteTransitionFadeTime = gForm->globalSettings->paletteTransitionFadeTime;
//    if (paletteTransitionFadeTime == 0) {
//        transition= .25;
//    }else if(paletteTransitionFadeTime == 1)
//    {
//        transition = .5;
//    }else if (paletteTransitionFadeTime == 2) {
//        
//        transition = .75;
//    }else {
//        transition = 1.0;
//    }
    //0=0.25, 1=0.5, 2=0.75, 3=1.0
//    
//    if (gForm->globalSettings->paletteTransition == 2) {
//        _zeroRect.size = NSMakeSize(0, 0);
//        _zeroRect.origin.x += (self.window.frame.size.width - _zeroRect.size.width) / 2;
//        _zeroRect.origin.y += (self.window.frame.size.height - _zeroRect.size.height) / 2;
//        
//    }
    
    NSDictionary *fadeInAttrs = [NSDictionary dictionaryWithObjectsAndKeys:
                                 self.window, NSViewAnimationTargetKey,
                                 NSViewAnimationFadeInEffect, NSViewAnimationEffectKey,
                                 [NSValue valueWithRect:_zeroRect], NSViewAnimationStartFrameKey,
                                 [NSValue valueWithRect:_saveRect], NSViewAnimationEndFrameKey,
                                 nil];
    
    NSViewAnimation *_viewAnimIn = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects: fadeInAttrs, nil]];
    
    [_viewAnimIn setDuration:transition];
    [_viewAnimIn setAnimationCurve:NSAnimationEaseInOut];
    [_viewAnimIn setAnimationBlockingMode:NSAnimationBlocking];
    [_viewAnimIn setDelegate:self];
    [_viewAnimIn startAnimation];
    
    
}

-(BOOL) animationShouldStart:(NSAnimation *)animation {
    return YES;
    
}
- (void)animationDidEnd:(NSAnimation *)animation
{
    // [[self.window.contentView subviews] setValue:[NSNumber numberWithBool:NO] forKey:@"hidden"];
    [self.window setAlphaValue:1.0];
    NSLog(@"end");
}


-(void) doFadeInAnimation {
    
    [self.window orderFront:self];
    [self.window setAlphaValue:0.0];
    [NSAnimationContext beginGrouping];
    [[NSAnimationContext currentContext] setDuration: 1.0];
    
    
    //[[mainWindow animator] setFrame: newWindowFrame]; --Doesn't work??
    [self.window setAlphaValue:1.0];
    [NSAnimationContext endGrouping];
    
}

- (void)fadeOutWindow:(NSWindow*)window{
	float alpha = 1.0;
	[window setAlphaValue:alpha];
	[window makeKeyAndOrderFront:self];
	for (int x = 0; x < 10; x++) {
		alpha -= 0.1;
		[window setAlphaValue:alpha];
		[NSThread sleepForTimeInterval:0.020];
	}
    [window close];
}
-(void)reset
{
   // [webview setMainFrameURL:@"about:blank"];
   
    NSString *url = [NSString stringWithFormat:@"https://accounts.google.com/o/oauth2/auth?response_type=code&client_id=%@&redirect_uri=%@&scope=%@&max_auth_age=0&access_type=offline",clientId,callback,scope];
    
    [webview setMainFrameURL:url];

}

-(void)logout:(NSString *)token
{
    NSString *url = [NSString stringWithFormat:@"https://accounts.google.com/o/oauth2/revoke?token=%@",token];
    [webview setMainFrameURL:url];
}
@end










