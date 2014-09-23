//
//  PreferencesController.m
//  BoxSimpleShare
//
//  Created by Sergey Plotnikov on 19/09/14.
//  Copyright (c) 2014 SAPLogix. All rights reserved.
//

#import "PreferencesController.h"
#import "MainPreferencesViewController.h"
#import "AccountViewController.h"
#import "ShortcutsViewController.h"
#import "BoxNetAccount.h"


@interface PreferencesController ()

@end

@implementation PreferencesController

-(void)awakeFromNib{
    
    //init BoxNetAccount
    self.account = [[BoxNetAccount alloc] init];
    
    NSLog(@"Iniiiit");
    
}


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
    
    [self mainPreferencesToolbarItemAction:nil];
}

-(void)processLoadAccountInfoNotification{
    
    
    [self.account populateInfo]; 
    
}

-(void)processShowPreferencesWindowNotification{
    
    //NSLog(@"Show preferences window notification!");
    
    [self.window makeKeyAndOrderFront:self];
    
    //[self.window orderFront:self];
    
}

#pragma mark - NSWindow delegate methods

-(void)windowDidResignKey:(NSNotification *)notification{
    
    NSLog(@"Window did resign key preferences");
    
    [self.window orderOut:self];
    
}

#pragma mark - menu toolbar items actions

-(IBAction)mainPreferencesToolbarItemAction:(id)sender{
    //set window title
    
    [self.window setTitle:@"Preferences"];
    
    
    
    //select toolbar item
    [self.toolbar setSelectedItemIdentifier:@"net.box.preferencesView"];
    
    //load mainPreferences view
    
    if (!self.mainPreferencesController) {
        
        self.mainPreferencesController = [[MainPreferencesViewController alloc] initWithNibName:@"MainPreferencesViewController" bundle:nil];
        
    }
    
    [self displayView:self.mainPreferencesController.view];
    
    
    
    
    NSLog(@"Main toolbar item action.");
    
}

-(IBAction)accountToolbarItemAction:(id)sender{
    
    [self.window setTitle:@"Accounts"];
    
    
    //load Account view
    
    if (!self.accountViewController) {
        
        self.accountViewController = [[AccountViewController alloc] initWithNibName:@"AccountViewController" bundle:nil];
        
        //set pairing reference
        
        self.accountViewController.rootController = self;
        
        //trigger lazy loading
        
        NSView *view = self.accountViewController.view;
        
        
        
        [self.accountViewController.loginTextField setTitleWithMnemonic:self.account.login];
        
        
        
    }
    
    
    [self displayView:self.accountViewController.view];
    
    
    
    
    NSLog(@"Accounts toolbar item action.");
    
    
}


-(void) displayView:(NSView *)view{
    
    
    //get size of view to be displayed
    NSRect viewRect = view.frame;
    
    NSRect windowRect = self.window.frame;
    
    //needed for holding window position while resizing
    
    NSInteger deltaY = 0;
    
    
    if (self.activeView) {
        
        NSRect oldViewrect = self.activeView.frame;
        
        deltaY = viewRect.size.height - oldViewrect.size.height;
        
        
        [self.activeView removeFromSuperview];
        
    }
    
    NSRect newWindowRect = NSMakeRect(windowRect.origin.x, windowRect.origin.y - deltaY, windowRect.size.width, 77 + viewRect.size.height);
    
    NSRect newViewRect = NSMakeRect(0, 0, viewRect.size.width, viewRect.size.height);
    
    //resize window
    [self.window setFrame:newWindowRect display:TRUE];
    
    //add subview
    
    [self.window.contentView addSubview:view];
    
    [view setFrame:newViewRect];
    
    //set as active view
    
    self.activeView = view;
    
    
}

-(IBAction)shortcutsToolbarItemAction:(id)sender{
    
    [self.window setTitle:@"Shortcuts"];
    
    if (!self.shortcutsViewController) {
        
        self.shortcutsViewController = [[ShortcutsViewController alloc] initWithNibName:@"ShortcutsViewController" bundle:nil];
        
    }
    
    [self displayView:self.shortcutsViewController.view];
    
    
    NSLog(@"Shortcuts toolbar item action.");
    
}



@end
