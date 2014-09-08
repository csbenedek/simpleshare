//
//  FeedbackWindowController.m
//  BoxSimpleShare
//
//  Created by Cuongnq on 2/24/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "FeedbackWindowController.h"

@interface FeedbackWindowController ()

@end

@implementation FeedbackWindowController

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

-(void)awakeFromNib
{
    [self.window setLevel:NSModalPanelWindowLevel];
}

-(IBAction)cancel:(id)sender
{
    [self.window close];
}

-(void)showWindow:(id)sender
{
    [subject setStringValue:@""];
    [textView setString:@""];
    [super showWindow:sender];
}
-(IBAction)sendMail:(id)sender
{
    NSString* email =@"ajdunn83@gmail.com";
    NSAttributedString* textAttributedString = textView.attributedString;
    NSString* mailtoLink = [NSString
                            stringWithFormat:@"mailto:%@?subject=%@&body=%@", email, @"Feedback for SimpleShare", textView.string];
                            
                            // This creates a URL string by adding percent escapes. Since the URL is
                            // just being used locally, I don't know if this is always necessary,
                            // however I thought it would be a good idea to stick to standards.
                            NSURL *url = [NSURL URLWithString:[(NSString*)
                                                               CFURLCreateStringByAddingPercentEscapes(NULL, (CFStringRef)mailtoLink,
                                                                                                       NULL, NULL, kCFStringEncodingUTF8) autorelease]];
    
    [[NSWorkspace sharedWorkspace] openURL:url];
    //NSURL* tempFileURL = [NSURL URLWithString:@""]; // write the attachment to that file
    
    //NSSharingService* mailShare = [NSSharingService sharingServiceNamed:NSSharingServiceNameComposeEmail];
    
   // NSArray* shareItems = @[textAttributedString, url];
    //[mailShare performWithItems:shareItems];
    
    [self.window performSelector:@selector(close) withObject:nil afterDelay:2.0];
}


@end
